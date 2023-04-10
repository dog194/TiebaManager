/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include <TBMCoreRules.h>
#include <ImageHelper.h>
#include <opencv2\imgproc.hpp>
#include <TBMCoreImageHelper.h>
#include <TBMAPI.h>


// 图片条件

CString CImageCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CImageParam&)_param;

	CString res = _T("含有图片\"") + param.m_imagePath + _T("\"，");
	CString tmp;
	switch (param.m_algorithm)
	{
	case CImageParam::EQUAL:           tmp = _T("完全相等"); break;
	case CImageParam::PSNR:            tmp.Format(_T("PSNR > %.5lf%s"), param.m_threshold, param.m_ignoreSize ? _T("，忽略尺寸") : _T("")); break;
	case CImageParam::SSIM:            tmp.Format(_T("SSIM > %.5lf%s"), param.m_threshold, param.m_ignoreSize ? _T("，忽略尺寸") : _T("")); break;
	case CImageParam::MATCH_TEMPLATE:  tmp.Format(_T("模板匹配最小差异 < %.5lf"), param.m_threshold); break;
	}
	res += tmp;
	return res;
}


CConditionParam* CImageCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CImageParam();

	COption<CString> path("Path");
	COption<int> algorithm("Algorithm", CImageParam::PSNR, InRange<int, CImageParam::EQUAL, CImageParam::MATCH_TEMPLATE>);
	COption<BOOL> ignoreSize("IgnoreSize", FALSE);
	COption<double> threshold("Threshold", 35.0);
	path.Read(*optionNode);
	algorithm.Read(*optionNode);
	ignoreSize.Read(*optionNode);
	threshold.Read(*optionNode);

	param->m_imagePath = path;
	param->m_algorithm = CImageParam::Algorithm(*algorithm);
	param->m_ignoreSize = ignoreSize;
	param->m_threshold = threshold;

	UpdateImage(*param);

	return param;
}

void CImageCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CImageParam&)_param;

	COption<CString> path("Path");
	*path = param.m_imagePath;
	path.Write(*optionNode);
	COption<int> algorithm("Algorithm");
	*algorithm = param.m_algorithm;
	algorithm.Write(*optionNode);
	COption<BOOL> ignoreSize("IgnoreSize");
	*ignoreSize = param.m_ignoreSize;
	ignoreSize.Write(*optionNode);
	COption<double> threshold("Threshold");
	*threshold = param.m_threshold;
	threshold.Write(*optionNode);
}

CConditionParam* CImageCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CImageParam&)_param;
	return new CImageParam(param);
}

void CImageCondition::UpdateImage(CImageParam& param)
{
	if (!ReadImage(param.m_imagePath, param.m_image))
	{
		AfxMessageBox(_T("加载图片失败：") + param.m_imagePath);
		return;
	}

	if (param.m_algorithm == CImageParam::EQUAL)
		return;

	// 对其他算法只比较灰度图，提高速度
	cv::Mat gray;
	cv::cvtColor(param.m_image, gray, cv::COLOR_BGR2GRAY);
	param.m_image = gray;
}


BOOL CImageCondition::MatchThread(const CConditionParam& _param, const TapiThreadInfo& thread, int& pos, int& length)
{
	return Match((CImageParam&)_param, thread);
}

BOOL CImageCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	return Match((CImageParam&)_param, post);
}

BOOL CImageCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	return Match((CImageParam&)_param, lzl);
}


static BOOL ImageEquals(const cv::Mat& I1, const cv::Mat& I2)
{
	int depth = I1.depth();
	if (I1.cols != I2.cols || I1.rows != I2.rows || depth != I2.depth())
		return FALSE;

	for (int y = 0; y < I1.rows; y++)
	{
		auto* p1 = I1.ptr<BYTE>(y);
		auto* p2 = I2.ptr<BYTE>(y);
		if (memcmp(p1, p2, I1.cols * depth) != 0)
			return FALSE;
	}
	return TRUE;
}

// I1、I2先转为灰度图
static double GetPSNR(const cv::Mat& I1, const cv::Mat& I2)
{
	if (I1.cols != I2.cols || I1.rows != I2.rows)
		return 0.0;

	cv::Mat s1;
	cv::absdiff(I1, I2, s1);   // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	cv::Scalar s = sum(s1);    // sum elements per channel

	double sse = s.val[0]; // sum channels

	if (sse <= 1e-10) // for small values return a big number
		return 999999.0;

	double mse = sse / (double)(I1.channels() * I1.total());
	double psnr = 10.0 * log10((255 * 255) / mse);
	return psnr;
}

// I1、I2先转为灰度图
static double GetMSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
	if (i1.cols != i2.cols || i1.rows != i2.rows)
		return 0.0;

	static const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	static const int d = CV_32F;
	try
	{
		cv::Mat I1, I2;
		i1.convertTo(I1, d);           // cannot calculate on one byte large values
		i2.convertTo(I2, d);

		cv::Mat I2_2 = I2.mul(I2);        // I2^2
		cv::Mat I1_2 = I1.mul(I1);        // I1^2
		cv::Mat I1_I2 = I1.mul(I2);       // I1 * I2

		/*************************** END INITS **********************************/

		cv::Mat mu1, mu2;   // PRELIMINARY COMPUTING
		cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
		I1.release();
		cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
		I2.release();

		cv::Mat mu1_2 = mu1.mul(mu1);
		cv::Mat mu2_2 = mu2.mul(mu2);
		cv::Mat mu1_mu2 = mu1.mul(mu2);
		mu1.release();
		mu2.release();

		cv::Mat sigma1_2, sigma2_2, sigma12;

		cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
		I1_2.release();
		sigma1_2 -= mu1_2;

		cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
		I2_2.release();
		sigma2_2 -= mu2_2;

		cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
		I1_I2.release();
		sigma12 -= mu1_mu2;

		///////////////////////////////// FORMULA ////////////////////////////////
		cv::Mat t1, t2, t3;

		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);               // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
		t2.release();

		cv::Mat ssim_map;
		cv::divide(t3, t1, ssim_map);  // ssim_map =  t3./t1;
		t1.release();
		t3.release();

		cv::Scalar mssim = cv::mean(ssim_map); // mssim = average of ssim map
		return mssim.val[0];
	}
	catch (...) // GaussianBlur创建Mat时可能抛出异常-215？
	{
		return 0.0;
	}
}

// I1中寻找I2，I1、I2先转为灰度图，返回最小归一化平方差
static double TemplateMatch(const cv::Mat& image, const cv::Mat& templ)
{
	if (image.cols < templ.cols || image.rows < templ.rows)
		return 999999.0;

	cv::Mat res;
	cv::matchTemplate(image, templ, res, cv::TM_SQDIFF_NORMED);
	double minVal;
	cv::minMaxLoc(res, &minVal);
	return minVal;
}

BOOL CImageCondition::Match(const CImageParam& param, const TBObject& obj)
{
	if (param.m_image.empty())
		return FALSE;

	auto& imageCache = CImageCache::GetInstance();
	std::vector<CString> urls;
	GetImageUrls(obj, urls);
	for (const auto& i : urls)
	{
		cv::Mat img;
		if (!imageCache.GetImage(i, img))
			continue;

		double cmpRes = CompareImage(param, img);
		if (cmpRes < 0.0)
			continue;

		BOOL res;
		switch (param.m_algorithm)
		{
		case CImageParam::EQUAL:
			res = cmpRes > 0.99;
			break;
		case CImageParam::PSNR:
		case CImageParam::SSIM:
			res = cmpRes > param.m_threshold;
			break;
		case CImageParam::MATCH_TEMPLATE:
			res = cmpRes < param.m_threshold;
			break;
		default:
			res = FALSE;
			break;
		}
		if (res)
			return TRUE;
	}
	return FALSE;
}

double CImageCondition::CompareImage(const CImageParam& param, const cv::Mat& img)
{
	if (param.m_image.empty() || img.empty())
		return -1.0;

	if (param.m_algorithm == CImageParam::EQUAL)
		return ImageEquals(img, param.m_image) ? 1.0 : 0.0;

	if (param.m_image.cols < 30 || img.cols < 30) // 信息量太小
		return -2.0;

	cv::Mat grayImg;
	cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);

	cv::Mat resizedParamImg = param.m_image;
	if (param.m_ignoreSize && (param.m_algorithm == CImageParam::PSNR || param.m_algorithm == CImageParam::SSIM))
	{
		if (param.m_image.total() < grayImg.total())
			cv::resize(grayImg, grayImg, param.m_image.size());
		else
			cv::resize(param.m_image, resizedParamImg, param.m_image.size());
	}

	switch (param.m_algorithm)
	{
	case CImageParam::PSNR:            return GetPSNR(grayImg, resizedParamImg);
	case CImageParam::SSIM:            return GetMSSIM(grayImg, resizedParamImg);
	case CImageParam::MATCH_TEMPLATE:  return TemplateMatch(grayImg, param.m_image);
	default: return -3.0;
	}
}


// 图片内容条件

CString CImgContentCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CImgContentParam&)_param;

	static LPCTSTR rangeDesc[] = {
	_T("图片文件头 "),
	_T("二维码识别结果 "),
	};

	CString res = rangeDesc[param.m_contentType];
	if (param.m_not)
		res += _T("不");
	res += param.m_include ? _T("含有") : _T("匹配");
	res += param.m_keyword.isRegex ? _T("正则表达式\"") : _T("\"");
	res += param.m_keyword.text;
	res += _T("\"");
	if (param.m_keyword.ignoreCase)
		res += _T("，忽略大小写");
	return res;
}

CConditionParam* CImgContentCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CImgContentParam();

	COption<int> type("Type", CImgContentParam::IMG_TYPE, InRange<int, CImgContentParam::IMG_TYPE, CImgContentParam::QR_CODE>);
	COption<BOOL> not("Not", FALSE);
	COption<BOOL> include("Include", TRUE);
	COption<BOOL> igPorti("IgPorti", TRUE);
	COption<RegexText> keyword("Keyword");
	type.Read(*optionNode);
	not.Read(*optionNode);
	include.Read(*optionNode);
	igPorti.Read(*optionNode);
	keyword.Read(*optionNode);

	param->m_contentType = CImgContentParam::ContentType(*type);
	param->m_not = not;
	param->m_include = include;
	param->m_ignorePortrait = igPorti;
	param->m_keyword = keyword;

	return param;
}

void CImgContentCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CImgContentParam&)_param;

	COption<int> range("Type");
	*range = param.m_contentType;
	range.Write(*optionNode);
	COption<BOOL> not("Not");
	*not = param.m_not;
	not.Write(*optionNode);
	COption<BOOL> include("Include");
	*include = param.m_include;
	include.Write(*optionNode);
	COption<BOOL> igPorti("IgPorti");
	*igPorti = param.m_ignorePortrait;
	igPorti.Write(*optionNode);
	COption<RegexText> keyword("Keyword");
	*keyword = param.m_keyword;
	keyword.Write(*optionNode);
}

CConditionParam* CImgContentCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CImgContentParam&)_param;
	return new CImgContentParam(param);
}

BOOL CImgContentCondition::MatchThread(const CConditionParam& _param, const TapiThreadInfo& thread, int& pos, int& length)
{
	return CImgContentCondition::Match((CImgContentParam&)_param, thread);
}

BOOL CImgContentCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	return CImgContentCondition::Match((CImgContentParam&)_param, post);
}

BOOL CImgContentCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	return CImgContentCondition::Match((CImgContentParam&)_param, lzl);
}

BOOL CImgContentCondition::Match(const CImgContentParam& param, const TBObject& obj)
{
	ILog& log = GetLog();
	std::vector<CString> urls;
	CString content, tmp;
	CString* attachedInfo = (CString*)&obj.attachedInfo;
	GetImageUrls(obj, urls, param.m_ignorePortrait);
	for (const auto& i : urls)
	{
		BOOL res;
		// 图片，或图片头获取
		switch (param.m_contentType)
		{
		case CImgContentParam::IMG_TYPE:
			content = GetImgHead(i);
			// Debug 信息
			// log.Log(i);
			// log.Log(content);
			if (content == _T("") || content.GetLength() > 3) {
				// ERROR 提示
				tmp.Format(_T("<font color=red> 图片文件头获取失败，稍后重试 </font>%s 文件头：\"%s\""), (LPCTSTR)HTMLEscape(i), (LPCTSTR)HTMLEscape(content));
				log.Log(tmp);
				continue;
			}
			*attachedInfo += _T("\r\n图片文件头：") + content;
			break;
		case CImgContentParam::QR_CODE:
			if (!GetImgContent(param, i, content)) {
				// 获取失败
				tmp.Format(_T("<font color=red> 图片处理失败，稍后重试 </font>%s, %d"), (LPCTSTR)HTMLEscape(i), param.m_contentType);
				log.Log(tmp);
				continue;
			}
			*attachedInfo += _T("\r\n二维码识别：") + content;
			// Debug 信息
			//tmp.Format(_T("Img:%s, %d"), (LPCTSTR)HTMLEscape(i), param.m_contentType);
			//log.Log(tmp);
			break;
		default:
			res = FALSE;
			break;
		}

		// 判断匹配
		int _pos, _length;
		if (param.m_include)
		{
			res = StringIncludes(content, param.m_keyword, &_pos, &_length);
		}
		else
		{
			res = StringMatchs(content, param.m_keyword);
			_length = content.GetLength();
		}
		// 取FALSE
		if (param.m_not)
			res = !res;
		if (res)
			return TRUE;
	}
	return FALSE;
}

BOOL CImgContentCondition::GetImgContent(const CImgContentParam& param, CString imgUrl, CString& content)
{
	switch (param.m_contentType)
	{
	case CImgContentParam::QR_CODE:
		return QRCodeScan(imgUrl, content);
	default: 
		return false;
	}
}


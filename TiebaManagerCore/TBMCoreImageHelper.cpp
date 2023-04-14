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
#include <TBMCoreImageHelper.h>

#include <ImageHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include <TBMCoreGlobal.h>


TBM_CORE_API void GetImageUrls(const TBObject& object, std::vector<CString>& urls, BOOL igPorti)
{
	// 1是图片地址
	static const std::wregex THREAD_IMG_REG(_T("<img .*?bpic=\"(.*?)\".*?/>"));
	// 2是图片地址
	static const std::wregex POST_IMG_REG(_T("<img .*?class=\"(BDE_Image|j_user_sign)\".*?src=\"(.*?)\".*?>"));


	if (object.m_type == TBObject::THREAD)
	{
		// 从主题预览取图片地址
		TapiThreadInfo& thread = (TapiThreadInfo&)object;

		for (std::regex_iterator<LPCTSTR> it((LPCTSTR)thread.preview, (LPCTSTR)thread.preview + thread.preview.GetLength(), THREAD_IMG_REG),
			end; it != end; ++it)
			urls.push_back((*it)[1].str().c_str());
	}
	else
	{
		// 从帖子或楼中楼获取图片
		CString content = object.GetContent();
		for (std::regex_iterator<LPCTSTR> it((LPCTSTR)content, (LPCTSTR)content + content.GetLength(), POST_IMG_REG),
			end; it != end; ++it)
			urls.push_back((*it)[2].str().c_str());
	}
	// 头像
	if (object.authorPortraitUrl != _T("") && !igPorti)
		urls.push_back(object.authorPortraitUrl);
}

// 获取图片文件，文件头  addCache 为 true 正常加入内存缓存， false 不加入，默认为true
TBM_CORE_API CString GetImgHead(CString imgUrl, const BOOL addCache)
{
	CString imgName = GetImageName(imgUrl);
	CString headInfo;
	// 从缓存中查找记录
	for (auto& i : g_pUserCache->m_imgHeadCache) {
		if (i.m_imgName == imgName) {
			// 有记录，直接返回结果
			return i.m_info;
		}
	}
	if (PathFileExists(CImageCache::CACHE_PATH + imgName))
	{
		// 从本地图片缓存文件读取文件头
		headInfo = GetLocalImgHead(CImageCache::CACHE_PATH + imgName);
	}
	else {
		// 下载图片文件头
		headInfo = HTTPGetFileHead(imgUrl);
	}
	// 校验一下结果，没问题添加到缓存
	if (headInfo != _T("") && headInfo.GetLength() < 4 && addCache) {
		g_pUserCache->m_imgHeadCache.push_back(CImgSingleInfoCache(imgName, headInfo));
	}
	return headInfo;
}

// 图片二维码识别  addCache 为 true 正常加入内存缓存， false 不加入，默认为true
TBM_CORE_API BOOL QRCodeScan(CString imgUrl, CString& content, const BOOL addCache) {
	CString imgName = GetImageName(imgUrl);
	// 从缓存中查找记录
	for (auto& i : g_pUserCache->m_imgQRCodeCache) {
		if (i.m_imgName == imgName) {
			// 有记录，直接返回结果
			content = i.m_info;
			return true;
		}
	}
	auto& imageCache = CImageCache::GetInstance();
	cv::Mat img;
	// 无缓存获取图片
	if (!imageCache.GetImage(imgUrl, img)) {
		content = _T("");
		return false;
	}
	if (QRCodeScan(img, content)) {
		if (addCache) {
			// 添加缓存
			g_pUserCache->m_imgQRCodeCache.push_back(CImgSingleInfoCache(imgName, content));
		}
		return true;
	}
	return false;
}

// 图片二维码识别
TBM_CORE_API BOOL QRCodeScan(const cv::Mat& img, CString& content)
{
	auto& imageCache = CImageCache::GetInstance();
	content = _T("");
	if (!imageCache.m_modelInit) {
		// detector 实例化失败
		return false;
	}
	auto res = imageCache.qrDetector->detectAndDecode(img);
	if (res.size() > 0) {
		int i = 1;
		CString tmp;
		for (const auto& v : res) { // UTF82W  GBK2W
			tmp.Format(_T("[%d]%s"), i++, UTF82W(v.c_str()));
			content += tmp;
		}
		return true;
	}
	else {
		// 无识别结果
		return true;
	}
}

// 图片二维码识别 本地文件
TBM_CORE_API BOOL QRCodeScanLocal(const CString path, CString& content)
{
	content = _T("");
	if (!PathFileExists(path)) {
		return false;
	}
	cv::Mat img;
	if (!ReadImage(path, img)) {
		return false;
	}
	return QRCodeScan(img, content);
}

// CImageCache

CString CImageCache::CACHE_PATH = _T("ImageCache\\");


CImageCache::CImageCache()
{
	m_cleanThread = std::make_unique<std::thread>(&CImageCache::CleanThread, this);
	try {
		qrDetector = std::make_unique<cv::wechat_qrcode::WeChatQRCode>(
			"Model\\detect.prototxt",
			"Model\\detect.caffemodel",
			"Model\\sr.prototxt",
			"Model\\sr.caffemodel"
			);
		m_modelInit = true;
	}
	catch (std::exception e) {
		AfxMessageBox(_T("二维码模型初始化失败，相关规则无法使用，应该是模型文件不在，到群问吧"));
		m_modelInit = false;
	}
}

CImageCache::~CImageCache()
{
	m_stopFlag = TRUE;
	if (m_cleanThread != nullptr && m_cleanThread->joinable())
		m_cleanThread->join();
}

void CImageCache::CleanThread()
{
	while (!m_stopFlag)
	{
		// 每30分钟清除图片缓存
		for (int i = 0; i < 30 * 60 * 1000 / 100; i++)
		{
			Sleep(100);
			if (m_stopFlag)
				break;
		}
		if (m_stopFlag)
			break;

		CFileFind fileFind;
		BOOL found = fileFind.FindFile(CACHE_PATH + _T("*"));
		while (found)
		{
			found = fileFind.FindNextFile();
			DeleteFile(fileFind.GetFilePath());
		}
	}
}


BOOL CImageCache::GetImage(CString imgUrl, cv::Mat& mat)
{
	mat.release();

	CString imgName = GetImageName(imgUrl);
	if (PathFileExists(CACHE_PATH + imgName))
	{
		// 读取图片缓存
		return ReadImage(CACHE_PATH + imgName, mat);
	}
	
	// 下载图片
	std::unique_ptr<BYTE[]> buffer;
	ULONG size;
	if (HTTPGetRaw(imgUrl, &buffer, &size) == NET_SUCCESS)
	{
		// 缓存
		CreateDir(CACHE_PATH);
		CFile file;
		if (file.Open(CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
			file.Write(buffer.get(), size);

		return ReadImage(buffer.get(), size, mat);
	}

	return FALSE;
}

BOOL CImageCache::GetImage(CString imgUrl, CImage& img)
{
	if (!img.IsNull())
		img.Destroy();

	CString imgName = GetImageName(imgUrl);
	if (PathFileExists(CACHE_PATH + imgName))
	{
		// 读取图片缓存
		return SUCCEEDED(img.Load(CACHE_PATH + imgName));
	}

	// 下载图片
	std::unique_ptr<BYTE[]> buffer;
	ULONG size;
	if (HTTPGetRaw(imgUrl, &buffer, &size) == NET_SUCCESS)
	{
		// 缓存
		CreateDir(CACHE_PATH);
		CFile file;
		if (file.Open(CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
			file.Write(buffer.get(), size);

		return ReadImage(buffer.get(), size, img);
	}

	return FALSE;
}

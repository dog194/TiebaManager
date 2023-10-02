﻿/*
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

#pragma once
#include "TiebaManagerCoreCommon.h"
#include "TBMCoreRule.h"
#include "Singleton.h"
#include "StringHelper.h"
#pragma warning(disable:4819) // OpenCV头文件包含Unicode字符
#include <opencv2\core\mat.hpp>


// 规则

class TBM_CORE_API CIllegalRule final : public CRule
{
public:
	BOOL m_forceToConfirm = FALSE;	          // 强制确认
	BOOL m_deleteIfIsLZ = FALSE;	          // 联动删除
	int m_trigCount = 0;    			      // 触发次数
};

// 条件

// 关键词条件

class TBM_CORE_API CKeywordParam final : public CConditionParam
{
public:
	enum KeywordRange
	{
		TITLE,            // 主题标题
		PREVIEW,          // 主题预览
		POST_CONTENT,     // 帖子内容
		LZL_CONTENT,      // 楼中楼内容
		AUTHOR,           // 作者名显示名，有昵称的时候为昵称，没昵称显示用户名
		ALL_CONTENT,      // 所有内容（包括主题标题、主题预览、帖子内容、楼中楼内容）
		UID,			  // 作者名，可能为空
		PORTRAIT,         // Portrait，目前版本的用户唯一标识符
		TID				  // TID, 帖子ID，可以根据帖子制定专门规则
	};


	CKeywordParam() : CConditionParam(_T("关键词条件")) { }


	KeywordRange m_range = ALL_CONTENT;      // 搜索范围
	BOOL m_not = FALSE;                      // 结果取FALSE
	BOOL m_include = TRUE;                   // TRUE为包含，FALSE为匹配
	RegexText m_keyword;                     // 关键词
};

class TBM_CORE_API CKeywordCondition final : public CCondition, public Singleton<CKeywordCondition>
{
	DECL_SINGLETON(CKeywordCondition);
private:
	CKeywordCondition() : CCondition(_T("关键词条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const TapiThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

private:
	BOOL Match(const CKeywordParam& param, const CString& content, int startPos, int& pos, int& length);
};


// 等级条件

class TBM_CORE_API CLevelParam final : public CConditionParam
{
public:
	enum Operator
	{
		LESS,             // <=
		GREATER           // >=
	};


	CLevelParam() : CConditionParam(_T("等级条件")) { }


	Operator m_operator = LESS;  // 操作符
	int m_level = 1;             // 等级
};

class TBM_CORE_API CLevelCondition final : public CCondition, public Singleton<CLevelCondition>
{
	DECL_SINGLETON(CLevelCondition);
private:
	CLevelCondition() : CCondition(_T("等级条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const TapiThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;
};


// 时间条件

class TBM_CORE_API CTimeParam final : public CConditionParam
{
public:
	enum Operator
	{
		LESS,             // <=
		GREATER           // >=
	};
	enum timeType
	{
		TIME,             // 纯时间
		DATE_TIME         // 日期+时间
	};

	CTimeParam() : CConditionParam(_T("时间条件")) { }

	Operator m_operator = GREATER;  // 操作符
	time_t m_time = time(NULL);     // Unix时间戳
	timeType m_timeType = DATE_TIME;// 时间模式，TRUE：完整时间，或，FALSE：仅时间
};

class TBM_CORE_API CTimeCondition final : public CCondition, public Singleton<CTimeCondition>
{
	DECL_SINGLETON(CTimeCondition);
private:
	CTimeCondition() : CCondition(_T("时间条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const TapiThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

private:
	BOOL Match(const CTimeParam& param, const TBObject& obj);
};


// 图片条件

class TBM_CORE_API CImageParam final : public CConditionParam
{
public:
	enum Algorithm
	{
		EQUAL,            // RGB完全相等
		PSNR,
		SSIM,             // 其实是MSSIM...
		MATCH_TEMPLATE    // 模板匹配，计算使用归一化平方差
	};


	CImageParam() : CConditionParam(_T("图片条件")) { }


	CString m_imagePath;             // 图片路径
	Algorithm m_algorithm = PSNR;    // 比较算法
	BOOL m_ignoreSize = FALSE;       // 比较不同尺寸图片前缩放，用于PSNR、SSIM
	double m_threshold = 35.0;       // 阈值，用于PSNR、SSIM、模板匹配
						     
	cv::Mat m_image;                 // 图片数据，改变m_imagePath或m_algorithm后应立即调用CImageCondition::UpdateImage
};

class TBM_CORE_API CImageCondition final : public CCondition, public Singleton<CImageCondition>
{
	DECL_SINGLETON(CImageCondition);
private:
	CImageCondition() : CCondition(_T("图片条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;
	// 读取图片数据、预处理
	void UpdateImage(CImageParam& param);

	virtual BOOL MatchThread(const CConditionParam& param, const TapiThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

	// 特殊情况返回负值
	double CompareImage(const CImageParam& param, const cv::Mat& img);

private:
	BOOL Match(const CImageParam& param, const TBObject& obj);
};

// 图片内容条件

class TBM_CORE_API CImgContentParam final : public CConditionParam
{
public:
	enum ContentType
	{
		IMG_TYPE,			// 图片类型
		QR_CODE,			// 二维码
	};

	CImgContentParam() : CConditionParam(_T("图片内容条件")) { }

	ContentType m_contentType = IMG_TYPE;    // 内容类型
	BOOL m_not = FALSE;                      // 结果取FALSE
	BOOL m_include = TRUE;                   // TRUE为包含，FALSE为匹配
	BOOL m_ignorePortrait = TRUE;			 // 忽略头像，TRUE 为忽略，FALSE为包括
	RegexText m_keyword;                     // 关键词
};

class TBM_CORE_API CImgContentCondition final : public CCondition, public Singleton<CImgContentCondition>
{
	DECL_SINGLETON(CImgContentCondition);
private:
	CImgContentCondition() : CCondition(_T("图片内容条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const TapiThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

	BOOL GetImgContent(const CImgContentParam& param, CString imgUrl, CString& content);

private:
	BOOL Match(const CImgContentParam& param, const TBObject& obj);
};

// 楼层条件

class TBM_CORE_API CFloorParam final : public CConditionParam
{
public:
	enum Operator
	{
		LESS,             // <=
		EQUAL,            // ==
		GREATER           // >=
	};


	CFloorParam() : CConditionParam(_T("楼层条件")) { }


	Operator m_operator = LESS;  // 操作符
	int m_floor = 2;             // 楼层
};

class TBM_CORE_API CFloorCondition final : public CCondition, public Singleton<CFloorCondition>
{
	DECL_SINGLETON(CFloorCondition);
private:
	CFloorCondition() : CCondition(_T("楼层条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const TapiThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

private:
	BOOL Match(const CConditionParam& param, const CString& floor);
};

// 用户信息

class TBM_CORE_API CUserInfo
{
public:
	CString m_uid;			// userName/userNickName = userShowName
	CString m_portrait;		// portrait
	int m_trigCount = 0;    // 触发次数
	CString m_note;			// 备注，没实际用途，单纯用来个人记录
	CString m_day2Free;		// 客户端接口获取全吧封禁日期

	CUserInfo();
	CUserInfo(const CString& uid);
	CUserInfo(const CString& uid, const CString& m_portrait);
	CUserInfo(const CString& uid, const CString& m_portrait, const CString& m_note);

	BOOL Match(const CString& uid, const CString& portrait);
	BOOL Match(const TapiThreadInfo& thread);
	BOOL Match(const PostInfo& post);
	BOOL Match(const LzlInfo& lzl);
};
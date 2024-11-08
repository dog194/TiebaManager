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
#include "TiebaAPICommon.h"
#include <regex>
#include <vector>


// 采集贴吧用的常量
#pragma region 用户信息
// 1是fid，2是贴吧名
const std::wregex FORUM_ID_NAME_REG(LR"#(id['"]:\s*['"]?(\d+)['"]?,.*?)#"
							        LR"#(name['"]:\s*['"](.*?)['"])#");
// 1是用户名
const std::wregex USER_NAME_REG(LR"#(name['"]:\s*['"](.*?)['"])#");
const TCHAR _TBS_LEFT[] = _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[] = _T("\"");
// 1是tbs
const std::wregex TBS_REG(LR"#(tbs['"]:\s*['"](.*?)['"])#");
#pragma endregion


class TIEBA_API_API TBObject
{
public:
	enum TBObjectType { THREAD, POST, LZL };
	const TBObjectType m_type;
	
	CString rawData;
	CString tid;		          // 主题ID
	CString author;		          // 作者
	CString authorShowName;       // 作者显示名
	CString authorID;	          // 作者ID
	CString authorPortraitUrl;	  // 作者头像URL。对于Web接口主题为空
	CString customState;		  // 虚拟形象心情
	BOOL isTidAuthor = FALSE;	  // 是否是楼主
	time_t timestamp;             // 创建时间，Unix时间戳(UTC)。对于Web接口主题为0
	CString attachedInfo = _T("");// 通用额外变量，用于存放一些额外信息传递给确认窗口

	
	TBObject(TBObjectType type) : m_type(type) { }
	virtual ~TBObject() = default;

	// 返回标题+预览或内容什么的
	virtual CString GetContent() const = 0;
};

class TIEBA_API_API TBUserObjBase
{
public:

	CString id;				// id
	CString name;			// 用户名
	CString ShowName;		// 显示名
	CString Portrait;		// 头像ID

	TBUserObjBase() { }
	virtual ~TBUserObjBase() = default;
};

class TIEBA_API_API TBUserObj : public TBUserObjBase
{
public:

	CString PortraitUrl;	// 作者头像URL。头像ID
	int level;				// 等级
	int glevel;				// 成长等级
	int is_bawu;			// 是否吧务
	CString bawu_type;		// 吧务类别
	int post_num;			// 发帖量
	CString tb_age;			// 吧龄
	int is_default_avatar;	// 是否默认头像
	CString tieba_uid;		// tieba uid
	CString virtual_info;	// 虚拟形象信息
	CString ip_address;		// ip 地址信息
	int reply_type;			// 回复模式


	virtual CString GetContent() const { 
		CString TMP,ret = _T("");
		ret += _T("id: ") + id + _T("\t用户名: ") + name + _T("\t昵称: ") + ShowName;
		TMP.Format(_T("\n头像ID: %s\n等级：%d\t是否吧务：%d-%s\n"), Portrait, level, is_bawu, bawu_type);
		ret += TMP;
		TMP.Format(_T("发帖数: %d\t吧龄：%s\t默认头像：%d\ttieba uid:%s\n"), post_num, tb_age, is_default_avatar, tieba_uid);
		ret += TMP; 
		TMP.Format(_T("虚拟形象信息: %s\n"), virtual_info);
		ret += TMP; return ret; }

	TBUserObj(){ }
	virtual ~TBUserObj() = default;
};

// 主题信息
class TIEBA_API_API TapiThreadInfo : public TBObject
{
public:
	CString reply;		    // 回复数
	CString title;		    // 标题
	CString authorLevel;	// 作者等级
	CString preview;	    // 预览
	CString lastAuthor;     // 最后回复


	TapiThreadInfo() : TBObject(THREAD) { }
	virtual ~TapiThreadInfo() = default;

	// 返回标题+预览
	virtual CString GetContent() const { return title + _T("\r\n") + preview; }
};

// 帖子信息
class TIEBA_API_API PostInfo : public TBObject
{
public:
	CString pid;			// 帖子ID
	CString floor;			// 楼层
	CString authorLevel;	// 作者等级
	CString content;		// 内容


	PostInfo() : TBObject(POST) { }
	virtual ~PostInfo() = default;

	// 返回内容
	virtual CString GetContent() const { return content; }
};

// 楼中楼信息
class TIEBA_API_API LzlInfo : public TBObject
{
public:
	CString cid;			// 楼中楼ID
	CString floor;			// 楼层
	CString authorLevel;	// 作者等级
	CString content;		// 内容


	LzlInfo() : TBObject(LZL) { }
	virtual ~LzlInfo() = default;

	// 返回内容
	virtual CString GetContent() const { return content; }
};


class AdditionalThreadInfo
{
public:
	CString src;        // 主题源码
	CStringA srcA;		// 主题源码A
	CString fid;        // 贴吧ID
	CString pageCount;  // 页数
};


class TIEBA_API_API TiebaClawer
{
public:
	virtual BOOL GetThreads(const CString& forumName, const CString& ignoreThread, std::vector<TapiThreadInfo>& threads) = 0;
	enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
	virtual GetPostsResult GetPosts(const CString& fid, const CString& tid, const CString& page, std::vector<PostInfo>& posts, 
		std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition = NULL) = 0;
	virtual GetPostsResult GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src, 
		std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition = NULL, const CStringA& srcA = "") = 0;
};

enum TiebaInterface { TIEBA_INTERFACE_WEB, TIEBA_INTERFACE_CLIENT, TIEBA_INTERFACE_CLIENT_NICKNAME};

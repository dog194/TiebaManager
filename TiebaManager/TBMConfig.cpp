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
#include "TBMConfig.h"
#include "TBMConfigPath.h"
#include "ConfigHelper.h"

// 贴吧信息缓存
CforumCache::CforumCache()
{
	m_forumID = _T("");
	m_forumName = _T("");
	m_userName = _T("");
}

CforumCache::CforumCache(const CString& forumID, const CString& forumName, const CString& userName)
{
	m_forumID = forumID;
	m_forumName = forumName;
	m_userName = userName;
	time(&m_cacheTime);
}

CforumCache::CforumCache(const CString& forumID, const CString& forumName, const CString& userName, const time_t& cacheTime)
{
	m_forumID = forumID;
	m_forumName = forumName;
	m_userName = userName;
	m_cacheTime = cacheTime;
}

// XML 读写
DECLEAR_READ(CforumCache)
{
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();	//虽然不知道做了什么，但是还是写了 = =
		return;
	}
	COption<CString> forumID("ForumID");
	COption<CString> forumName("ForumName");
	COption<CString> userName("UserName");
	COption<time_t> cacheTime("CacheTime", 0LL);
	
	forumID.Read(*optionNode);
	forumName.Read(*optionNode);
	userName.Read(*optionNode);
	cacheTime.Read(*optionNode);

	m_value.m_forumID = forumID;
	m_value.m_forumName = forumName;
	m_value.m_userName = userName;
	m_value.m_cacheTime = cacheTime;

	if (!IsValid(m_value))	//虽然不知道做了什么，但是还是写了 = =
		UseDefault();
}

DECLEAR_WRITE(CforumCache)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	tinyxml2::XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> forumID("ForumID");
	COption<CString> forumName("ForumName");
	COption<CString> userName("UserName");
	COption<time_t> cacheTime("CacheTime", 0LL);
	forumID.Read(*optionNode);
	forumName.Read(*optionNode);
	userName.Read(*optionNode);
	cacheTime.Read(*optionNode);

	*forumID = m_value.m_forumID;
	*forumName = m_value.m_forumName;
	*userName = m_value.m_userName;
	*cacheTime = m_value.m_cacheTime;
	forumID.Write(*optionNode);
	forumName.Write(*optionNode);
	userName.Write(*optionNode);
	cacheTime.Write(*optionNode);
}

// 全局配置
CGlobalConfig::CGlobalConfig() : CConfigBase("Global"),
	m_firstRun("FirstRun", TRUE),
	m_currentUser("UserName", _T("[NULL]"), [](const CString& value){ return value != _T("") && PathFileExists(USERS_DIR_PATH + value + _T("\\ck.xml")); }),
	m_autoUpdate("AutoUpdate", TRUE),
	m_scanPage("ScanPage", _T("1")),
	m_forumCache("ForumCache")
{
	m_options.push_back(&m_firstRun);
	m_options.push_back(&m_currentUser);
	m_options.push_back(&m_autoUpdate);
	m_options.push_back(&m_scanPage);
	m_options.push_back(&m_forumCache);
}

// 用户配置
CUserConfig::CUserConfig() : CConfigBase("User"),
	m_plan("Plan", _T("默认")),
	m_forumName("ForumName")
{
	m_options.push_back(&m_plan);
	m_options.push_back(&m_forumName);
}

// Cookie文件
CCookieConfig::CCookieConfig() : CConfigBase("Cookie"),
	m_cookie("Cookie")
{
	m_options.push_back(&m_cookie);
}

// 方案
CPlan::CPlan() : CTBMCoreConfig("Plan"),
	m_autoSaveLog		("AutoSaveLog",			FALSE)
{
	m_options.push_back(&m_autoSaveLog);
}

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

#include "stdafx.h"
#include <TBMCoreRules.h>
using namespace tinyxml2;


// 规则 ///////////////////////////////////////////////////////////////////////////

DECLEAR_READ(CIllegalRule)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<CRule> rule(m_name);
	COption<BOOL> forceToConfirm("ForceToConfirm", FALSE);
	COption<BOOL> deleteIfIsLZ("AceDel", FALSE);
	COption<int> trigCount("TrigCount", 0);
	rule.Read(root);
	forceToConfirm.Read(*optionNode);
	deleteIfIsLZ.Read(*optionNode);
	trigCount.Read(*optionNode);

	m_value.m_name = std::move(rule->m_name);
	m_value.m_conditionParams = std::move(rule->m_conditionParams);
	m_value.m_forceToConfirm = forceToConfirm;
	m_value.m_deleteIfIsLZ = deleteIfIsLZ;
	m_value.m_trigCount = trigCount;

	if (!IsValid(m_value))
		UseDefault();
}

DECLEAR_WRITE(CIllegalRule)
{
	COption<CRule> rule(m_name);
	*rule = m_value;
	rule.Write(root);

	XMLElement* optionNode = root.FirstChildElement(m_name);

	COption<BOOL> forceToConfirm("ForceToConfirm");
	*forceToConfirm = m_value.m_forceToConfirm;
	forceToConfirm.Write(*optionNode);
	COption<BOOL> deleteIfIsLZ("AceDel");
	*deleteIfIsLZ = m_value.m_deleteIfIsLZ;
	deleteIfIsLZ.Write(*optionNode);
	COption<int> trigCount("TrigCount");
	*trigCount = m_value.m_trigCount;
	trigCount.Write(*optionNode);
}


// 条件 ///////////////////////////////////////////////////////////////////////////

void InitRules()
{
	CCondition::AddCondition(CKeywordCondition::GetInstance());
	CCondition::AddCondition(CLevelCondition::GetInstance());
	CCondition::AddCondition(CTimeCondition::GetInstance());
	CCondition::AddCondition(CImageCondition::GetInstance());
	CCondition::AddCondition(CImgContentCondition::GetInstance());
	CCondition::AddCondition(CFloorCondition::GetInstance());
}


// 关键词条件

CString CKeywordCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CKeywordParam&)_param;
	static LPCTSTR rangeDesc[] = {
		_T("主题标题"),
		_T("主题预览"),
		_T("帖子内容"),
		_T("楼中楼内容"),
		_T("作者显示名"),
		_T("所有内容"),
		_T("作者名"),
		_T("头像ID(Portirt)"),
		_T("主题帖号(Tid)"),
		_T("虚拟形象心情")
	};
	
	CString res = rangeDesc[param.m_range];
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


CConditionParam* CKeywordCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CKeywordParam();

	COption<int> range("Range", CKeywordParam::ALL_CONTENT, InRange<int, CKeywordParam::TITLE, CKeywordParam::CUSTOM_STATE>);
	COption<BOOL> not("Not", FALSE);
	COption<BOOL> include("Include", TRUE);
	COption<RegexText> keyword("Keyword");
	range.Read(*optionNode);
	not.Read(*optionNode);
	include.Read(*optionNode);
	keyword.Read(*optionNode);

	param->m_range = CKeywordParam::KeywordRange(*range);
	param->m_not = not;
	param->m_include = include;
	param->m_keyword = keyword;

	return param;
}

void CKeywordCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CKeywordParam&)_param;

	COption<int> range("Range");
	*range = param.m_range;
	range.Write(*optionNode);
	COption<BOOL> not("Not");
	*not = param.m_not;
	not.Write(*optionNode);
	COption<BOOL> include("Include");
	*include = param.m_include;
	include.Write(*optionNode);
	COption<RegexText> keyword("Keyword");
	*keyword = param.m_keyword;
	keyword.Write(*optionNode);
}

CConditionParam* CKeywordCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CKeywordParam&)_param;
	return new CKeywordParam(param);
}


BOOL CKeywordCondition::Match(const CKeywordParam& param, const CString& content, int startPos, int& pos, int& length)
{
	// 判断匹配
	BOOL res;
	int _pos, _length;
	if (param.m_include)
	{
		res = StringIncludes(content, param.m_keyword, &_pos, &_length);
		_pos += startPos;
	}
	else
	{
		res = StringMatchs(content, param.m_keyword);
		_pos = startPos;
		_length = content.GetLength();
	}
	if (!param.m_not)
	{
		pos = _pos;
		length = _length;
	}

	// 取FALSE
	if (param.m_not)
		res = !res;

	return res;
}

BOOL CKeywordCondition::MatchThread(const CConditionParam& _param, const TapiThreadInfo& thread, int& pos, int& length)
{
	const auto& param = (CKeywordParam&)_param;

	// 判断范围
	int startPos;
	CString content;
	switch (param.m_range)
	{
	default: return FALSE;
	case CKeywordParam::TITLE:           startPos = 0;                                    content = thread.title;           break;
	case CKeywordParam::PREVIEW:         startPos = thread.title.GetLength() + 2;         content = thread.preview;         break;
	case CKeywordParam::AUTHOR:          startPos = thread.GetContent().GetLength() + 10; content = thread.authorShowName;  break;
	case CKeywordParam::ALL_CONTENT:     startPos = 0;                                    content = thread.GetContent();    break;
	case CKeywordParam::UID:             
		startPos = thread.GetContent().GetLength() + 10 + thread.authorShowName.GetLength() + 5 + thread.authorLevel.GetLength() + 8;
		content = thread.author;          
		break;
	case CKeywordParam::PORTRAIT:        
		startPos = thread.GetContent().GetLength() + 10 + thread.authorShowName.GetLength() + 5 + thread.authorLevel.GetLength() + 8 + thread.author.GetLength() + 17;
		content = GetPortraitFromString(thread.authorPortraitUrl);    
		break;
	case CKeywordParam::CUSTOM_STATE:
		startPos = thread.GetContent().GetLength() + 10 +
			thread.authorShowName.GetLength() + 5 + 
			thread.authorLevel.GetLength() + 8 +
			thread.author.GetLength() + 17 +
			GetPortraitFromString(thread.authorPortraitUrl).GetLength() + 9;
		content = thread.customState;
		break;
	case CKeywordParam::TID:
		startPos = thread.GetContent().GetLength() + 10 + 
			thread.authorShowName.GetLength() + 5 + 
			thread.authorLevel.GetLength() + 8 +
			thread.author.GetLength() + 17 + 
			GetPortraitFromString(thread.authorPortraitUrl).GetLength() + 9 +
			thread.customState.GetLength() + 15 +
			GetYYMMDD_HHMMSS_FromTimeT(thread.timestamp).GetLength() + 8;
		content = GetPortraitFromString(thread.tid);
		break;
	}

	return Match(param, content, startPos, pos, length);
}

BOOL CKeywordCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	const auto& param = (CKeywordParam&)_param;

	// 判断范围
	int startPos;
	CString content;
	switch (param.m_range)
	{
	default: return FALSE;
	case CKeywordParam::POST_CONTENT:    startPos = 0;                                    content = post.content;         break;
	case CKeywordParam::AUTHOR:          startPos = post.GetContent().GetLength() + 10;   content = post.authorShowName;  break;
	case CKeywordParam::ALL_CONTENT:     startPos = 0;                                    content = post.GetContent();    break;
	case CKeywordParam::UID:
		startPos = post.GetContent().GetLength() + 10 + 
			post.authorShowName.GetLength() + 5 + 
			post.authorLevel.GetLength() + 5 + 
			post.floor.GetLength() +  8;
		content = post.author;
		break;
	case CKeywordParam::PORTRAIT:
		startPos = post.GetContent().GetLength() + 10 + 
			post.authorShowName.GetLength() + 5 +
			post.authorLevel.GetLength() + 5 +
			post.floor.GetLength() + 8 + 
			post.author.GetLength() + 17;
		content = GetPortraitFromString(post.authorPortraitUrl);
		break;
	case CKeywordParam::CUSTOM_STATE:
		startPos = post.GetContent().GetLength() + 10 +
			post.authorShowName.GetLength() + 5 +
			post.authorLevel.GetLength() + 5 +
			post.floor.GetLength() + 8 +
			post.author.GetLength() + 17 +
			GetPortraitFromString(post.authorPortraitUrl).GetLength() + 9;
		content = post.customState;
		break;
	case CKeywordParam::TID:
		startPos = post.GetContent().GetLength() + 10 +
			post.authorShowName.GetLength() + 5 +
			post.authorLevel.GetLength() + 5 +
			post.floor.GetLength() + 8 +
			post.author.GetLength() + 17 + 
			GetPortraitFromString(post.authorPortraitUrl).GetLength() + 9 +
			post.customState.GetLength() + 15 +
			GetYYMMDD_HHMMSS_FromTimeT(post.timestamp).GetLength() + 8;
		content = GetPortraitFromString(post.tid);
		break;
	}

	return Match(param, content, startPos, pos, length);
}

BOOL CKeywordCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	const auto& param = (CKeywordParam&)_param;

	// 判断范围
	int startPos;
	CString content;
	switch (param.m_range)
	{
	default: return FALSE;
	case CKeywordParam::LZL_CONTENT:     startPos = 0;                                    content = lzl.content;          break;
	case CKeywordParam::AUTHOR:          startPos = lzl.GetContent().GetLength() + 10;    content = lzl.authorShowName;   break;
	case CKeywordParam::ALL_CONTENT:     startPos = 0;                                    content = lzl.GetContent();     break;
	case CKeywordParam::UID:
		startPos = lzl.GetContent().GetLength() + 7 + 
			lzl.authorLevel.GetLength() + 5 +
			lzl.floor.GetLength() + 10 + 
			lzl.authorShowName.GetLength() + 8;
		content = lzl.author;
		break;
	case CKeywordParam::PORTRAIT:
		startPos = lzl.GetContent().GetLength() + 7 + 
			lzl.authorLevel.GetLength() + 5 +
			lzl.floor.GetLength() + 10 + 
			lzl.authorShowName.GetLength() + 8 + 
			lzl.author.GetLength() + 17;
		content = GetPortraitFromString(lzl.authorPortraitUrl);
		break;
	case CKeywordParam::TID:
		startPos = lzl.GetContent().GetLength() + 7 +
			lzl.authorLevel.GetLength() + 5 +
			lzl.floor.GetLength() + 10 +
			lzl.authorShowName.GetLength() + 8 +
			lzl.author.GetLength() + 17 +
			GetPortraitFromString(lzl.authorPortraitUrl).GetLength() + 15 +
			GetYYMMDD_HHMMSS_FromTimeT(lzl.timestamp).GetLength() + 8;
		content = GetPortraitFromString(lzl.tid);
		break;
	case CKeywordParam::CUSTOM_STATE:
		// 虚拟形象心情 楼中楼 默认返回 FALSE
		return FALSE;
	}

	return Match(param, content, startPos, pos, length);
}


// 等级条件

CString CLevelCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CLevelParam&)_param;
	static LPCTSTR operatorDesc[] = {
		_T("等级 <= "),
		_T("等级 >= ")
	};

	CString res;
	res.Format(_T("%s%d"), operatorDesc[param.m_operator], param.m_level);
	return res;
}


CConditionParam* CLevelCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CLevelParam();

	COption<int> op("Operator", CLevelParam::LESS, InRange<int, CLevelParam::LESS, CLevelParam::GREATER>);
	COption<int> level("Level", 1, InRange<int, 1, 18>);
	op.Read(*optionNode);
	level.Read(*optionNode);

	param->m_operator = CLevelParam::Operator(*op);
	param->m_level = level;

	return param;
}

void CLevelCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CLevelParam&)_param;

	COption<int> op("Operator");
	*op = param.m_operator;
	op.Write(*optionNode);
	COption<int> level("Level");
	*level = param.m_level;
	level.Write(*optionNode);
}

CConditionParam* CLevelCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CLevelParam&)_param;
	return new CLevelParam(param);
}


BOOL CLevelCondition::MatchThread(const CConditionParam& _param, const TapiThreadInfo& thread, int& pos, int& length)
{
	const auto& param = (CLevelParam&)_param;

	if (thread.authorLevel == _T(""))
		return FALSE;

	int level = _ttoi(thread.authorLevel);
	switch (param.m_operator)
	{
	default: return FALSE;
	case CLevelParam::LESS:       return level <= param.m_level;
	case CLevelParam::GREATER:    return level >= param.m_level;
	}
}

BOOL CLevelCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	const auto& param = (CLevelParam&)_param;

	if (post.authorLevel == _T(""))
		return FALSE;

	int level = _ttoi(post.authorLevel);
	switch (param.m_operator)
	{
	default: return FALSE;
	case CLevelParam::LESS:       return level <= param.m_level;
	case CLevelParam::GREATER:    return level >= param.m_level;
	}
}

BOOL CLevelCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	const auto& param = (CLevelParam&)_param;

	if (lzl.authorLevel == _T(""))
		return FALSE;

	int level = _ttoi(lzl.authorLevel);
	switch (param.m_operator)
	{
	default: return FALSE;
	case CLevelParam::LESS:       return level <= param.m_level;
	case CLevelParam::GREATER:    return level >= param.m_level;
	}
}


// 时间条件

CString CTimeCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CTimeParam&)_param;
	static LPCTSTR operatorDesc[] = {
		_T("时间 <= "),
		_T("时间 >= ")
	};

	tm timeInfo;
	localtime_s(&timeInfo, &param.m_time);
	CString time;
	if (param.m_timeType == CTimeParam::DATE_TIME)
		_tcsftime(time.GetBuffer(100), 100, _T("%Y-%m-%d %X"), &timeInfo);
	else
		_tcsftime(time.GetBuffer(100), 100, _T("%X"), &timeInfo);
	time.ReleaseBuffer();
	return operatorDesc[param.m_operator] + time;
}


CConditionParam* CTimeCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CTimeParam();

	COption<int> op("Operator", CTimeParam::GREATER, InRange<int, CTimeParam::LESS, CTimeParam::GREATER>);
	COption<time_t> time("Time", 0LL);
	COption<int> m_dt("DT", CTimeParam::DATE_TIME, InRange<int, CTimeParam::TIME, CTimeParam::DATE_TIME>);
	op.Read(*optionNode);
	time.Read(*optionNode);
	m_dt.Read(*optionNode);

	param->m_operator = CTimeParam::Operator(*op);
	param->m_time = time;
	param->m_timeType = CTimeParam::timeType(*m_dt);

	return param;
}

void CTimeCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CTimeParam&)_param;

	COption<int> op("Operator");
	*op = param.m_operator;
	op.Write(*optionNode);
	COption<time_t> time("Time");
	*time = param.m_time;
	time.Write(*optionNode);
	COption<int> m_dt("DT");
	*m_dt = param.m_timeType;
	m_dt.Write(*optionNode);
}

CConditionParam* CTimeCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CTimeParam&)_param;
	return new CTimeParam(param);
}


BOOL CTimeCondition::Match(const CTimeParam& param, const TBObject& obj)
{
	if (obj.timestamp == 0)
		return FALSE;

	int diff;
	switch (param.m_timeType)
	{
	default: return FALSE;
	case CTimeParam::TIME:
		// 纯比较时间
		diff = GetTimeDiffInS(obj.timestamp, param.m_time);
		switch (param.m_operator)
		{
		default: return FALSE;
		case CTimeParam::LESS:       return diff >= 0;
		case CTimeParam::GREATER:    return diff <= 0;
		}
	case CTimeParam::DATE_TIME:
		switch (param.m_operator)
		{
		default: return FALSE;
		case CTimeParam::LESS:       return obj.timestamp <= param.m_time;
		case CTimeParam::GREATER:    return obj.timestamp >= param.m_time;
		}
	}
}

BOOL CTimeCondition::MatchThread(const CConditionParam& _param, const TapiThreadInfo& thread, int& pos, int& length)
{
	return Match((CTimeParam&)_param, thread);
}

BOOL CTimeCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	return Match((CTimeParam&)_param, post);
}

BOOL CTimeCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	return Match((CTimeParam&)_param, lzl);
}


// 楼层条件

CString CFloorCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CFloorParam&)_param;
	static LPCTSTR operatorDesc[] = {
		_T("楼层 <= "),
		_T("楼层 = "),
		_T("楼层 >= ")
	};

	CString res;
	res.Format(_T("%s%d"), operatorDesc[param.m_operator], param.m_floor);
	return res;
}


CConditionParam* CFloorCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CFloorParam();

	COption<int> op("Operator", CFloorParam::LESS, InRange<int, CFloorParam::LESS, CFloorParam::GREATER>);
	COption<int> floor("Floor", 2, GreaterThan<int, 1>);
	op.Read(*optionNode);
	floor.Read(*optionNode);

	param->m_operator = CFloorParam::Operator(*op);
	param->m_floor = floor;

	return param;
}

void CFloorCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CFloorParam&)_param;

	COption<int> op("Operator");
	*op = param.m_operator;
	op.Write(*optionNode);
	COption<int> floor("Floor");
	*floor = param.m_floor;
	floor.Write(*optionNode);
}

CConditionParam* CFloorCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CFloorParam&)_param;
	return new CFloorParam(param);
}


BOOL CFloorCondition::Match(const CConditionParam& _param, const CString& floor)
{
	const auto& param = (CFloorParam&)_param;

	if (floor == _T(""))
		return FALSE;

	int iFloor = _ttoi(floor);
	switch (param.m_operator)
	{
	default: return FALSE;
	case CFloorParam::LESS:       return iFloor <= param.m_floor;
	case CFloorParam::EQUAL:      return iFloor == param.m_floor;
	case CFloorParam::GREATER:    return iFloor >= param.m_floor;
	}
}

BOOL CFloorCondition::MatchThread(const CConditionParam& _param, const TapiThreadInfo& thread, int& pos, int& length)
{
	return FALSE;
}

BOOL CFloorCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	return Match(_param, post.floor);
}

BOOL CFloorCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	return Match(_param, lzl.floor);
}


// 用户信息
CUserInfo::CUserInfo()
{
	m_uid = _T("");
	m_portrait = _T("");
	m_note = _T("");
	m_day2Free = _T("");
}

CUserInfo::CUserInfo(const CString& uid)
{
	m_uid = uid;
	m_portrait = _T("");
	m_note = _T("");
	m_day2Free = _T("");
}

CUserInfo::CUserInfo(const CString& uid, const CString& portrait)
{
	m_uid = uid;
	m_portrait = portrait;
	m_note = _T("");
	m_day2Free = _T("");
}

CUserInfo::CUserInfo(const CString& uid, const CString& portrait, const CString& note)
{
	m_uid = uid;
	m_portrait = portrait;
	m_note = note;
	m_day2Free = _T("");
}

BOOL CUserInfo::Match(const CString& uid, const CString& portrait)
{
	if (m_portrait != _T("")) {
		// 优先以portrait为准，不为空就用portrait
		if (m_portrait == portrait) {
			return TRUE;
		}
	}
	else if (m_uid != _T("")){
		// portrait为空，用uid
		if (m_uid == uid) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CUserInfo::Match(const TapiThreadInfo& thread)
{
	return CUserInfo::Match(thread.author, GetPortraitFromString(thread.authorPortraitUrl));
}

BOOL CUserInfo::Match(const PostInfo& post)
{
	return CUserInfo::Match(post.author, GetPortraitFromString(post.authorPortraitUrl));
}

BOOL CUserInfo::Match(const LzlInfo& lzl)
{
	return CUserInfo::Match(lzl.author, GetPortraitFromString(lzl.authorPortraitUrl));
}

// XML 读写
DECLEAR_READ(CUserInfo)
{
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();	//虽然不知道做了什么，但是还是写了 = =
		return;
	}
	COption<CString> uid("uid");
	COption<CString> portrait("portrait");
	COption<int> trigCount("trigCount");
	COption<CString> note("note");
	COption<CString> day2free("d2f");
	uid.Read(*optionNode);
	trigCount.Read(*optionNode);
	portrait.Read(*optionNode);
	note.Read(*optionNode);
	day2free.Read(*optionNode);

	m_value.m_uid = uid;
	m_value.m_portrait = portrait;
	m_value.m_trigCount = trigCount;
	m_value.m_note = note;
	m_value.m_day2Free = day2free;

	if (!IsValid(m_value))	//虽然不知道做了什么，但是还是写了 = =
		UseDefault();
}

DECLEAR_WRITE(CUserInfo)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	tinyxml2::XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> uid("uid");
	COption<CString> portrait("portrait");
	COption<int> trigCount("trigCount");
	COption<CString> note("note");
	COption<CString> day2free("d2f");
	uid.Read(*optionNode);
	trigCount.Read(*optionNode);
	portrait.Read(*optionNode);
	note.Read(*optionNode);
	day2free.Read(*optionNode);

	*uid = m_value.m_uid;
	*portrait = m_value.m_portrait;
	*trigCount = m_value.m_trigCount;
	*note = m_value.m_note;
	*day2free = m_value.m_day2Free;
	uid.Write(*optionNode);
	portrait.Write(*optionNode);
	trigCount.Write(*optionNode);
	note.Write(*optionNode);
	day2free.Write(*optionNode);
}
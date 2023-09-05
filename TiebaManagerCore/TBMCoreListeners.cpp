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
#include "TBMCoreListeners.h"
#include <TBMCoreEvents.h>
#include <TBMCoreGlobal.h>


CTBMCoreListeners::CTBMCoreListeners()
{
	// 扫描事件

	g_checkThreadIllegalEvent.AddListener(OnCheckIllegal<TapiThreadInfo>);
	g_checkPostIllegalEvent.AddListener(OnCheckIllegal<PostInfo>);
	g_checkLzlIllegalEvent.AddListener(OnCheckIllegal<LzlInfo>);

	g_preScanThreadEvent.AddListener(OnPreScanThread);
}

template<class TbObj>
static void CTBMCoreListeners::OnCheckIllegal(const TbObj& obj, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length, CString& ruleName, int&ruleType)
{
	std::unique_lock<decltype(g_pTbmCoreConfig->m_optionsLock)> lock(g_pTbmCoreConfig->m_optionsLock);

	// 扫描帖子列表 检查信任主题
	if (obj.m_type == TBObject::THREAD) 
	{
		if (g_pTbmCoreConfig->m_trustedThreads->find(obj.tid) != g_pTbmCoreConfig->m_trustedThreads->end())
		{
			res = FALSE;
			return;
		}
	}

	// 信任规则
	for (auto& i : *g_pTbmCoreConfig->m_trustedRules)
	{
		if (i.Match(obj, pos, length))
		{
			res = FALSE;
			return;
		}
	}

	// 黑名单规则
	if (g_pTbmCoreConfig->m_blackListEnable) {
		if (g_pTbmCoreConfig->m_blackListBan || g_pTbmCoreConfig->m_blackListDelete) {
			for (auto& i : *g_pTbmCoreConfig->m_blackListRules) {
				if (i.Match(obj)) {
					++i.m_trigCount;
					forceToConfirm = g_pTbmCoreConfig->m_blackListConfirm;
					msg.Format(_T("<font color=pink> 触发用户黑名单规则 </font><a href=\"rb:%s\">%s</a><font color=pink> 备注:</font>%s"), 
						(LPCTSTR)HTMLEscape(i.m_portrait), (LPCTSTR)HTMLEscape(i.m_uid), (LPCTSTR)HTMLEscape(i.m_note));
					ruleName = i.m_uid + _T(" : ") + i.m_note;
					ruleType = RULE_TYPE_BLACK_LIST;
					res = TRUE;
					return;
				}
			}
		}
	}

	// 违规规则
	for (auto& i : *g_pTbmCoreConfig->m_illegalRules)
	{
		if (i.Match(obj, pos, length))
		{
			++i.m_trigCount;
			forceToConfirm = i.m_forceToConfirm;
			msg.Format(_T("<font color=red> 触发违规规则 </font><a href=\"rn:%s\">%s</a>"), (LPCTSTR)HTMLEscape(i.m_name), (LPCTSTR)HTMLEscape(i.m_name));
			ruleName = i.m_name;
			if (i.m_deleteIfIsLZ == TRUE && obj.isTidAuthor == TRUE)
				ruleType = RULE_TYPE_ILLEGA_RULE_DEL_LZ;
			else
				ruleType = RULE_TYPE_ILLEGA_RULE;
			res = TRUE;
			return;
		}
	}
	res = FALSE;
}

void CTBMCoreListeners::OnPreScanThread(int threadID, const TapiThreadInfo& thread, BOOL& pass)
{
	std::unique_lock<decltype(g_pTbmCoreConfig->m_optionsLock)> lock(g_pTbmCoreConfig->m_optionsLock);

	// 检查信任主题
	if (g_pTbmCoreConfig->m_trustedThreads->find(thread.tid) != g_pTbmCoreConfig->m_trustedThreads->end())
	{
		pass = FALSE;
		return;
	}
}

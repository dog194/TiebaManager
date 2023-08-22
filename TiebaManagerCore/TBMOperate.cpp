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
#include <TBMOperate.h>
#include <TBMCoreEvents.h>

#include <TiebaClawerProxy.h>
#include <TiebaOperate.h>
#include <TBMCoreGlobal.h>

#include <MiscHelper.h>
#include <StringHelper.h>

#include <Mmsystem.h>


CTBMOperate::CTBMOperate()
{
	// 线程要在任务队列初始化后启动
	m_operateThread = std::make_unique<std::thread>(&CTBMOperate::OperateThread, this);
	m_confirmThread = std::make_unique<std::thread>(&CTBMOperate::ConfirmThread, this);
}

CTBMOperate::~CTBMOperate()
{
	m_operationQueue.Stop();
	m_confirmQueue.Stop();
	if (m_operateThread != nullptr && m_operateThread->joinable())
		m_operateThread->join();
	if (m_confirmThread != nullptr && m_confirmThread->joinable())
		m_confirmThread->join();
}


// 添加确认
void CTBMOperate::AddConfirm(Operation&& op)
{
	if (op.ruleType == RULE_TYPE_ILLEGA_RULE && (g_pTbmCoreConfig->m_confirm || op.forceToConfirm)) {
		m_confirmQueue.push(std::move(op));
		g_comfirmQueneChangeEvent(m_confirmQueue.size());
	}
	else if (op.ruleType == RULE_TYPE_BLACK_LIST && op.forceToConfirm) {
		m_confirmQueue.push(std::move(op));
		g_comfirmQueneChangeEvent(m_confirmQueue.size());
	}
	else
		AddOperation(std::move(op));
}

// 确认线程
void CTBMOperate::ConfirmThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	Operation op;
	while (true)
	{
		PopResult res = m_confirmQueue.pop(op);
		g_comfirmQueneChangeEvent(m_confirmQueue.size());
		if (res == POP_STOP)
			break;
		if (res == POP_UNEXPECTED)
			continue;

		if (m_confirmQueue.size() <= 0) {
			// 确认队列为0 清空临时规则
			g_pUserCache->m_tempIgnoreRule.clear();
		}

		// 没有操作
		if (op.ruleType == RULE_TYPE_ILLEGA_RULE)
			if (!g_pTbmCoreConfig->m_delete && !g_pTbmCoreConfig->m_banID && !g_pTbmCoreConfig->m_defriend)
				continue;
		if (op.ruleType == RULE_TYPE_BLACK_LIST)
			if (!g_pTbmCoreConfig->m_blackListBan && !g_pTbmCoreConfig->m_blackListDelete)
				continue;

		// 确认是否操作
		if ( (op.ruleType == RULE_TYPE_ILLEGA_RULE && (g_pTbmCoreConfig->m_confirm || op.forceToConfirm)) || 
			 (op.ruleType == RULE_TYPE_BLACK_LIST  && (g_pTbmCoreConfig->m_blackListConfirm || op.forceToConfirm)) )
		{
			BOOL res = TRUE; // 是否处理Flag

			// 是否属于本次确认队列临时忽略规则
			for (auto& i : g_pUserCache->m_tempIgnoreRule) {
				if (i.m_tid != _T("")) {
					// tid 不为空，表示按照tid忽略
					if (i.m_tid == op.object.get()->tid) {
						op.ruleType = RULE_TYPE_IGNORE_TID;
						res = FALSE;
					}
				}
				else if (i.m_portrait != _T("")) {
					// portrait 不为空，表示按照portrait忽略
					if (i.m_portrait == GetPortraitFromString(op.object.get()->authorPortraitUrl)) {
						op.ruleType = RULE_TYPE_IGNORE_POR;
						res = FALSE;
					}
				}
			}
			
			// 重新查验是否属于黑名单用户
			CString tmp_note; // 黑名单备注记录
			if (g_pTbmCoreConfig->m_blackListEnable && res) { // 功能开启
				if (g_pTbmCoreConfig->m_blackListBan || g_pTbmCoreConfig->m_blackListDelete) {
					for (auto& i : *g_pTbmCoreConfig->m_blackListRules) {
						if (i.Match(op.object->author, GetPortraitFromString(op.object->authorPortraitUrl))) {
							op.isBlUser = true;
							tmp_note = i.m_note;
							break;
						}
					}
				}
			}
			op.confirmQueneLeft = m_confirmQueue.size(); // 传递确认队列剩余数量
			// 查询当前作者缓存违规计次
			auto countIt = g_pUserCache->m_userTrigCount->find(GetPortraitFromString(op.object->authorPortraitUrl));
			BOOL hasHistory = countIt != g_pUserCache->m_userTrigCount->end();
			int count = hasHistory ? (countIt->second) : 0;
			op.ruleBreakCount = count; // 传递当前用户缓存已违规次数

			if (!res) {
				// res = FALSE
				// 临时的忽略规则
			} else if (g_pTbmCoreConfig->m_blackListEnable && (!g_pTbmCoreConfig->m_blackListConfirm) && 
				g_pTbmCoreConfig->m_blackListRecheck && op.isBlUser) { // 黑名单启用 && 没有强制确认 && 对确认窗前生效启用 && 属于黑名单
				// 黑名单对确认窗口生效
				g_pLog->Log(_T("<font color=pink>用户:") + op.object->authorShowName
					+ _T(" 备注:") + tmp_note + _T(" 已在黑名单列表，自动确认处理</font>"));
				res = TRUE;
			}
			else {
				int tmpRUleType = op.ruleType;
				g_comfirmEvent(op, res, tmpRUleType);
				op.ruleType = tmpRUleType;
				if (m_confirmQueue.size() <= 0) {
					// 确认队列为0 清空临时规则
					g_pUserCache->m_tempIgnoreRule.clear();
				}
			}
			if (!res)
			{
				// 本次确认队列忽略规则
				if (op.ruleType == RULE_TYPE_IGNORE_TID) {
					// 本次确认队列忽略 指定主题帖所有内容
					g_pLog->Log(_T("<font color='#32CD32'>本次确认队列：忽略 </font>") + HTMLEscape(op.title) + _T("<font color='#32CD32'> 主题帖所有内容</font>"));
				}
				else if (op.ruleType == RULE_TYPE_IGNORE_POR) {
					// 本次确认队列忽略 指定用户所有内容
					g_pLog->Log(_T("<font color='#32CD32'>本次确认队列：忽略 </font>") + HTMLEscape(op.object.get()->authorShowName) + _T("<font color='#32CD32'> 作者所有内容</font>"));
				}
				switch (op.object->m_type)
				{
				case TBObject::THREAD:
				{
					TapiThreadInfo* thread = (TapiThreadInfo*)op.object.get();
					g_pUserCache->m_initIgnoredTID->insert(_ttoi64(thread->tid));
					g_pLog->Log(_T("<font color=green>忽略 </font><a href=\"https://tieba.baidu.com/p/") + thread->tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					break;
				}
				case TBObject::POST:
				{
					PostInfo* post = (PostInfo*)op.object.get();
					g_pUserCache->m_initIgnoredPID->insert(_ttoi64(post->pid));
					g_pLog->Log(_T("<font color=green>忽略 </font><a href=\"https://tieba.baidu.com/p/") + post->tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + post->floor + _T("楼"));
					break;
				}
				case TBObject::LZL:
				{
					LzlInfo* lzl = (LzlInfo*)op.object.get();
					g_pUserCache->m_initIgnoredLZLID->insert(_ttoi64(lzl->cid));
					g_pLog->Log(_T("<font color=green>忽略 </font><a href=\"https://tieba.baidu.com/p/") + lzl->tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + lzl->floor + _T("楼回复"));
					break;
				}
				}
				continue;
			}
		}

		// 加入操作
		AddOperation(std::move(op));
	}

	CoUninitialize();
	
	TRACE(_T("确认线程结束\n"));
}

// 添加操作
void CTBMOperate::AddOperation(Operation&& operation)
{
	m_operationQueue.push(std::move(operation));
	g_operateQueneChangeEvent(m_operationQueue.size());
}

// 操作线程
void CTBMOperate::OperateThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	Operation op;
	while (true)
	{
		PopResult res = m_operationQueue.pop(op);
		g_operateQueneChangeEvent(m_operationQueue.size());
		if (res == POP_STOP)
			break;
		if (res == POP_UNEXPECTED)
			continue;

		// 没有操作
		if (op.ruleType == RULE_TYPE_ILLEGA_RULE)
			if (!g_pTbmCoreConfig->m_delete && !g_pTbmCoreConfig->m_banID && !g_pTbmCoreConfig->m_defriend)
				continue;
		if (op.ruleType == RULE_TYPE_BLACK_LIST)
			if (!g_pTbmCoreConfig->m_blackListBan && !g_pTbmCoreConfig->m_blackListDelete)
				continue;

		BOOL pass = TRUE;
		g_preOperateEvent(op, pass);
		if (!pass)
			continue;

		// TODO 进一步优化？
		// 校验 用户 全吧封禁状态 day to free
		if (op.ruleType == RULE_TYPE_CHECK_D2F) {
			// 校验结果
			CString d2f, u_portrait;
			u_portrait = op.ruleName;
			for (auto& i : *g_pTbmCoreConfig->m_blackListRules) {
				if (i.m_portrait == u_portrait) {
					// TODO 校验上次检测时间
					CString content, u_name;
					int d2fCode = GetUserAntiDay(u_portrait, d2f, u_name);
					if (d2f != D2F_RET_TIME_OUT) {
						// 并非超时，更新结果到列表
						content.Format(D2F_TAG_TIME, GetYYMMDD_FromTimeT());
						d2f += content;
						i.m_day2Free = d2f;
						if (d2fCode == 0) {
							content.Format(_T("<font color=green>校验封禁状态 </font>%s<font color=green> 账号正常！</font>"), (LPCTSTR)u_name);
						}
						else if (d2fCode > 0) {
							content.Format(_T("<font color=red>校验封禁状态 </font>%s<font color=red> 全吧封禁：%d</font>"), (LPCTSTR)u_name, d2fCode);
						}
						else if (d2fCode == D2F_INT_ERROR)
						{
							content.Format(_T("<font color=purple>校验封禁状态 </font>%s<font color=red> 失败！临时抽风、或参数错误</font>"), (LPCTSTR)i.m_uid);
						}
						else if (d2fCode == D2F_INT_DELETE) {
							content.Format(_T("<font color=purple>校验封禁状态 </font>%s<font color=purple> 账号注销，账号回复记录不会消失</font>"), (LPCTSTR)i.m_uid);
						}
						else {
							content.Format(_T("<font color=red>校验封禁状态 </font>%s<font color=red> 未知结果</font>"), (LPCTSTR)i.m_uid);
							DebugRecord(_T("校验封禁状态 返回结果异常"), d2fCode, _T("m_uid：") + i.m_uid);
						}
					}
					else {
						// 超时 TODO?
						content.Format(_T("<font color=red>校验封禁状态 </font>%s<font color=red> 失败！请求超时</font>"), (LPCTSTR)i.m_uid);
					}
					g_pLog->Log(content);
					break;
				}
			}
			Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
			continue;
		}

		BOOL isBan = FALSE;
		BOOL isDelete = FALSE;

		if (op.ruleType == RULE_TYPE_DELETE_ONLY) {
			isDelete = TRUE;
			g_pLog->Log(_T("<font color=Orange>使用 只删不封 模式</font>"));
		}
		else if (op.ruleType == RULE_TYPE_BAN_DIRECTLY) {
			isBan = TRUE;
			isDelete = TRUE;
			g_pLog->Log(_T("<font color=Orange>使用 立即删封 模式</font>"));
		}

		if (op.ruleType == RULE_TYPE_ILLEGA_RULE) { //只有常规违规规则有违规次数设定
			// 增加违规次数
			auto countIt = g_pUserCache->m_userTrigCount->find(GetPortraitFromString(op.object->authorPortraitUrl));
			BOOL hasHistory = countIt != g_pUserCache->m_userTrigCount->end();
			int count = hasHistory ? (countIt->second + 1) : 1;
			if (hasHistory)
				countIt->second = count;
			else
				(*g_pUserCache->m_userTrigCount)[GetPortraitFromString(op.object->authorPortraitUrl)] = 1;

			if (g_pTbmCoreConfig->m_banID && count >= g_pTbmCoreConfig->m_banTrigCount
				&& g_pUserCache->m_bannedUser->find(GetPortraitFromString(op.object->authorPortraitUrl)) == g_pUserCache->m_bannedUser->end()) { // 达到封禁违规次数且未封
				isBan = TRUE;
			}


			// 拉黑 只有常规违规有拉黑，移动到上面来，但是拉黑功能，应该到时候会被删除。
			if (g_pTbmCoreConfig->m_defriend && count >= g_pTbmCoreConfig->m_defriendTrigCount
				&& g_pUserCache->m_defriendedUser->find(op.object->author) == g_pUserCache->m_defriendedUser->end()) // 达到拉黑违规次数且未拉黑
			{
				pass = TRUE;
				g_preDefriendEvent(op, pass);
				if (pass)
				{
					BOOL result = FALSE;
					CString code = g_pTiebaOperate->Defriend(op.object->authorID);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<font color=red>拉黑 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
							_T("\"df:%s\">重试</a>"), (LPCTSTR)op.object->authorShowName, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code),
							(LPCTSTR)op.object->authorID);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						if (g_pTbmCoreConfig->m_playSound)
							sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pUserCache->m_defriendedUser->insert(op.object->author);
						g_pLog->Log(_T("<font color=red>拉黑 </font>") + op.object->authorShowName);
					}

					g_postDefriendEvent(op, result);
				}
			}
		}
		// 黑名单只有一个总开关控制
		if (op.ruleType == RULE_TYPE_BLACK_LIST && g_pTbmCoreConfig->m_blackListBan
			&& g_pUserCache->m_bannedUser->find(GetPortraitFromString(op.object->authorPortraitUrl)) == g_pUserCache->m_bannedUser->end()) {
			isBan = TRUE;
		}

		// 封禁
		if (isBan)
		{
			pass = TRUE;
			g_preBanEvent(op, pass);
			if (g_pUserCache->m_bannedUser->find(GetPortraitFromString(op.object->authorPortraitUrl)) != g_pUserCache->m_bannedUser->end()) {
				// 封禁前集中判断一次是否已经封禁，减少代码冗余。不等于，说明已经被封过了。
				pass = FALSE;
			}
			if (pass)
			{
				BOOL result = FALSE;
				CString portrait, nick_name;

				switch (op.object->m_type)
				{
				case TBObject::THREAD:
					portrait = op.object.get()->authorPortraitUrl;
					nick_name = op.object.get()->authorShowName;
					break;
				case TBObject::POST:
					portrait = ((PostInfo*)op.object.get())->authorPortraitUrl;
					nick_name = ((PostInfo*)op.object.get())->authorShowName;
					break;
				case TBObject::LZL:
					portrait = ((LzlInfo*)op.object.get())->authorPortraitUrl;
					nick_name = ((LzlInfo*)op.object.get())->authorShowName;
					break;
				}

				CString code = (g_pTbmCoreConfig->m_banClientInterface) ?
					g_pTiebaOperate->BanIDClient(op.object->author, portrait, nick_name) : g_pTiebaOperate->BanID(op.object->author, portrait, nick_name);
				if (code != _T("0"))
				{
					CString content; //需同步更新 url 传递
					content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
						_T("\"bd:%s,%s,%s,%s,Dog194\">重试</a>"), (LPCTSTR)op.object->authorShowName, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code),
						(LPCTSTR)op.object->author, (LPCTSTR)_T(""), (LPCTSTR)portrait, (LPCTSTR)nick_name);
					g_pLog->Log(content);
				}
				else
				{
					result = TRUE;
					if (g_pTbmCoreConfig->m_playSound)
						sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
					g_pUserCache->m_bannedUser->insert(GetPortraitFromString(op.object->authorPortraitUrl));
					g_pLog->Log(_T("<font color=red>封禁 </font>") + op.object->authorShowName);
				}

				g_postBanEvent(op, result);
			}
		}

		// 主题已被删则不再删帖
		__int64 tid = _ttoi64(op.object->tid);
		if (g_pUserCache->m_deletedTID.find(tid) != g_pUserCache->m_deletedTID.end()) {
			continue;
		}

		// 删帖
		if (op.ruleType == RULE_TYPE_ILLEGA_RULE && g_pTbmCoreConfig->m_delete)
			isDelete = TRUE;
		else if (op.ruleType == RULE_TYPE_BLACK_LIST  && g_pTbmCoreConfig->m_blackListDelete)
			isDelete = TRUE;

		if (isDelete)
		{
			pass = TRUE;
			g_preDeleteEvent(op, pass);
			if (pass)
			{
				BOOL result = FALSE;
				if (op.object->m_type == TBObject::THREAD) // 主题
				{
				CaseThread:
					CString code = g_pTiebaOperate->DeleteThread(op.object->tid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<a href=\"https://tieba.baidu.com/p/%s\">%s</a><font color=red> 删除失败！错误代码：%s(%s)</font><a href=")
							_T("\"dt:%s\">重试</a>"), (LPCTSTR)op.object->tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)code,
							(LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.object->tid);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						if (g_pTbmCoreConfig->m_playSound)
							sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pUserCache->m_deletedTID.insert(_ttoi64(op.object->tid));
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"https://tieba.baidu.com/p/") + op.object->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					}
				}
				else if (op.object->m_type == TBObject::POST) // 帖子
				{
					PostInfo* post = (PostInfo*)op.object.get();
					if (post->floor == _T("1"))
						goto CaseThread;

					CString code = g_pTiebaOperate->DeletePost(op.object->tid, post->pid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<a href=\"https://tieba.baidu.com/p/%s\">%s</a> %s楼<font color=red> 删除失败！错误代码：%s(%s)</font>")
							_T("<a href=\"dp:%s,%s\">重试</a>"), (LPCTSTR)op.object->tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)post->floor,
							(LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.object->tid, (LPCTSTR)post->pid);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						if (g_pTbmCoreConfig->m_playSound)
							sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"https://tieba.baidu.com/p/") + op.object->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + post->floor + _T("楼"));
					}
				}
				else if (op.object->m_type == TBObject::LZL) // 楼中楼
				{
					LzlInfo* lzl = (LzlInfo*)op.object.get();
					CString code = g_pTiebaOperate->DeleteLZL(op.object->tid, lzl->cid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<a href=\"https://tieba.baidu.com/p/%s\">%s</a> %s楼回复<font color=red> 删除失败！错误代码：")
							_T("%s(%s)</font><a href=\"dl:%s,%s\">重试</a>"), (LPCTSTR)lzl->tid, (LPCTSTR)HTMLEscape(op.title),
							(LPCTSTR)lzl->floor, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)lzl->tid, (LPCTSTR)lzl->cid);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						if (g_pTbmCoreConfig->m_playSound)
							sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"https://tieba.baidu.com/p/") + lzl->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + lzl->floor + _T("楼回复"));
					}
				}
				g_postDeleteEvent(op, result);
			}
		}
		if (isDelete || isBan) {
			Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
		}
	}

	CoUninitialize();

	TRACE(_T("操作线程结束\n"));
}

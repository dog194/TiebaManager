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
			BOOL res = TRUE;
			// 重新查验是否属于黑名单用户 
			if (g_pTbmCoreConfig->m_blackListEnable) { // 功能开启
				if (g_pTbmCoreConfig->m_blackListBan || g_pTbmCoreConfig->m_blackListDelete) {
					for (auto& i : *g_pTbmCoreConfig->m_blackListRules) {
						if (i.Match(op.object->author, GetPortraitFromString(op.object->authorPortraitUrl))) {
							op.isBlUser = true;
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

			// 黑名单对确认窗口生效
			if (g_pTbmCoreConfig->m_blackListEnable && (!g_pTbmCoreConfig->m_blackListConfirm) && 
				g_pTbmCoreConfig->m_blackListRecheck && op.isBlUser) { // 黑名单启用 && 没有强制确认 && 对确认窗前生效启用 && 属于黑名单
				g_pLog->Log(_T("<font color=pink>用户: ") + op.object->authorShowName
					+  _T(" 已在黑名单列表，自动确认处理</font>"));
				res = TRUE;
			}
			else {
				int tmpRUleType = op.ruleType;
				g_comfirmEvent(op, res, tmpRUleType);
				op.ruleType = tmpRUleType;
			}
			if (!res)
			{
				switch (op.object->m_type)
				{
				case TBObject::THREAD:
				{
					ThreadInfo* thread = (ThreadInfo*)op.object.get();
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
				CString pid, portrait, nick_name;
				// 不管是不是客户端接口封，都获取 PID

				switch (op.object->m_type)
				{
				case TBObject::THREAD:
				{
					std::vector<PostInfo> posts;
					std::vector<LzlInfo> lzls;
					TiebaClawerProxy::GetInstance().GetPosts(g_pTiebaOperate->GetForumID(), op.object->tid, _T("1"), posts, lzls);
					if (posts.size() > 0){
						pid = posts[0].pid;
						portrait = posts[0].authorPortraitUrl;
						nick_name = posts[0].authorShowName;
					}		
					break;
				}
				case TBObject::POST:
					pid = ((PostInfo*)op.object.get())->pid;
					portrait = ((PostInfo*)op.object.get())->authorPortraitUrl;
					nick_name = ((PostInfo*)op.object.get())->authorShowName;
					break;
				case TBObject::LZL:
					pid = ((LzlInfo*)op.object.get())->cid;
					portrait = ((LzlInfo*)op.object.get())->authorPortraitUrl;
					nick_name = ((LzlInfo*)op.object.get())->authorShowName;
					break;
				}

				if (pid == _T(""))
					g_pLog->Log(_T("<font color=red>封禁 </font>") + op.object->authorShowName + _T("<font color=red> 失败！(获取帖子ID失败)</font>"));

				CString code = (g_pTbmCoreConfig->m_banClientInterface || pid == _T("")) ?
					g_pTiebaOperate->BanIDClient(op.object->author, pid, portrait, nick_name) : g_pTiebaOperate->BanID(op.object->author, pid, portrait, nick_name);
				if (code != _T("0"))
				{
					CString content; //需同步更新 url 传递
					content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
						_T("\"bd:%s,%s,%s,%s,Dog194\">重试</a>"), (LPCTSTR)op.object->authorShowName, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code),
						(LPCTSTR)op.object->author, (LPCTSTR)pid, (LPCTSTR)portrait, (LPCTSTR)nick_name);
					g_pLog->Log(content);
				}
				else
				{
					result = TRUE;
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
						sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pUserCache->m_deletedTID.insert(_ttoi64(op.object->tid));
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"https://tieba.baidu.com/p/") + op.object->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
						Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
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
						sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"https://tieba.baidu.com/p/") + op.object->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + post->floor + _T("楼"));
						Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
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
						sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"https://tieba.baidu.com/p/") + lzl->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + lzl->floor + _T("楼回复"));
						Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
					}
				}

				g_postDeleteEvent(op, result);
			}
		}
	}

	CoUninitialize();

	TRACE(_T("操作线程结束\n"));
}

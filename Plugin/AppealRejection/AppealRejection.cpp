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

// AppealRejection.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "AppealRejection.h"

#include <TBMAPI.h>
#include <TBMEvents.h>
#include <TBMCoreEvents.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include <TiebaClawer.h>
#include <TBMCoreGlobal.h>

#include <document.h>

using namespace std::placeholders;
using namespace rapidjson;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CAppealRejection::CAppealRejection(HMODULE module) :
	m_module(module)
{
	g_mainDialogPostInitEvent.AddListener(std::bind(&CAppealRejection::Init, this), m_module);
}

CAppealRejection::~CAppealRejection()
{
	Uninit();
}

void CAppealRejection::Init()
{
	auto plugin = GetPlugin(m_module);
	if (plugin == NULL)
		return;

	m_version = GetFileVersionString(plugin->m_path, _T("ProductVersion"));
	plugin->m_description = _T("申诉驳回插件\r\n")
							_T("\r\n")
							_T("作者：Dog194")
							_T("\r\n")
							_T("版本：") + m_version;
	plugin->m_onConfig = std::bind(&CAppealRejection::OnConfig, this);

	// 注册监听
	g_postSetTiebaEvent.AddListener(std::bind(&CAppealRejection::OnPostSetTieba, this, _1), m_module);
	
	// 每8小时开始处理一次
	std::thread([this]{
		while (true)
		{
			Sleep(8 * 60 * 60 * 1000);
			if (GetTiebaOperate().HasSetTieba())
				std::thread(&CAppealRejection::AppealRejectionThread, this).detach();
		}
	}).detach();
}

void CAppealRejection::Uninit()
{
	// 关闭窗口
	if (m_appealRejectionDlg != NULL)
		m_appealRejectionDlg->DestroyWindow();

	// 保存配置
	if (GetTiebaOperate().HasSetTieba())
	{
		CString currentUserDir = GetCurrentUserDir();
		m_config.Save(currentUserDir + _T("config_AR.xml"));
	}
}

void CAppealRejection::OnConfig()
{
	if (!GetTiebaOperate().HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	if (m_appealRejectionDlg == NULL)
	{
		m_appealRejectionDlg = new CAppealRejectionDlg(m_appealRejectionDlg, m_config, m_version);
		m_appealRejectionDlg->Create(m_appealRejectionDlg->IDD, CWnd::GetDesktopWindow());
	}
}

void CAppealRejection::OnPostSetTieba(const CString& forumName)
{
	// 载入配置
	CString currentUserDir = GetCurrentUserDir();
	m_config.Load(currentUserDir + _T("config_AR.xml"));

	// 开始申诉驳回
	std::thread(&CAppealRejection::AppealRejectionThread, this).detach();
}

void CAppealRejection::AppealRejectionThread()
{
	// 检查是否启用
	if (!m_config.m_enable)
		return;

	ILog& log = GetLog();
	CString currentUserDir = GetCurrentUserDir();
	CTiebaOperate& tiebaOperate = GetTiebaOperate();
	CTBMOperate& operate = GetOperate();
	
	// 开始处理
	SYSTEMTIME time = {};
	GetLocalTime(&time);
	CString startDate;
	startDate.Format(_T("<font color=blue>开始申诉驳回（每8小时处理一次）:</font><font color=green>=====%d年%d月%d日-%d:%d:%d=====</font>"),
		(LPCTSTR)time.wYear, (LPCTSTR)time.wMonth, (LPCTSTR)time.wDay, (LPCTSTR)time.wHour, (LPCTSTR)time.wMinute, (LPCTSTR)time.wSecond);
	log.Log(startDate);

	int c_pn = 1; // 页码
	BOOL hasMore = FALSE; // 是否还有更多申诉
	do {
		startDate.Format(_T("<font color=blue>====第%d页====</font>"), c_pn);
		log.Log(startDate);

		// 获取申诉列表
		CString src;
		src = tiebaOperate.GetAppealList(c_pn);
		if (src == NET_TIMEOUT_TEXT)
		{
			log.Log(_T("<font color=red>访问超时！</font>"));
			return;
		}

		// 判断返回码是否成功 "no":0,"error":"success" 如果失败，返回error信息
		if (GetStringBetween(src, _T("\"no\":"), _T(",")) != _T("0"))
		{
			CString tmp;
			tmp.Format(_T("<font color=red>获取申诉列表失败！错误原因：%s</font>"), GetStringBetween(src, _T("\"error\":"), _T(",")));
			log.Log(tmp);
			return;
		}

		// 解析Json申诉列表
		GenericDocument<UTF16<>> document;
		document.Parse(src);
		if (document.HasParseError() || !document.IsObject() || !document.HasMember(L"data") || !document[L"data"].IsObject())
		{
			log.Log(_T("<font color=red>获取申诉列表失败！返回数据格式错误！</font>"));
			DebugRecord(_T("获取申诉列表失败"), src);
			return;
		}

		int appeal_num = document[L"data"][L"appeal_count_data"][L"appeal_num"].GetInt(); // 待处理申诉数量
		if (appeal_num == 0)
		{
			log.Log(_T("<font color=blue>没有待处理的申诉！</font>"));
			return;
		}

		const auto& appeal_list = document[L"data"][L"appeal_list"];
		// 判断appeal_list有多少需要处理
		int appeal_list_size = appeal_list.Size();
		CString rejList = _T("");
		int rejCount = 0;
		hasMore = document[L"data"][L"has_more"].GetBool(); // 是否还有更多申诉

		for (UINT i = 0; i < appeal_list_size; i++)
		{
			const auto& appeal = appeal_list[i];
			CString a_id = appeal[L"appeal_id"].GetString(); // 申诉ID
			time_t a_time = _ttoi64(appeal[L"appeal_time"].GetString()); // appeal_time 申述时间 String 时间戳 1749883308 
			CString a_reason = appeal[L"appeal_reason"].GetString(); // 申诉理由
			time_t a_punish_start = _ttoi64(appeal[L"punish_start_time"].GetString());
			time_t a_punish_end = _ttoi64(appeal[L"punish_end_time"].GetString());
			CString a_operator = appeal[L"operate_man"].GetString(); // 操作人
			int a_punish_day = appeal[L"punish_day_num"].GetInt(); // 封禁天数
			time_t current_time = GetTimestamp(); // 计算剩余封禁天数
			int a_punish_left_hour = (int)((a_punish_end - current_time) / 3600); // 剩余封禁小时数
			int a_punish_left_day = (int)((a_punish_end - current_time) / (24 * 3600)); // 剩余封禁天数
			time_t a_punish_due_time;
			switch (a_punish_day) {
			case 1: // 1天申诉不处理
			case 3: // 3天申诉24小时释放
			default: // 其他情况默认同3天
				a_punish_due_time = a_time + 24 * 3600;
				break;
			case 10: // 10天 48小时处理
			case 90: // 90天 按照 10天处理 // 但是超级会员是 24小时，统一默认24小时
				a_punish_due_time = a_time + 24 * 3600;
				break;
			}
			int a_op_left_hour = (int)((a_punish_due_time - current_time) / 3600); // 剩余申诉处理小时数
			CString a_punish_reason = appeal[L"punish_reason"].GetString(); // 封禁理由
			CString a_man = appeal[L"user"][L"name_show"].GetString();
			CString a_portrait = appeal[L"portrait"].GetString(); // 头像ID

			// 打印日志： 申诉 [a_man] 10天(剩5天) 封禁时间：2025-01-01 14:41 申诉时间：2025-01-01 14:41 申诉理由：a_reason 封禁理由：a_punish_reason 操作人： a_operator
			CString tmp;
			if (a_punish_left_hour > 24) {
				tmp.Format(_T("<font color=blue>申诉</font> [%s] <font color=red>%d</font>天(剩<font color=red>%d</font>天)封禁时间：<font color=red>%s</font> 申诉时间：<font color=orange>%s</font> 申诉理由：%s 封禁理由：%s 操作人：%s"),
					a_man, a_punish_day, a_punish_left_day,
					GetYYMMDD_HHMMSS_FromTimeT(a_punish_start), GetYYMMDD_HHMMSS_FromTimeT(a_time),
					a_reason, a_punish_reason, a_operator);
			}
			else {
				tmp.Format(_T("<font color=blue>申诉</font> [%s] <font color=red>%d</font>天(剩<font color=red>%d</font>小时)封禁时间：<font color=red>%s</font> 申诉时间：<font color=orange>%s</font> 申诉理由：%s 封禁理由：%s 操作人：%s"),
					a_man, a_punish_day, a_punish_left_hour,
					GetYYMMDD_HHMMSS_FromTimeT(a_punish_start), GetYYMMDD_HHMMSS_FromTimeT(a_time),
					a_reason, a_punish_reason, a_operator);
			}
			log.Log(tmp);

			if (a_punish_due_time > a_punish_end)
			{
				log.Log(_T("<font color=green>申诉处理剩余时间大于封禁时间，跳过！</font>"));
				continue;
			}
			if (a_op_left_hour >= 12)
			{
				log.Log(_T("<font color=green>申诉处理剩余时间大于12小时，跳过！</font>"));
				continue;
			}
			tmp.Format(_T("<font color=blue>加入拒绝列表...%s</font>"), (LPCTSTR)a_id);
			log.Log(tmp);
			tmp.Format(_T("&appeal_list[%d]=%s"), rejCount, (LPCTSTR)a_id);
			rejList += tmp; // 拼接申诉列表
			rejCount++;
		}
		if (rejCount > 0)
		{
			// 批量申诉驳回
			CString src = tiebaOperate.PostAppealReject(rejList);
			if (src == NET_TIMEOUT_TEXT)
			{
				log.Log(_T("<font color=red>访问超时！</font>"));
				return;
			}

			// 判断返回码是否成功 "no":0,"error":"success" 如果失败，返回error信息
			if (GetStringBetween(src, _T("\"no\":"), _T(",")) != _T("0"))
			{
				CString tmp;
				tmp.Format(_T("<font color=red>批量申诉驳回失败！错误原因：%s</font>"), GetStringBetween(src, _T("\"error\":"), _T(",")));
				DebugRecord(_T("批量申诉驳回失败"), src);
				log.Log(tmp);
				return;
			}
			else {
				document.Parse(src);
				if (document.HasParseError() || !document.IsObject() || !document.HasMember(L"data") || !document[L"data"].IsObject())
				{
					log.Log(_T("<font color=red>获取申诉列表失败！返回数据格式错误！</font>"));
					DebugRecord(_T("申述驳回返回值异常"), src);
					return;
				}
				int s_num = document[L"data"][L"success_num"].GetInt(); // 成功驳回数量
				startDate.Format(_T("<font color=green>批量申诉驳回[%d]，成功[%d]</font>"), rejCount, s_num);
				log.Log(startDate);
			}
			Sleep((DWORD)(1000));
		}
		c_pn++; // 页码加1
	} while (hasMore);

	//结束日志
	log.Log(_T("<font color=blue>申诉驳回结束</font>"));
	CoUninitialize();
}
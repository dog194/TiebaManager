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
#include "TBMCoreRules.h"
#include <mutex>
#include "TiebaOperate.h"


class TBM_CORE_API CTBMCoreConfig : public CConfigBase
{
public:
	std::mutex m_optionsLock; // 方案锁

	COption<int>		m_scanInterval;			// 扫描间隔
	COption<BOOL>		m_onlyScanTitle;		// 只扫描标题
	COption<int>		m_scanPageCount;		// 扫描最后页数
	COption<BOOL>		m_briefLog;				// 只输出删帖封号
	COption<int>		m_threadCount;			// 线程数
	COption<int>        m_clawerInterface;      // 贴吧采集API用的接口
	COption<BOOL>       m_nickNameInterface;    // 启用昵称版本接口

	COption<BOOL>		m_autoVerify;			// 启动后自动验证
	COption<BOOL>		m_autoScan;				// 验证后自动扫描

	COption<BOOL>		m_delete;				// 删帖
	COption<BOOL>		m_banID;				// 封ID
	COption<BOOL>		m_defriend;				// 拉黑
	COption<float>		m_deleteInterval;		// 删帖间隔
	COption<int>		m_banDuration;			// 封禁时长
	COption<CString>	m_banReason;			// 封号原因
	COption<int>		m_banTrigCount;			// 封禁违规次数
	COption<int>		m_defriendTrigCount;	// 拉黑违规次数
	COption<BOOL>		m_confirm;				// 操作前提示
	COption<BOOL>		m_windowPro;			// 详细版操作界面
	COption<BOOL>		m_playSound;			// 播放删帖音效
	COption<BOOL>		m_showName;				// 标题/托盘显示用户名
	COption<BOOL>		m_ruleDoubleClick;		// 确认窗口规则跳转改用双击
	COption<BOOL>		m_hiddenFunction;		// 隐藏的，未经充分测试的功能
	COption<BOOL>		m_banClientInterface;	// 封禁用客户端接口

	COption<BOOL>		m_blackListEnable;		// 黑名单功能总开关
	COption<BOOL>		m_blackListConfirm;		// 黑名单强制确认
	COption<BOOL>		m_blackListDelete;		// 黑名单删帖
	COption<BOOL>		m_blackListBan;			// 黑名单封禁
	COption<BOOL>		m_blackListRecheck;		// 确认窗口重新检测是否在黑名单

	COption<BOOL>		m_acedEnhancedLzl;		// 进阶-增强型楼中楼
	COption<BOOL>		m_acedBlackCheckBan;	// 进阶-定期校验黑名单列表

	COption<BOOL>		m_infoNoAceDel;			// 信息-不在警示联动删除

	COption<BOOL>		m_toolsDebug;			// 工具-Debug

	COption<std::vector<CIllegalRule> >      m_illegalRules;      // 违规规则
	COption<std::vector<CRule> >             m_trustedRules;      // 信任规则
	COption<std::set<CString> >              m_trustedThreads;    // 信任主题
	COption<std::vector<CUserInfo> >		 m_blackListRules;    // 黑名单规则

	CTBMCoreConfig(CStringA name);
	virtual void OnChange();
	virtual void PostChange();
};

class TBM_CORE_API CTempIgnoreRule {
public:
	CString m_tid;
	CString m_portrait;

	CTempIgnoreRule(CString pTid, CString pPortrait) {
		m_tid = pTid;
		m_portrait = pPortrait;
	}
};

class TBM_CORE_API CImgSingleInfoCache {
public:
	CString m_imgName;
	CString m_info;

	CImgSingleInfoCache(CString pImgName, CString pInfo) {
		m_imgName = pImgName;
		m_info = pInfo;
	}
};

class TBM_CORE_API CUserCache : public CConfigBase
{
public:
	COption<std::set<__int64> > m_initIgnoredTID;		// 不删的主题ID(手动忽略)，要写入文件
	COption<std::set<__int64> > m_initIgnoredPID;		// 不删的帖子ID(手动忽略)，要写入文件
	COption<std::set<__int64> > m_initIgnoredLZLID;		// 不删的楼中楼ID(手动忽略)，要写入文件
	std::set<__int64> m_ignoredTID;						// 不删的主题ID(已扫描且违规)
	std::set<__int64> m_ignoredPID;						// 不删的帖子ID(已扫描且违规)
	std::set<__int64> m_ignoredLZLID;					// 不删的楼中楼ID(已扫描且违规)
	std::set<__int64> m_deletedTID;						// 已删的主题ID
	COption<std::map<__int64, int> > m_reply;			// 主题的回复数，要写入文件
	COption<std::map<__int64, CString> > m_lastAuthor;	// 主题的最后回复作者，要写入文件
	COption<std::map<CString, int> > m_userTrigCount;	// 某用户违规次数，要写入文件
	COption<std::set<CString> > m_bannedUser;			// 已封的用户，要写入文件
	COption<std::set<CString> > m_defriendedUser;		// 已拉黑的用户，要写入文件

	std::vector<CTempIgnoreRule> m_tempIgnoreRule;		// 一次性临时确认列表忽略规则，不写入文件

	CUserCache();

	virtual void PostChange();
};

class TBM_CORE_API ILog
{
public:
	virtual ~ILog() = default;

	virtual void Log(const CString& content) = 0;
};

const int RULE_TYPE_ILLEGA_RULE = 0;
const int RULE_TYPE_ILLEGA_RULE_DEL_LZ = 32;
const int RULE_TYPE_ILLEGA_RULE_CANCEL_DEL_LZ = 33;
const int RULE_TYPE_BLACK_LIST = 1;
const int RULE_TYPE_DELETE_ONLY = 2;
const int RULE_TYPE_BAN_DIRECTLY = 3;
const int RULE_TYPE_IGNORE_TID = 4;
const int RULE_TYPE_IGNORE_POR = 5;
const int RULE_TYPE_CHECK_D2F = 16;


// 使用本模块前应先设置的全局变量

extern TBM_CORE_API CTBMCoreConfig* g_pTbmCoreConfig;
extern TBM_CORE_API CUserCache* g_pUserCache;
extern TBM_CORE_API ILog* g_pLog;
extern TBM_CORE_API CTiebaOperate* g_pTiebaOperate;

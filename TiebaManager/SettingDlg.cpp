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
#include "SettingDlg.h"
#include <TBMCoreEvents.h>

#include "TBMGlobal.h"
#include "TBMConfigPath.h"
#include <MiscHelper.h>

#include "TiebaManager.h"
#include <TBMScan.h>
#include <Update.h>

// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CModelessDlg)

// 构造函数
CSettingDlg::CSettingDlg(CSettingDlg*& pThis, CWnd* pParent /*=NULL*/) : CModelessDlg(CSettingDlg::IDD, (CModelessDlg**)&pThis, pParent),
	m_scanPage(new CScanPage()),
	m_operatePage(new COperatePage()),
	m_illegalRulesPage(new CIllegalRulesPage()),
	m_trustedRulesPage(new CRulesPage<CRule>()),
	m_trustedThreadPage(new CNormalListPage(_T("主题ID："))),
	m_blackListRulesPage(new CBlackListRulesPage()),
	m_optionsPage(new COptionsPage()),
	m_usersPage(new CUsersPage()),
	m_aboutPage(new CAboutPage()),
	m_toolsPage(new CToolsPage())
{
	// 初始化m_pages
	m_pages.push_back(m_scanPage.get());
	m_pages.push_back(m_operatePage.get());
	m_pages.push_back(m_illegalRulesPage.get());
	m_pages.push_back(m_trustedRulesPage.get());
	m_pages.push_back(m_trustedThreadPage.get());
	m_pages.push_back(m_blackListRulesPage.get());
	m_pages.push_back(m_optionsPage.get());
	m_pages.push_back(m_usersPage.get());
	m_pages.push_back(m_aboutPage.get());
	m_pages.push_back(m_toolsPage.get());
}

#pragma region MFC
CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyButton);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CModelessDlg)
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CSettingDlg::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CSettingDlg::ApplyChange)
	ON_WM_TIMER()
END_MESSAGE_MAP()
#pragma endregion

// CSettingDlg 消息处理程序

// 初始化
BOOL CSettingDlg::OnInitDialog()
{
	CModelessDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	// 初始化m_tree
	int i = 0;
	HTREEITEM item = NULL;
	m_tree.SetItemData(m_tree.InsertItem(_T("扫描")), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("操作")), i++);
	m_tree.SetItemData(item = m_tree.InsertItem(_T("规则")), i);
	m_tree.SetItemData(m_tree.InsertItem(_T("违规规则"), item), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("信任规则"), item), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("忽略主题"), item), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("用户黑名单"), item), i++);
	m_tree.Expand(item, TVE_EXPAND);
	m_tree.SetItemData(m_tree.InsertItem(_T("方案")), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("账号管理")), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("关于&更新")), i++);
	m_tree.SetItemData(m_tree.InsertItem(_T("没想好叫啥")), i++);

	// 初始化各页
#define CREATE_PAGE(page) page->Create(page->IDD, this)
	CREATE_PAGE(m_scanPage);
	CREATE_PAGE(m_operatePage);
	AfxSetResourceHandle(GetModuleHandle(_T("Helper.dll"))); // 资源ID冲突....
	CREATE_PAGE(m_illegalRulesPage);
	CREATE_PAGE(m_trustedRulesPage);
	m_trustedRulesPage->m_static.SetWindowText(_T("匹配的帖子不会违规"));
	CREATE_PAGE(m_trustedThreadPage);
	m_trustedThreadPage->m_static.SetWindowText(_T("添加的主题不会扫描，主题ID是网址中\"p/\"后面跟的数字"));
	CREATE_PAGE(m_blackListRulesPage);
	AfxSetResourceHandle(theApp.m_hInstance);
	CREATE_PAGE(m_optionsPage);
	CREATE_PAGE(m_usersPage);
	CREATE_PAGE(m_aboutPage);
	CREATE_PAGE(m_toolsPage);

	CRect treeRect, wndRect, pageRect;
	m_tree.GetWindowRect(&treeRect);
	ScreenToClient(&treeRect);
	GetClientRect(&wndRect);
	pageRect.SetRect(treeRect.right + treeRect.left, treeRect.top, wndRect.Width() - treeRect.left, treeRect.bottom);
	for (const auto& page : m_pages)
		page->SetWindowPos(NULL, pageRect.left, pageRect.top, pageRect.Width(), pageRect.Height(), SWP_HIDEWINDOW);

	m_resize.AddControl(&m_tree, RT_NULL, NULL, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tree);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tree);
	m_resize.AddControl(&m_applyButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tree);
	for (const auto& page : m_pages)
		m_resize.AddControl(page, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_tree.SelectItem(m_tree.GetFirstVisibleItem());

	// 显示配置
	ShowPlan(g_plan);
	m_clearScanCache = FALSE; // 在m_scanPage->m_scanPageCountEdit.SetWindowText后初始化

	m_optionsPage->m_currentOptionStatic.SetWindowText(_T("当前方案：") + g_userConfig.m_plan); // 当前方案
	// 方案
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_DIR_PATH + _T("*.xml"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage->m_list.AddString(fileFind.GetFileTitle());
	}

	m_usersPage->m_currentUserStatic.SetWindowText(_T("当前账号：") + g_globalConfig.m_currentUser); // 当前账号
	// 账号
	m_usersPage->m_list.AddString(_T("[NULL]"));
	flag = fileFind.FindFile(USERS_DIR_PATH + _T("*"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots() 
			&& (PathFileExists(fileFind.GetFilePath() + _T("\\ck.xml"))))
		{
			CString name = fileFind.GetFileName();
			if (name != _T("[NULL]"))
				m_usersPage->m_list.AddString(name);
		}
	}

	m_aboutPage->m_autoCheckUpdateCheck.SetCheck(g_globalConfig.m_autoUpdate); // 自动更新
	SetWindowText(_T("设置-") + UPDATE_CURRENT_VERSION);

	// 更新基础库页面信息
	m_toolsPage->InitFilesList();
	m_toolsPage->UpdateVersionLabel();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 取消
void CSettingDlg::OnCancel()
{
	DestroyWindow();
}

// 提示是否保存
void CSettingDlg::OnClose()
{
	int result = AfxMessageBox(_T("保存设置？"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;

	CModelessDlg::OnClose();
}

// 切换标签
void CSettingDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	int index = m_tree.GetItemData(pNMTreeView->itemNew.hItem);
	for (int i = 0; i < (int)m_pages.size(); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}

// 显示关于
void CSettingDlg::ShowAbout()
{
	for (const auto& page : m_pages)
		page->ShowWindow(page == m_aboutPage.get() ? SW_SHOW : SW_HIDE);
}

// 显示违规规则
void CSettingDlg::ShowIllegalRulesPage()
{
	for (const auto& page : m_pages)
		page->ShowWindow(page == m_illegalRulesPage.get() ? SW_SHOW : SW_HIDE);
}

// 显示黑名单规则
void CSettingDlg::ShowBlackListRulePage()
{
	for (const auto& page : m_pages)
		page->ShowWindow(page == m_blackListRulesPage.get() ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// 显示当前设置
void CSettingDlg::ShowPlan(const CPlan& plan)
{
	CString tmp;

	tmp.Format(_T("%d"), *plan.m_scanInterval);
	m_scanPage->m_scanIntervalEdit.SetWindowText(tmp);				    // 扫描间隔
	m_scanPage->m_onlyScanTitleCheck.SetCheck(plan.m_onlyScanTitle);	// 只扫描标题
	tmp.Format(_T("%d"), *plan.m_scanPageCount);
	m_scanPage->m_scanPageCountEdit.SetWindowText(tmp);				    // 扫描最后页数
	m_scanPage->m_briefLogCheck.SetCheck(plan.m_briefLog);			    // 只输出删帖封号
	tmp.Format(_T("%d"), *plan.m_threadCount);						    
	m_scanPage->m_threadCountEdit.SetWindowText(tmp);				    // 线程数
	m_scanPage->m_autoSaveLogCheck.SetCheck(plan.m_autoSaveLog);		// 自动保存日志
	m_scanPage->m_clawerClientInterfaceCheck.SetCheck(plan.m_clawerInterface == 0 ? FALSE : TRUE); // 扫描接口
	m_scanPage->m_nicknameClientInterfaceCheck.SetCheck(plan.m_nickNameInterface); // 启用昵称版本接口
	m_scanPage->OnEnClientInterfaceChange();
	m_scanPage->m_autoVerifyCheck.SetCheck(plan.m_autoVerify);			// 启动后自动确认贴吧
	m_scanPage->m_autoScanCheck.SetCheck(plan.m_autoScan);				// 确认后贴吧自动扫描

	m_operatePage->m_deleteCheck.SetCheck(plan.m_delete);			    // 删帖
	m_operatePage->m_banIDCheck.SetCheck(plan.m_banID);				    // 封ID
	m_operatePage->OnBnClickedCheck1();
	m_operatePage->m_defriendCheck.SetCheck(plan.m_defriend);		    // 拉黑
	m_operatePage->OnBnClickedCheck3();
	tmp.Format(_T("%g"), *plan.m_deleteInterval);
	m_operatePage->m_deleteIntervalEdit.SetWindowText(tmp);			    // 删帖间隔
	m_operatePage->m_banDurationCombo.SetCurSel(plan.m_banDuration == 1 ? 0 : (plan.m_banDuration == 3 ? 1 : 2)); // 封禁时长
	m_operatePage->m_banReasonEdit.SetWindowText(*plan.m_banReason);	// 封禁原因
	tmp.Format(_T("%d"), *plan.m_banTrigCount);
	m_operatePage->m_banTrigCountEdit.SetWindowText(tmp);			    // 封禁违规次数
	tmp.Format(_T("%d"), *plan.m_defriendTrigCount);
	m_operatePage->m_defriendTrigCountEdit.SetWindowText(tmp);		    // 拉黑违规次数   
	m_operatePage->m_confirmCheck.SetCheck(plan.m_confirm);			    // 操作前提示
	m_operatePage->m_ProWinCheck.SetCheck(plan.m_windowPro);			// Pro窗口
	m_operatePage->m_PlaySoundCheck.SetCheck(plan.m_playSound);			// 播放删帖音效
	m_operatePage->m_ShowNameCheck.SetCheck(plan.m_showName);			// 标题/托盘显示用户名
	m_operatePage->m_RuleDoubleClickCheck.SetCheck(plan.m_ruleDoubleClick);			// 确认窗口规则跳转改用双击
	m_operatePage->m_banClientInterfaceCheck.SetCheck(plan.m_banClientInterface);	// 封禁使用客户端接口
	m_operatePage->m_blackListEnableCheck.SetCheck(plan.m_blackListEnable);			// 黑名单功能启用
	m_operatePage->m_blackListConfirmCheck.SetCheck(plan.m_blackListConfirm);		// 黑名单强制确认
	m_operatePage->m_blackListDeleteCheck.SetCheck(plan.m_blackListDelete);			// 黑名单删除贴
	m_operatePage->m_blackListBanCheck.SetCheck(plan.m_blackListBan);				// 黑名单封禁作者
	m_operatePage->m_blackListRecheckCheck.SetCheck(plan.m_blackListRecheck);		// 黑名单对确认窗口生效
	m_operatePage->OnBnClickedCheckBlEnable();

	// 违规规则
	m_illegalRulesPage->ShowList(plan.m_illegalRules);

	// 信任规则
	m_trustedRulesPage->ShowList(plan.m_trustedRules);

	// 信任主题
	m_trustedThreadPage->ShowList(plan.m_trustedThreads);

	// 黑名单
	m_blackListRulesPage->ShowList(plan.m_blackListRules);
}

// 应用对话框中的设置
void CSettingDlg::ApplyPlanInDlg(CPlan& plan)
{
	CString strBuf;
	int intBuf;
	plan.OnChange();

	m_scanPage->m_scanIntervalEdit.GetWindowText(strBuf);
	*plan.m_scanInterval = _ttoi(strBuf);								// 扫描间隔
	*plan.m_onlyScanTitle = m_scanPage->m_onlyScanTitleCheck.GetCheck(); // 只扫描标题
	m_scanPage->m_scanPageCountEdit.GetWindowText(strBuf);
	*plan.m_scanPageCount = _ttoi(strBuf);								// 扫描最后页数
	*plan.m_briefLog = m_scanPage->m_briefLogCheck.GetCheck();			// 只输出删帖封号
	m_scanPage->m_threadCountEdit.GetWindowText(strBuf);
	*plan.m_threadCount = _ttoi(strBuf);								// 线程数
	*plan.m_autoSaveLog = m_scanPage->m_autoSaveLogCheck.GetCheck();	// 自动保存日志
	*plan.m_clawerInterface = m_scanPage->m_clawerClientInterfaceCheck.GetCheck() == FALSE ? 0 : 1; // 扫描接口
	*plan.m_nickNameInterface = m_scanPage->m_nicknameClientInterfaceCheck.GetCheck();				// 启用昵称版本接口
	*plan.m_autoVerify = m_scanPage->m_autoVerifyCheck.GetCheck();		// 启动后自动确认贴吧
	*plan.m_autoScan = m_scanPage->m_autoScanCheck.GetCheck();			// 确认后贴吧自动扫描

	*plan.m_delete = m_operatePage->m_deleteCheck.GetCheck();			// 删帖
	*plan.m_banID = m_operatePage->m_banIDCheck.GetCheck();				// 封ID
	*plan.m_defriend = m_operatePage->m_defriendCheck.GetCheck();		// 拉黑
	m_operatePage->m_deleteIntervalEdit.GetWindowText(strBuf);
	*plan.m_deleteInterval = (float)_ttof(strBuf);						// 删帖间隔
	intBuf = m_operatePage->m_banDurationCombo.GetCurSel();
	*plan.m_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);		// 封禁时长
	m_operatePage->m_banReasonEdit.GetWindowText(strBuf);
	*plan.m_banReason = strBuf;											// 封禁原因
	m_operatePage->m_banTrigCountEdit.GetWindowText(strBuf);
	*plan.m_banTrigCount = _ttoi(strBuf);								// 封禁违规次数
	m_operatePage->m_defriendTrigCountEdit.GetWindowText(strBuf);
	*plan.m_defriendTrigCount = _ttoi(strBuf);							// 拉黑违规次数
	*plan.m_confirm = m_operatePage->m_confirmCheck.GetCheck();			// 操作前提示
	*plan.m_windowPro = m_operatePage->m_ProWinCheck.GetCheck();		// Pro窗口
	*plan.m_playSound = m_operatePage->m_PlaySoundCheck.GetCheck();		// 播放删帖音效
	*plan.m_showName = m_operatePage->m_ShowNameCheck.GetCheck();		// 标题/托盘显示用户名
	*plan.m_ruleDoubleClick = m_operatePage->m_RuleDoubleClickCheck.GetCheck();			// 确认窗口规则跳转改用双击
	*plan.m_banClientInterface = m_operatePage->m_banClientInterfaceCheck.GetCheck();	// 封禁使用客户端接口
	*plan.m_blackListEnable = m_operatePage->m_blackListEnableCheck.GetCheck();			// 黑名单功能启用
	*plan.m_blackListConfirm = m_operatePage->m_blackListConfirmCheck.GetCheck();		// 黑名单强制确认
	*plan.m_blackListDelete = m_operatePage->m_blackListDeleteCheck.GetCheck();			// 黑名单删除贴
	*plan.m_blackListBan = m_operatePage->m_blackListBanCheck.GetCheck();				// 黑名单封禁作者
	*plan.m_blackListRecheck = m_operatePage->m_blackListRecheckCheck.GetCheck();		// 黑名单对确认窗口生效

	// 违规规则
	m_illegalRulesPage->ApplyList(plan.m_illegalRules);

	// 信任规则
	m_trustedRulesPage->ApplyList(plan.m_trustedRules);

	// 信任主题
	m_trustedThreadPage->ApplyList(plan.m_trustedThreads);

	// 黑名单
	m_blackListRulesPage->ApplyList(plan.m_blackListRules);

	plan.PostChange();

	if (&plan == &g_plan && m_clearScanCache)
	{
		if (!plan.m_briefLog)
			g_pLog->Log(_T("<font color=green>清除历史回复</font>"));
		g_userCache.m_reply->clear();
	}
}

// 显示文件中的设置
void CSettingDlg::ShowPlanInFile(const CString& path)
{
	CPlan tmp;
	tmp.Load(path);
	ShowPlan(tmp);
}

// 把对话框中的设置写入文件
void CSettingDlg::SavePlanInDlg(const CString& path)
{
	CPlan tmp;
	ApplyPlanInDlg(tmp);
	tmp.Save(path);
}

// 确认
void CSettingDlg::OnOK()
{
	*g_globalConfig.m_autoUpdate = m_aboutPage->m_autoCheckUpdateCheck.GetCheck();
	g_globalConfig.Save(GLOBAL_CONFIG_PATH);

	CString tmp;
	m_optionsPage->m_currentOptionStatic.GetWindowText(tmp);
	*g_userConfig.m_plan = tmp.Right(tmp.GetLength() - 5); // "当前方案："
	g_userConfig.Save(USER_CONFIG_PATH);

	CreateDir(OPTIONS_DIR_PATH);
	SavePlanInDlg(OPTIONS_DIR_PATH + g_userConfig.m_plan + _T(".xml"));
	ApplyPlanInDlg(g_plan);

	g_settingWinCloseEvent();
	DestroyWindow();
}

// 应用
void CSettingDlg::ApplyChange()
{
	m_applyButton.EnableWindow(FALSE);

	*g_globalConfig.m_autoUpdate = m_aboutPage->m_autoCheckUpdateCheck.GetCheck();
	g_globalConfig.Save(GLOBAL_CONFIG_PATH);

	CString tmp;
	m_optionsPage->m_currentOptionStatic.GetWindowText(tmp);
	*g_userConfig.m_plan = tmp.Right(tmp.GetLength() - 5); // "当前方案："
	g_userConfig.Save(USER_CONFIG_PATH);

	CreateDir(OPTIONS_DIR_PATH);
	SavePlanInDlg(OPTIONS_DIR_PATH + g_userConfig.m_plan + _T(".xml"));
	ApplyPlanInDlg(g_plan);

	g_settingWinCloseEvent();
	
	SetTimer(0, 2000, NULL);
}

void CSettingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		KillTimer(0);
		m_applyButton.EnableWindow(TRUE);
		m_okButton.SetFocus();
	}
	CDialog::OnTimer(nIDEvent);
}
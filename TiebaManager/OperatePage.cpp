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

// OperatePage.cpp : 实现文件
//

#include "stdafx.h"
#include "OperatePage.h"
#include "SettingDlg.h"


// COperatePage 对话框

IMPLEMENT_DYNAMIC(COperatePage, CNormalDlg)

COperatePage::COperatePage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(COperatePage::IDD, pParent)
{
}

#pragma region MFC
COperatePage::~COperatePage()
{
}

void COperatePage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK2, m_deleteCheck);
	DDX_Control(pDX, IDC_CHECK1, m_banIDCheck);
	DDX_Control(pDX, IDC_COMBO1, m_banDurationCombo);
	DDX_Control(pDX, IDC_EDIT3, m_banTrigCountEdit);
	DDX_Control(pDX, IDC_EDIT4, m_deleteIntervalEdit);
	DDX_Control(pDX, IDC_CHECK4, m_confirmCheck);
	DDX_Control(pDX, IDC_PRO_WINDOW, m_ProWinCheck);
	DDX_Control(pDX, IDC_PLAY_SOUND, m_PlaySoundCheck);
	DDX_Control(pDX, IDC_SHOW_NAME, m_ShowNameCheck);
	DDX_Control(pDX, IDC_RULE_DOUBLE_CLICK, m_RuleDoubleClickCheck);
	DDX_Control(pDX, IDC_EDIT7, m_banReasonEdit);
	DDX_Control(pDX, IDC_CHECK3, m_defriendCheck);
	DDX_Control(pDX, IDC_EDIT8, m_defriendTrigCountEdit);
	DDX_Control(pDX, IDC_CHECK5, m_banClientInterfaceCheck);
	DDX_Control(pDX, IDC_CHECK_BL_ENABLE,	m_blackListEnableCheck);
	DDX_Control(pDX, IDC_CHECK_BL_CONFIRM,	m_blackListConfirmCheck);
	DDX_Control(pDX, IDC_CHECK_BL_DELETE,	m_blackListDeleteCheck);
	DDX_Control(pDX, IDC_CHECK_BL_BAN,		m_blackListBanCheck);
	DDX_Control(pDX, IDC_CHECK_BL_RECHECK,	m_blackListRecheckCheck);
}


BEGIN_MESSAGE_MAP(COperatePage, CNormalDlg)
	ON_BN_CLICKED(IDC_CHECK1, &COperatePage::OnBnClickedCheck1)
	ON_EN_KILLFOCUS(IDC_EDIT3, &COperatePage::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, &COperatePage::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT8, &COperatePage::OnEnKillfocusEdit8)
	ON_BN_CLICKED(IDC_CHECK3, &COperatePage::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK_BL_ENABLE,	&COperatePage::OnBnClickedCheckBlEnable)
	ON_BN_CLICKED(IDC_CHECK_BL_CONFIRM, &COperatePage::OnBnClickedCheckBlForceEnable)
END_MESSAGE_MAP()
#pragma endregion

// COperatePage 消息处理程序

// 封ID
void COperatePage::OnBnClickedCheck1()
{
	BOOL enable = m_banIDCheck.GetCheck();
	m_banDurationCombo.EnableWindow(enable);
	m_banReasonEdit.EnableWindow(enable);
	m_banTrigCountEdit.EnableWindow(enable);
}

// 拉黑
void COperatePage::OnBnClickedCheck3()
{
	BOOL enable = m_defriendCheck.GetCheck();
	m_defriendTrigCountEdit.EnableWindow(enable);
}

// 封禁违规次数
void COperatePage::OnEnKillfocusEdit3()
{
	CString tmp;
	m_banTrigCountEdit.GetWindowText(tmp);
	int trigCount = _ttoi(tmp);
	if (trigCount < 1)
		m_banTrigCountEdit.SetWindowText(_T("1"));
}

// 拉黑违规次数
void COperatePage::OnEnKillfocusEdit8()
{
	CString tmp;
	m_defriendTrigCountEdit.GetWindowText(tmp);
	int trigCount = _ttoi(tmp);
	if (trigCount < 1)
		m_defriendTrigCountEdit.SetWindowText(_T("1"));
}

// 删帖间隔
void COperatePage::OnEnKillfocusEdit4()
{
	CString tmp;
	m_deleteIntervalEdit.GetWindowText(tmp);
	float deleteInterval = (float)_ttof(tmp);
	if (deleteInterval < 0 || deleteInterval > 60)
		m_deleteIntervalEdit.SetWindowText(_T("1.5"));
}

// 黑名单
void COperatePage::OnBnClickedCheckBlEnable()
{
	BOOL enable = m_blackListEnableCheck.GetCheck();
	m_blackListConfirmCheck.EnableWindow(enable);
	m_blackListDeleteCheck.EnableWindow(enable);
	m_blackListBanCheck.EnableWindow(enable);
	OnBnClickedCheckBlForceEnable();
}

// 黑名单强制确认，与对确认窗口生效互斥
void COperatePage::OnBnClickedCheckBlForceEnable()
{
	BOOL enableALL = m_blackListEnableCheck.GetCheck();
	BOOL enableCon = m_blackListConfirmCheck.GetCheck();
	m_blackListRecheckCheck.EnableWindow(!(!enableALL || enableCon));
	if (enableCon) {
		m_blackListRecheckCheck.SetCheck(!enableCon);
	}
}
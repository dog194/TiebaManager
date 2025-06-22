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

// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "AppealRejectionDlg.h"
#include "AppealRejection.h"
#include <TBMAPI.h>

// CLoopBanDlg 对话框

IMPLEMENT_DYNAMIC(CAppealRejectionDlg, CDialog)

CAppealRejectionDlg::CAppealRejectionDlg(CAppealRejectionDlg*& pThis, CAppealRejectionConfig& config, const CString pVersion,CWnd* pParent /*=NULL*/) :
	m_pThis(pThis),
	m_config(config),
	m_version(pVersion)
{
}

#pragma region MFC
CAppealRejectionDlg::~CAppealRejectionDlg()
{
}

void CAppealRejectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_AR_OPEN, m_enableCheck);
	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}

BEGIN_MESSAGE_MAP(CAppealRejectionDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanDlg 消息处理程序

// 初始化
BOOL CAppealRejectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 显示配置
	CString tmp;
	m_enableCheck.SetCheck(m_config.m_enable);				// 开启
	tmp.Format(_T("大吧，或者大吧授权给小吧。可以处理申诉。\r\n每8小时处理一次，距离自动释放低于12小时则驳回。\r\n1天封禁不处理\r\n3天申诉超24小时释放\r\n10天申诉超48小时释放\r\n90天百度没说，按照10天。"));
	m_static1.SetWindowText(tmp);					// 封禁间隔

	SetWindowText(_T("申诉驳回-") + m_version);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 取消
void CAppealRejectionDlg::OnCancel()
{
	DestroyWindow();
}

// 提示是否保存
void CAppealRejectionDlg::OnClose()
{
	int result = AfxMessageBox(_T("保存设置？"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;
}

// 释放this
void CAppealRejectionDlg::PostNcDestroy()
{
	m_pThis = NULL;
	delete this;
}
#pragma endregion

// 确认
void CAppealRejectionDlg::OnOK()
{
	CString currentUserDir = GetCurrentUserDir();

	*m_config.m_enable = m_enableCheck.GetCheck();				// 开启
	m_config.Save(currentUserDir + _T("config_AR.xml"));

	DestroyWindow();
}
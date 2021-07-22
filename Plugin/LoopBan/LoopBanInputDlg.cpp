/*
Copyright (C) 2011-2021  xfgryujk
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

// InputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LoopBanInputDlg.h"
#include "resource.h"


// CInputDlg 对话框
const UINT CLoopBanInputDlg::IDD = IDD_LOOP_BAN_USER_INFO;

IMPLEMENT_DYNAMIC(CLoopBanInputDlg, CDialog)

CLoopBanInputDlg::CLoopBanInputDlg(CString& uid, CString& portrait, CString& note, UINT nIDTemplate, CWnd* pParent)
	: CDialog(nIDTemplate, pParent), 
	m_uid(uid),
	m_portrait(portrait),
	m_note(note)
{
}

CLoopBanInputDlg::~CLoopBanInputDlg()
{
}

void CLoopBanInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UID, m_edit_uid);
	DDX_Control(pDX, IDC_EDIT_PORTRAIT, m_edit_portrait);
	DDX_Control(pDX, IDC_EDIT_NOTE, m_edit_note);
}

BEGIN_MESSAGE_MAP(CLoopBanInputDlg, CDialog)
END_MESSAGE_MAP()


// CInputDlg 消息处理程序

// 初始化
BOOL CLoopBanInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit_uid.SetWindowText(m_uid);
	m_edit_portrait.SetWindowText(m_portrait);
	m_edit_note.SetWindowText(m_note);
	m_edit_uid.SetSel(0, -1);
	m_edit_uid.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 确定
void CLoopBanInputDlg::OnOK()
{
	m_edit_uid.GetWindowText(m_uid);
	m_edit_portrait.GetWindowText(m_portrait);
	m_edit_note.GetWindowText(m_note);

	CDialog::OnOK();
}
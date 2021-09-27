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

CLoopBanInputDlg::CLoopBanInputDlg(CString& uid, CString& portrait, CString& note, UINT nIDTemplate, CWnd* pParent
	, const CString& pPreNote, const CString& pNextNote)
	: CDialog(nIDTemplate, pParent), 
	m_uid(uid),
	m_portrait(portrait),
	m_note(note),
	m_pre_note(pPreNote),
	m_next_note(pNextNote)
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
	DDX_Control(pDX, IDC_BUTTON_PRE, m_button_note_pre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_button_note_next);
}

BEGIN_MESSAGE_MAP(CLoopBanInputDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_PORTRAIT, &CLoopBanInputDlg::OnEnKillfocusEditPortrait)
	ON_BN_CLICKED(IDC_BUTTON_PRE, &CLoopBanInputDlg::OnClickNotePre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CLoopBanInputDlg::OnClickNoteNext)
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

	if (m_pre_note == _T("")) {
		m_button_note_pre.ShowWindow(SW_HIDE);
	}
	if (m_next_note == _T("")) {
		m_button_note_next.ShowWindow(SW_HIDE);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 确定
void CLoopBanInputDlg::OnOK()
{
	m_edit_uid.GetWindowText(m_uid);
	if (m_uid == _T("")) {
		m_edit_uid.ShowBalloonTip(_T(""), _T("用户名/昵称 不能为空，用于封禁或日志显示"), TTI_NONE);
		return;
	}
	m_edit_portrait.GetWindowText(m_portrait);
	m_edit_note.GetWindowText(m_note);

	CDialog::OnOK();
}

// 输入Url -> portrait
void CLoopBanInputDlg::OnEnKillfocusEditPortrait()
{
	CString tmp, tmpP, tmpU_ori, tmpU;
	m_edit_portrait.GetWindowText(tmp);
	if (tmp.GetLength() > 36) {
		tmpP = GetStringBetween(tmp, _T("id="), _T("&"));
		if (tmpP.GetLength() > 36) {
			tmpP = GetStringBefore(tmpP, _T("?"));
		}
		if (tmpP.GetLength() <= 36 && tmpP.GetLength() > 30) {
			m_edit_portrait.SetWindowTextW(tmpP);
		}
		m_edit_uid.GetWindowText(tmpU_ori);
		if (tmpU_ori == _T("")) {
			tmpU_ori = GetStringBetween(tmp, _T("un="), _T("&"));
			tmpU = DncodeURI(tmpU_ori);
			if (tmpU == _T("")) {
				tmpU = DncodeURI_GBK(tmpU_ori);
			}
			m_edit_uid.SetWindowTextW(DncodeURI(tmpU));
		}
	}
}

// 快捷填写 同上
void CLoopBanInputDlg::OnClickNotePre()
{
	m_edit_note.SetWindowText(m_pre_note);
}

// 快捷填写 同下
void CLoopBanInputDlg::OnClickNoteNext()
{
	m_edit_note.SetWindowText(m_next_note);
}
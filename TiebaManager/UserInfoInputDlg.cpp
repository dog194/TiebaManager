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
#include "UserInfoInputDlg.h"
//#include "resource.h"


// CInputDlg 对话框
const UINT CUserInfoInputDlg::IDD = IDD_USER_INFO;

IMPLEMENT_DYNAMIC(CUserInfoInputDlg, CDialog)

CUserInfoInputDlg::CUserInfoInputDlg(CUserInfo& userinfo, UINT nIDTemplate, CWnd* pParent, const CString& pPreNote, const CString& pNextNote, std::shared_ptr<CUserInfo> preFillInfo )
	: CDialog(nIDTemplate, pParent),
	m_userinfo(userinfo),
	m_pre_note(pPreNote),
	m_next_note(pNextNote),
	m_preFillUserInfo(std::move(preFillInfo))
{
}

CUserInfoInputDlg::CUserInfoInputDlg(CUserInfo& userinfo, UINT nIDTemplate, CWnd* pParent)
	: CDialog(nIDTemplate, pParent),
	m_userinfo(userinfo)
{
}

CUserInfoInputDlg::~CUserInfoInputDlg()
{
}

void CUserInfoInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UID, m_edit_uid);
	DDX_Control(pDX, IDC_EDIT_PORTRAIT, m_edit_portrait);
	DDX_Control(pDX, IDC_EDIT_NOTE, m_edit_note);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_button_note_pre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_button_note_next);
	DDX_Control(pDX, IDC_STATIC3, m_static_portrait);
}

BEGIN_MESSAGE_MAP(CUserInfoInputDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_PORTRAIT,	&CUserInfoInputDlg::OnEnKillfocusEditPortrait)
	ON_BN_CLICKED(	IDC_BUTTON_PRE,		&CUserInfoInputDlg::OnClickNotePre)
	ON_BN_CLICKED(	IDC_BUTTON_NEXT,	&CUserInfoInputDlg::OnClickNoteNext)
END_MESSAGE_MAP()


// CInputDlg 消息处理程序

// 初始化
BOOL CUserInfoInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_userinfo.m_uid == _T("") && m_userinfo.m_portrait == _T("") && m_userinfo.m_note == _T("")) {
		if (m_preFillUserInfo != NULL) {
			m_edit_uid.SetWindowText(m_preFillUserInfo->m_uid);
			m_edit_portrait.SetWindowText(m_preFillUserInfo->m_portrait);
			m_edit_note.SetWindowText(m_preFillUserInfo->m_note);
		}
	}
	else {
		m_edit_uid.SetWindowText(m_userinfo.m_uid);
		m_edit_portrait.SetWindowText(m_userinfo.m_portrait);
		m_edit_note.SetWindowText(m_userinfo.m_note);
	}
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
void CUserInfoInputDlg::OnOK()
{
	m_edit_uid.GetWindowText(m_userinfo.m_uid);
	if (m_userinfo.m_uid == _T("")) {
		m_edit_uid.ShowBalloonTip(_T(""), _T("用户名/昵称 不能为空，用于封禁或日志显示"), TTI_NONE);
		return;
	}
	m_edit_portrait.GetWindowText(m_userinfo.m_portrait);
	m_edit_note.GetWindowText(m_userinfo.m_note);
	m_preFillUserInfo = NULL;

	CDialog::OnOK();
}

// 输入Url -> portrait
void CUserInfoInputDlg::OnEnKillfocusEditPortrait()
{
	CString tmp, tmpP, tmpU_ori, tmpU;
	// url自动截取
	m_edit_portrait.GetWindowText(tmp);
	m_edit_uid.GetWindowText(tmpU_ori);
	if (tmp.GetLength() > PORT_LEN_MAX) {
		tmpP = GetStringBetween(tmp, _T("id="), _T("&"));
		if (tmpP.GetLength() > PORT_LEN_MAX) {
			tmpP = GetStringBefore(tmpP, _T("?"));
		}
		if (tmpP.GetLength() <= PORT_LEN_MAX && tmpP.GetLength() >= PORT_LEN_MIN) {
			m_edit_portrait.SetWindowTextW(tmpP);
		}
	}
	// 输入验证
	m_edit_portrait.GetWindowText(tmp);
	m_edit_uid.GetWindowText(tmpU_ori);
	if (tmp.GetLength() > PORT_LEN_MAX) {
		m_static_portrait.SetWindowTextW(_T("		 头像ID长度超过，如果复制正确，请到群里反馈"));
		m_edit_note.ShowBalloonTip(_T(""), _T("头像ID长度超过，如果复制正确，请到群里反馈"), TTI_NONE);
		return;
	}
	if (tmp.GetLength() < PORT_LEN_MIN) {
		m_static_portrait.SetWindowTextW(_T("		 头像ID长度不足，如果复制正确，请到群里反馈"));
		m_edit_note.ShowBalloonTip(_T(""), _T("头像ID长度不足，如果复制正确，请到群里反馈"), TTI_NONE);
		return;
	}
	if (tmp.GetLength() >= PORT_LEN_MIN && tmp.GetLength() <= PORT_LEN_MAX) {
		tmpU = GetNameUsingPortrait(tmp);
		if (tmpU == GET_NAME_ERROR_SHORT) {
			m_static_portrait.SetWindowTextW(_T("		  头像ID长度不足，如果复制正确，请到群里反馈"));
			m_edit_note.ShowBalloonTip(_T(""), _T("头像ID长度不足，如果复制正确，请到群里反馈"), TTI_NONE);
			return;
		}
		else if (tmpU == GET_NAME_ERROR_TIME_OUT) {
			m_static_portrait.SetWindowTextW(_T("		 头像ID验证超时，请检查网络，或重试"));
			m_edit_note.ShowBalloonTip(_T(""), _T("头像ID验证超时，请检查网络，或重试"), TTI_NONE);
			return;
		}
		else if (tmpU == GET_NAME_ERROR_INPUT_ERROR) {
			m_static_portrait.SetWindowTextW(_T("		 头像ID输入错误，无法查到对应用户"));
			m_edit_portrait.ShowBalloonTip(_T(""), _T("头像ID输入错误，无法查到对应用户"), TTI_ERROR);
			return;
		}
		else if (tmpU == GET_NAME_ERROR_FORMAT_ERROR) {
			m_static_portrait.SetWindowTextW(_T("		头像ID验证返回错误，请到群里反馈"));
			m_edit_uid.ShowBalloonTip(_T(""), _T("头像ID验证返回错误，请到群里反馈"), TTI_ERROR);
			return;
		}
		else {
			m_static_portrait.SetWindowTextW(_T("		头像ID验证成功：") + tmpU);
			if (tmpU_ori == _T("")) 
				m_edit_uid.SetWindowTextW(DncodeURI(tmpU));
		}
	}
}

// 快捷填写 同上
void CUserInfoInputDlg::OnClickNotePre()
{
	m_edit_note.SetWindowText(m_pre_note);
}

// 快捷填写 同下
void CUserInfoInputDlg::OnClickNoteNext()
{
	m_edit_note.SetWindowText(m_next_note);
}
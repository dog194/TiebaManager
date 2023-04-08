/*
Copyright (C) 2023  xfgryujk Dog194
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

// InputKeywordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputImgContentDlg.h"
#include <StringHelper.h>


// CInputImgContentDlg 对话框

IMPLEMENT_DYNAMIC(CInputImgContentDlg, CDialog)

CInputImgContentDlg::CInputImgContentDlg(CImgContentParam* param, CWnd* pParent /*=NULL*/) :
	CDialog(CInputImgContentDlg::IDD, pParent),
	m_param(param)
{
}

CInputImgContentDlg::~CInputImgContentDlg()
{
}

void CInputImgContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_rangeCombo);
	DDX_Control(pDX, IDC_CHECK1, m_notCheck);
	DDX_Control(pDX, IDC_COMBO2, m_includeCombo);
	DDX_Control(pDX, IDC_CHECK3, m_regexCheck);
	DDX_Control(pDX, IDC_EDIT1, m_contentEdit);
	DDX_Control(pDX, IDC_EDIT3, m_testEdit);
	DDX_Control(pDX, IDC_CHECK4, m_ignoreCaseCheck);
	DDX_Control(pDX, IDC_CHECK_IG_PORTI, m_igPortiCheck);
}


BEGIN_MESSAGE_MAP(CInputImgContentDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON7, &CInputImgContentDlg::OnBnClickedButton7)
END_MESSAGE_MAP()


// CInputKeywordDlg 消息处理程序

BOOL CInputImgContentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_rangeCombo.SetCurSel(m_param->m_contentType);
	m_notCheck.SetCheck(m_param->m_not);
	m_includeCombo.SetCurSel(m_param->m_include ? 0 : 1);
	m_regexCheck.SetCheck(m_param->m_keyword.isRegex);
	m_ignoreCaseCheck.SetCheck(m_param->m_keyword.ignoreCase);
	m_igPortiCheck.SetCheck(m_param->m_ignorePortrait);
	m_contentEdit.SetWindowText(m_param->m_keyword.text);

	m_testEdit.SetWindowText(_T("图片文件头可用于判断图片类型，如GIF： GIF, PNG：‰PN， JPG：ÿØÿ 二维码识别：无法识别返回空，至少一个结果[1]httpxxxx, 多个结果[1]xxx[2]xxx...匹配任意识别结果可以使用\"[1]\""));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CInputImgContentDlg::OnOK()
{
	m_param->m_contentType = CImgContentParam::ContentType(m_rangeCombo.GetCurSel());
	m_param->m_not = m_notCheck.GetCheck();
	m_param->m_include = m_includeCombo.GetCurSel() == 0;
	m_param->m_ignorePortrait = m_igPortiCheck.GetCheck();
	CString content;
	m_contentEdit.GetWindowText(content);
	m_param->m_keyword.Set(content, m_regexCheck.GetCheck(), m_ignoreCaseCheck.GetCheck());

	CDialog::OnOK();
}

// 测试
void CInputImgContentDlg::OnBnClickedButton7()
{
	CString content, test;
	m_contentEdit.GetWindowText(content);
	m_testEdit.GetWindowText(test);

	BOOL res;
	if (m_includeCombo.GetCurSel() == 0)
		res = StringIncludes(test, content, m_regexCheck.GetCheck(), m_ignoreCaseCheck.GetCheck());
	else
		res = StringMatchs(test, content, m_regexCheck.GetCheck(), m_ignoreCaseCheck.GetCheck());

	AfxMessageBox(res ? _T("匹配成功") : _T("匹配失败"));
}

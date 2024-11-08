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

// ExploreThreadPage.cpp : 实现文件
//

#include "stdafx.h"
#include "ExploreThreadPage.h"
#include "ExplorerDlg.h"
#include "ExplorePostPage.h"

#include <TiebaClawerProxy.h>
#include <TBMCoreImageHelper.h>
#include <StringHelper.h>


// CExploreThreadPage 对话框

IMPLEMENT_DYNAMIC(CExploreThreadPage, CExplorerPage)

CExploreThreadPage::CExploreThreadPage(const CString& forumName, CWnd* pParent /*=NULL*/) : CExplorerPage(pParent),
	m_forumName(forumName)
{
}

#pragma region MFC
CExploreThreadPage::~CExploreThreadPage()
{
}

void CExploreThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExploreThreadPage, CExplorerPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CExploreThreadPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_T_ID, &CExploreThreadPage::OnBnClickedButtonTID)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CExploreThreadPage::OnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CExploreThreadPage::OnDblclkList1)
END_MESSAGE_MAP()
#pragma endregion

// CExploreThreadPage 消息处理程序

// 初始化
BOOL CExploreThreadPage::OnInitDialog()
{
	CExplorerPage::OnInitDialog();

	m_edit.SetWindowText(_T("1"));

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int i = 0;
	m_list.InsertColumn(i++, _T(""), LVCFMT_LEFT, 0);
	m_list.InsertColumn(i++, _T("回复"), LVCFMT_RIGHT, 50);
	m_list.InsertColumn(i++, _T("标题"), LVCFMT_LEFT, 540);
	m_list.InsertColumn(i++, _T("作者"), LVCFMT_CENTER, 130);
	m_list.DeleteColumn(0); // 解决第一列文字不能右对齐的问题

	OnBnClickedButton1();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 转到
void CExploreThreadPage::OnBnClickedButton1()
{
	m_gotoButton.EnableWindow(FALSE);

	CString sPage;
	m_edit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	if (iPage < 1)
	{
		m_edit.SetWindowText(_T("1"));
		iPage = 1;
	}
	CString ignoreThread; // 忽略前几个主题
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	TiebaClawerProxy::GetInstance().GetThreads(m_forumName, ignoreThread, m_threads);
	m_list.DeleteAllItems();
	((CExplorerDlg*)GetParent()->GetParent())->m_edit.SetWindowText(_T(""));
	for (const TapiThreadInfo& i : m_threads)
	{
		int index = m_list.GetItemCount();
		m_list.InsertItem(index, i.reply);
		m_list.SetItemText(index, 1, i.title);
		m_list.SetItemText(index, 2, i.authorShowName);
	}

	m_gotoButton.EnableWindow(TRUE);
}

// 通过贴ID跳转
void CExploreThreadPage::OnBnClickedButtonTID()
{
	m_gotoButtonTID.EnableWindow(FALSE);

	CExplorerDlg* parentDlg = (CExplorerDlg*)GetParent()->GetParent();
	CExplorePostPage& explorePostPage = *parentDlg->m_explorePostPage;
	CString sTid;
	m_editTID.GetWindowText(sTid);
	explorePostPage.m_tid = sTid;
	explorePostPage.m_edit.SetWindowText(_T("1"));
	explorePostPage.m_editTID.SetWindowText(sTid);
	explorePostPage.OnBnClickedButton1();
	parentDlg->m_tab.SetCurSel(1);
	LRESULT tmp;
	parentDlg->OnTcnSelchangeTab1(NULL, &tmp);

	m_gotoButtonTID.EnableWindow(TRUE);
}

// 选中项改变
void CExploreThreadPage::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->iItem != LB_ERR && (pNMLV->uNewState & LVIS_SELECTED) != 0)
	{
		m_editTID.SetWindowText(m_threads[pNMLV->iItem].tid);
		CExplorerDlg* explorerDlg = (CExplorerDlg*)GetParent()->GetParent();
		explorerDlg->m_edit.SetWindowText(m_threads[pNMLV->iItem].title + _T("\r\n")
			+ m_threads[pNMLV->iItem].preview +
			+ _T("\r\n\r\n作者显示名：") + m_threads[pNMLV->iItem].authorShowName
			+ _T("\r\n等级：") + m_threads[pNMLV->iItem].authorLevel
			+ _T("\r\n\r\n作者名：") + m_threads[pNMLV->iItem].author
			+ _T("\r\n头像ID(Portrait)：") + GetPortraitFromString(m_threads[pNMLV->iItem].authorPortraitUrl)
			+ _T("\r\n虚拟形象心情：") + GetPortraitFromString(m_threads[pNMLV->iItem].customState)
			+ _T("\r\n\r\n时        间：") + GetYYMMDD_HHMMSS_FromTimeT(m_threads[pNMLV->iItem].timestamp)
			+ _T("\r\n主题帖ID：") + m_threads[pNMLV->iItem].tid);
		if (m_threads[pNMLV->iItem].isTidAuthor == TRUE)
			explorerDlg->m_isLZ.ShowWindow(SW_SHOW);
		else
			explorerDlg->m_isLZ.ShowWindow(SW_HIDE);
		auto img = std::make_unique<std::vector<CString> >();
		GetImageUrls(m_threads[pNMLV->iItem], *img);
		explorerDlg->ViewImages(std::move(img));
	}

	*pResult = 0;
}

// 双击列表
void CExploreThreadPage::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iItem != LB_ERR)
	{
		CExplorerDlg* parentDlg = (CExplorerDlg*)GetParent()->GetParent();
		CExplorePostPage& explorePostPage = *parentDlg->m_explorePostPage;
		m_editTID.SetWindowText(m_threads[pNMItemActivate->iItem].tid);
		explorePostPage.m_tid = m_threads[pNMItemActivate->iItem].tid;
		explorePostPage.m_editTID.SetWindowText(m_threads[pNMItemActivate->iItem].tid);
		explorePostPage.m_edit.SetWindowText(_T("1"));
		explorePostPage.OnBnClickedButton1();
		parentDlg->m_tab.SetCurSel(1);
		LRESULT tmp;
		parentDlg->OnTcnSelchangeTab1(NULL, &tmp);
	}

	*pResult = 0;
}

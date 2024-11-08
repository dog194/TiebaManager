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
#include "ExplorePostPage.h"
#include "ExploreLzlPage.h"
#include "ExplorerDlg.h"

#include <TiebaClawerProxy.h>
#include <TBMCoreImageHelper.h>
#include <StringHelper.h>


// CExplorePostPage 对话框

IMPLEMENT_DYNAMIC(CExplorePostPage, CExplorerPage)

CExplorePostPage::CExplorePostPage(const CString& forumID, CWnd* pParent /*=NULL*/) : CExplorerPage(pParent),
	m_forumID(forumID)
{
}

#pragma region MFC
CExplorePostPage::~CExplorePostPage()
{
}

void CExplorePostPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExplorePostPage, CExplorerPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CExplorePostPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_T_ID, &CExplorePostPage::OnBnClickedButtonTID)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CExplorePostPage::OnItemchangedList1)
END_MESSAGE_MAP()
#pragma endregion

// CExplorePostPage 消息处理程序

// 初始化
BOOL CExplorePostPage::OnInitDialog()
{
	CExplorerPage::OnInitDialog();

	m_edit.SetWindowText(_T("1"));

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int i = 0;
	m_list.InsertColumn(i++, _T(""), LVCFMT_LEFT, 0);
	m_list.InsertColumn(i++, _T("楼层"), LVCFMT_RIGHT, 50);
	m_list.InsertColumn(i++, _T("内容"), LVCFMT_LEFT, 540);
	m_list.InsertColumn(i++, _T("作者"), LVCFMT_CENTER, 130);
	m_list.DeleteColumn(0); // 解决第一列文字不能右对齐的问题

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 转到
void CExplorePostPage::OnBnClickedButton1()
{
	if (m_tid == _T(""))
		return;
	m_gotoButton.EnableWindow(FALSE);

	CString sPage;
	m_edit.GetWindowText(sPage);
	if (_ttoi(sPage) < 1)
	{
		m_edit.SetWindowText(_T("1"));
		sPage = _T("1");
	}


	CExplorerDlg* parentDlg = (CExplorerDlg*)GetParent()->GetParent();
	CExploreLzlPage& exploreLzlPage = *parentDlg->m_exploreLzlPage;
	TiebaClawerProxy::GetInstance().GetPosts(m_forumID, m_tid, sPage, m_posts, exploreLzlPage.m_lzls);
	m_list.DeleteAllItems();
	parentDlg->m_edit.SetWindowText(_T(""));
	for (const PostInfo& i : m_posts)
	{
		int index = m_list.GetItemCount();
		m_list.InsertItem(index, i.floor);
		m_list.SetItemText(index, 1, i.content);
		m_list.SetItemText(index, 2, i.authorShowName);
	}
	exploreLzlPage.m_list.DeleteAllItems();
	for (const LzlInfo& i : exploreLzlPage.m_lzls)
	{
		int index = exploreLzlPage.m_list.GetItemCount();
		exploreLzlPage.m_list.InsertItem(index, i.floor);
		exploreLzlPage.m_list.SetItemText(index, 1, i.content);
		exploreLzlPage.m_list.SetItemText(index, 2, i.authorShowName);
	}

	m_gotoButton.EnableWindow(TRUE);
}

// 通过贴ID跳转
void CExplorePostPage::OnBnClickedButtonTID()
{
	m_gotoButtonTID.EnableWindow(FALSE);

	CString sTid;
	m_editTID.GetWindowText(sTid);
	m_tid = sTid;
	OnBnClickedButton1();
	m_gotoButtonTID.EnableWindow(TRUE);
}

// 选中项改变
void CExplorePostPage::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->iItem != LB_ERR && (pNMLV->uNewState & LVIS_SELECTED) != 0)
	{
		CExplorerDlg* explorerDlg = (CExplorerDlg*)GetParent()->GetParent();
		explorerDlg->m_edit.SetWindowText(m_posts[pNMLV->iItem].content 
			+ _T("\r\n\r\n作者显示名：") + m_posts[pNMLV->iItem].authorShowName
			+ _T("\r\n等级：") + m_posts[pNMLV->iItem].authorLevel
			+ _T("\r\n楼层：") + m_posts[pNMLV->iItem].floor
			+ _T("\r\n\r\n作者名：") + m_posts[pNMLV->iItem].author
			+ _T("\r\n头像ID(Portrait)：") + GetPortraitFromString(m_posts[pNMLV->iItem].authorPortraitUrl)
			+ _T("\r\n虚拟形象心情：") + GetPortraitFromString(m_posts[pNMLV->iItem].customState)
			+ _T("\r\n\r\n时        间：") + GetYYMMDD_HHMMSS_FromTimeT(m_posts[pNMLV->iItem].timestamp)
			+ _T("\r\n主题帖ID：") + m_posts[pNMLV->iItem].tid);
		if (m_posts[pNMLV->iItem].isTidAuthor == TRUE)
			explorerDlg->m_isLZ.ShowWindow(SW_SHOW);
		else
			explorerDlg->m_isLZ.ShowWindow(SW_HIDE);
		auto img = std::make_unique<std::vector<CString> >();
		GetImageUrls(m_posts[pNMLV->iItem], *img);
		explorerDlg->ViewImages(std::move(img));
	}

	*pResult = 0;
}

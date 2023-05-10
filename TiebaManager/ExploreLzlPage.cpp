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
#include "ExploreLzlPage.h"
#include "ExplorerDlg.h"

#include <TBMCoreImageHelper.h>
#include <StringHelper.h>


// CExploreLzlPage 对话框

IMPLEMENT_DYNAMIC(CExploreLzlPage, CExplorerPage)

CExploreLzlPage::CExploreLzlPage(CWnd* pParent /*=NULL*/)
	: CExplorerPage(pParent)
{
}

#pragma region MFC
CExploreLzlPage::~CExploreLzlPage()
{
}

void CExploreLzlPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExploreLzlPage, CExplorerPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CExploreLzlPage::OnItemchangedList1)
END_MESSAGE_MAP()
#pragma endregion

// CExploreLzlPage 消息处理程序

// 初始化
BOOL CExploreLzlPage::OnInitDialog()
{
	CExplorerPage::OnInitDialog();

	m_static.ShowWindow(SW_HIDE);
	m_edit.ShowWindow(SW_HIDE);
	m_gotoButton.ShowWindow(SW_HIDE);
	m_staticTID.ShowWindow(SW_HIDE);
	m_editTID.ShowWindow(SW_HIDE);
	m_gotoButtonTID.ShowWindow(SW_HIDE);

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

// 选中项改变
void CExploreLzlPage::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->iItem != LB_ERR && (pNMLV->uNewState & LVIS_SELECTED) != 0)
	{
		CExplorerDlg* explorerDlg = (CExplorerDlg*)GetParent()->GetParent();
		explorerDlg->m_edit.SetWindowText(m_lzls[pNMLV->iItem].content 
			+ _T("\r\n\r\n作者显示名：") + m_lzls[pNMLV->iItem].authorShowName
			+ _T("\r\n等级：") + m_lzls[pNMLV->iItem].authorLevel
			+ _T("\r\n楼层：") + m_lzls[pNMLV->iItem].floor
			+ _T("\r\n\r\n作者名：") + m_lzls[pNMLV->iItem].author
			+ _T("\r\n头像ID(Portrait)：") + GetPortraitFromString(m_lzls[pNMLV->iItem].authorPortraitUrl)
			+ _T("\r\n\r\n时        间：") + GetYYMMDD_HHMMSS_FromTimeT(m_lzls[pNMLV->iItem].timestamp)
			+ _T("\r\n主题帖ID：") + m_lzls[pNMLV->iItem].tid);
		auto img = std::make_unique<std::vector<CString> >();
		GetImageUrls(m_lzls[pNMLV->iItem], *img);
		explorerDlg->ViewImages(std::move(img));
	}

	*pResult = 0;
}

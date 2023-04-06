/*
Copyright (C) 2023  Dog194
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

// AcedPage.cpp : 实现文件
//
#include "stdafx.h"
#include "AcedPage.h"
#include "SettingDlg.h"


// CAcedPage 对话框

IMPLEMENT_DYNAMIC(CAcedPage, CNormalDlg)

CAcedPage::CAcedPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CAcedPage::IDD, pParent)
{
}

#pragma region MFC
CAcedPage::~CAcedPage()
{
}

void CAcedPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENHANCED_LZL, m_acedEnhancedLzlCheck);
}


BEGIN_MESSAGE_MAP(CAcedPage, CNormalDlg)

END_MESSAGE_MAP()
#pragma endregion

// CAcedPage 消息处理程序
void CAcedPage::InitFilesList()
{
	CString tmp;
}

BOOL CAcedPage::OnInitDialog()
{
	CString tmp;
	return FALSE;
}
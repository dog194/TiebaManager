﻿/*
Copyright (C) 2022  Dog194
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

#pragma once
#include "afxwin.h"
#include <NormalDlg.h>
#include "resource.h"
#include "TBMConfig.h"


// CToolsPage 对话框

class CToolsPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CToolsPage)

public:
	CToolsPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolsPage();

	// 对话框数据
	enum { IDD = IDD_TOOLS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void InitFilesList();
	afx_msg void UpdateVersionLabel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ShowPlan(const CPlan& plan);
	void ApplyPlanInDlg(CPlan& plan);

public:
	CStatic m_staticHelpV;
	CStatic m_staticCurlV;
	CStatic m_staticCurlExeV;
	CStatic m_static7zV;
	CStatic m_static7zVnew;
	CStatic m_staticCurlVnew;
	CStatic m_staticCurlExeVnew;
	
	CString m_latestLibcurlV = _T("");
	
	CButton m_btnUpdate;
	CButton m_btnSaveCache;
	CButton m_checkDebug;

	afx_msg void OnBnClickedButtonLibcurlUpdate();
	afx_msg void OnBnClickedButtonOepnQq();
	afx_msg void OnBnClickedButtonSaveCache();
};

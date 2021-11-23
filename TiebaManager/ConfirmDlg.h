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

#pragma once
#include "afxwin.h"
#include "ResizeControl.h"
#include <TBMOperate.h>
#include "ImageViewDlg.h"


// CConfirmDlg 对话框

class CConfirmDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfirmDlg)

public:
	CConfirmDlg(CWnd* pParent = NULL);   // 标准构造函数
	CConfirmDlg(const Operation* operation, int &ruleType, CWnd* pParent = NULL);   // 指定内容的构造函数
	virtual ~CConfirmDlg();

// 对话框数据
	enum { IDD = IDD_CONFIRM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonAddBl();
	afx_msg void OpenRulePage();
	afx_msg void OnStnClickStaticRule();
	afx_msg void OnStnDblclickStaticRule();
	afx_msg void OnBnClickedButtonDeleteOnly();
	afx_msg void OnBnClickedButtonBanNow();

public:
	CEdit m_contentEdit;
	CStatic m_static;
	CStatic m_static_rule;
	CStatic m_static_is_bl;
	CStatic m_static_break_rule_count;
	CStatic m_static_con_quene_count;
	CButton m_explorerButton;
	CButton m_yesButton;
	CButton m_noButton;
	CButton m_addBlButton;
	CButton m_banDirectButton;
	CButton m_deleteOnlyButton;

	CImageViewDlg* m_imageViewDlg;

protected:
	CResizeControl m_resize;

	const Operation* m_operation;
	int& m_ruleType;
	int t = 1; // 没啥用，用于兼容原有接口。

	// 上次确认的时间
	static DWORD lastTime;
};

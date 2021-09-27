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

#pragma once
#include "HelperCommon.h"
#include <NormalDlg.h>

// CListPage 对话框

class HELPER_API CListPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CListPage)

public:
	CListPage(CWnd* pParent = NULL);   // 标准构造函数
	CListPage(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CListPage();

// 对话框数据
	const UINT IDD;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg virtual void OnClickedButton1();
	afx_msg virtual void OnClickedButton2();
	afx_msg virtual void OnClickedButton3();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg virtual void OnClickedButton4();
	afx_msg virtual void OnClickedButton5();
	afx_msg virtual void OnClickedButton6();

	virtual BOOL SetItem(int index) = 0;
	// 导入时index = -1，修改时也会调用，可以比较列表项数和内部数组尺寸判断是添加还是修改
	virtual void OnAdd(int index) { }
	// 清除时index = -1，取消添加时也会调用
	virtual void OnDelete(int index) { }
	// 设置选中
	virtual void SetSelectedRow(const int index);
	// 滑动到指定行
	virtual void ScrollToIndex(const int index);
	// 根据字符串第一列搜索，返回index
	virtual int FindIndexFromStr(const CString key);
	virtual BOOL Export(const CString& path);
	virtual BOOL Import(const CString& path);


public:
	CListCtrl m_list;
	CButton m_addButton;
	CButton m_deleteButton;
	CButton m_changeButton;
	CButton m_exportButton;
	CButton m_importButton;
	CButton m_clearButton;
	CStatic m_static;
	CString m_file_type = _T("XML文件 (*.xml)|*.xml|TXT文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||");
protected:
	COLORREF m_staticColor;
};

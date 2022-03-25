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
#include <NormalListPage.h>
class CLoopBanConfig;
#include "resource.h"

const UINT COLUMN_INDEX_UID = 0;
const UINT COLUMN_INDEX_PORTRAIT = 1;
const UINT COLUMN_INDEX_NOTE = 2;

//D: 新的数据结构
class CLBUserInfo
{
public:
	CString m_uid;			// userName
	CString m_pid;			// pid	//似乎用不到
	CString m_portrait;		// portrait
	CString m_note;		// 备注，没实际用途，单纯用来个人记录
	CLBUserInfo();
	CLBUserInfo(const CString& uid);
	CLBUserInfo(const CString& uid, const CString& pid, const CString& portrait);
	CLBUserInfo(const CString& uid, const CString& pid, const CString& portrait, const CString& note);
};

// CLoopBanDlg 对话框

class CLoopBanDlg : public CNormalListPage
{
	DECLARE_DYNAMIC(CLoopBanDlg)

public:
	CLoopBanDlg(CLoopBanDlg*& pThis, CLoopBanConfig& config, const CString pVersion, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoopBanDlg();

// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	virtual void OnCancel();
	afx_msg void OnClose();
	virtual void PostNcDestroy();
	virtual void OnOK();

	virtual BOOL SetItem(int index) override;
	virtual BOOL Export(const CString& path) override;
	virtual BOOL Import(const CString& path) override;
	virtual void OnAdd(int index) override;
	virtual void OnDelete(int index) override;
	void ShowList(const std::vector<CLBUserInfo>& list);
	void ApplyList(std::vector<CLBUserInfo>& list);


public:
	CButton m_enableCheck;
	CButton m_logCheck;
	CStatic m_static2;
	CEdit m_banIntervalEdit;
	CStatic m_static3;
	CButton m_autoLoopBanCheck;
	CButton m_okButton;
	CButton m_cancelButton;

	CLoopBanConfig& m_config;
	std::vector<CString> m_pid;

	BOOL m_clearCache = FALSE;

protected:
	CLoopBanDlg*& m_pThis;
	CString m_version = _T("");
};

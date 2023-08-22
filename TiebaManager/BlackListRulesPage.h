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

#pragma once
#include "afxwin.h"
#include <NormalListPage.h>
#include <TBMCoreRules.h>
#include "resource.h"

const UINT COLUMN_INDEX_UID = 0;
const UINT COLUMN_INDEX_PORTRAIT = 1;
const UINT COLUMN_INDEX_TRIG_COUNT = 2;
const UINT COLUMN_INDEX_NOTE = 3;
const UINT COLUMN_INDEX_BAN_STATUS = 4;

// CBlackListRulesPage 对话框

class CBlackListRulesPage : public CNormalListPage
{
	DECLARE_DYNAMIC(CBlackListRulesPage)

public:
	CBlackListRulesPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBlackListRulesPage() = default;

public:
	virtual BOOL OnInitDialog() override;

	virtual BOOL SetItem(int index) override;
	virtual void OnAdd(int index) override;
	virtual void OnDelete(int index) override;
	virtual BOOL Export(const CString& path) override;
	virtual BOOL Import(const CString& path) override;
	void setRuleD2Y(const CString& u_portrait, const CString& d2f);
	void ShowList(const std::vector<CUserInfo>& list);
	void ApplyList(std::vector<CUserInfo>& list);

	void SetPreFillInfo(const CString& authorShowName, const CString& portrait);
	
protected:
	class CRuleListFile : public CConfigBase
	{
	public:
		COption<std::vector<CUserInfo> > m_list;

		CRuleListFile() : CConfigBase("BlackList"),
			m_list("BlackList")
		{
			m_options.push_back(&m_list);
		}
	};

	std::vector<CUserInfo> m_rules;
	std::shared_ptr<CUserInfo> preFillInfo;
};
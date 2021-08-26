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

// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "BlackListRulesPage.h"
#include "SettingDlg.h"
#include "UserInfoInputDlg.h"
#include "StringHelper.h"

IMPLEMENT_DYNAMIC(CBlackListRulesPage, CNormalListPage)

CBlackListRulesPage::CBlackListRulesPage(CWnd* pParent /*=NULL*/) :
	CNormalListPage(_T("黑名单："))
{
	m_file_type = _T("XML文件 (*.xml)|*.xml|所有文件 (*.*)|*.*||");
}

// 初始化
BOOL CBlackListRulesPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	//D: override OnInitDialog, 因为要多加Colunm。且不知道父类有没有其他东西调用 = =||
	m_list.DeleteColumn(0);
	m_list.ModifyStyle(LVS_NOCOLUMNHEADER, 0);
	m_list.InsertColumn(COLUMN_INDEX_UID, _T("用户名/昵称"), LVCFMT_LEFT, 150);
	m_list.InsertColumn(COLUMN_INDEX_PORTRAIT, _T("portrait"), LVCFMT_LEFT, 260);
	m_list.InsertColumn(COLUMN_INDEX_TRIG_COUNT, _T("触发次数"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(COLUMN_INDEX_NOTE, _T("备注"), LVCFMT_LEFT, 500);
	m_static.SetWindowText(_T("黑名单用户，根据设置直接删帖或封禁。优先级低于信任规则，高于违规规则。"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 使用 UserInfoInput 输入窗口
BOOL CBlackListRulesPage::SetItem(int index)
{
	CUserInfoInputDlg dlg(m_rules[index], CUserInfoInputDlg::IDD, this);
	if (dlg.DoModal() == IDOK && m_rules[index].m_uid != _T(""))
	{
		// 判断是否已经在列表中，避免重复添加
		for (int i = 0; i < m_list.GetItemCount(); i++) {
			if (i == index)
				continue;
			if (m_rules[index].m_portrait != _T("")) {
				// portrait 不为空 ，默认用portrait
				if (m_rules[index].m_portrait == m_list.GetItemText(i, COLUMN_INDEX_PORTRAIT)) {
					AfxMessageBox(m_rules[index].m_portrait + _T(" 已经在列表中！"), MB_ICONINFORMATION);
					SetSelectedRow(i);
					ScrollToIndex(i);
					return FALSE;
				}
			}
			else {
				if (m_rules[index].m_uid == m_list.GetItemText(i, COLUMN_INDEX_UID)) {
					AfxMessageBox(m_rules[index].m_uid + _T(" 已经在列表中！"), MB_ICONINFORMATION);
					SetSelectedRow(i);
					ScrollToIndex(i);
					return FALSE;
				}
			}
		}
		// 更新列表
		m_list.SetItemText(index, COLUMN_INDEX_UID, m_rules[index].m_uid);
		m_list.SetItemText(index, COLUMN_INDEX_PORTRAIT, m_rules[index].m_portrait);
		m_list.SetItemText(index, COLUMN_INDEX_TRIG_COUNT, Int2CString(m_rules[index].m_trigCount));
		m_list.SetItemText(index, COLUMN_INDEX_NOTE, m_rules[index].m_note);

		((CSettingDlg*)GetParent())->m_clearScanCache = TRUE;
		return TRUE;
	}
	return FALSE;
}

void CBlackListRulesPage::OnAdd(int index)
{
	if (index >= 0)
	{
		if (m_rules.size() != m_list.GetItemCount()) // 添加
			m_rules.insert(m_rules.begin() + index, CUserInfo());
	}
}

void CBlackListRulesPage::OnDelete(int index)
{
	if (index >= 0)
		m_rules.erase(m_rules.begin() + index);
	else
		m_rules.clear();
}

// 导出xml
BOOL CBlackListRulesPage::Export(const CString& path)
{
	CRuleListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

// 导入xml
BOOL CBlackListRulesPage::Import(const CString& path)
{
	CRuleListFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(std::move(tmp.m_list));
	return TRUE;
}

void CBlackListRulesPage::ShowList(const std::vector<CUserInfo>& list)
{
	m_rules = list;

	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++) {
		m_list.InsertItem(i, list[i].m_uid);
		m_list.SetItemText(i, COLUMN_INDEX_PORTRAIT, list[i].m_portrait);
		m_list.SetItemText(i, COLUMN_INDEX_TRIG_COUNT, Int2CString(list[i].m_trigCount));
		m_list.SetItemText(i, COLUMN_INDEX_NOTE, list[i].m_note);
	}
}

void CBlackListRulesPage::ApplyList(std::vector<CUserInfo>& list)
{
	list = m_rules;
}
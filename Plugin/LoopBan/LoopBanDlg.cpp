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

// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "LoopBanDlg.h"
#include "LoopBan.h"
#include "LoopBanInputDlg.h"
#include <TBMAPI.h>

// 用户信息
CLBUserInfo::CLBUserInfo()
{
	m_uid = _T("");
	m_pid = _T("");
	m_portrait = _T("");
	m_note = _T("");
}

CLBUserInfo::CLBUserInfo(const CString& uid)
{
	m_uid = uid;
	m_pid = _T("");
	m_portrait = _T("");
	m_note = _T("");
}

CLBUserInfo::CLBUserInfo(const CString& uid, const CString& pid, const CString& portrait)
{
	m_uid = uid;
	m_pid = pid;
	m_portrait = portrait;
	m_note = _T("");
}

CLBUserInfo::CLBUserInfo(const CString& uid, const CString& pid, const CString& portrait, const CString& note)
{
	m_uid = uid;
	m_pid = pid;
	m_portrait = portrait;
	m_note = note;
}

DECLEAR_READ(CLBUserInfo)
{
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();	//虽然不知道做了什么，但是还是写了 = =
		return;
	}
	COption<CString> uid("uid");
	COption<CString> pid("pid");
	COption<CString> portrait("portrait");
	COption<CString> note("note");
	uid.Read(*optionNode);
	pid.Read(*optionNode);
	portrait.Read(*optionNode);
	note.Read(*optionNode);

	m_value.m_uid = uid;
	m_value.m_pid = pid;
	m_value.m_portrait = portrait;
	m_value.m_note = note;

	if (!IsValid(m_value))	//虽然不知道做了什么，但是还是写了 = =
		UseDefault();
}

DECLEAR_WRITE(CLBUserInfo)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	tinyxml2::XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> uid("uid");
	COption<CString> pid("pid");
	COption<CString> portrait("portrait");
	COption<CString> note("note");
	uid.Read(*optionNode);
	pid.Read(*optionNode);
	portrait.Read(*optionNode);
	note.Read(*optionNode);

	*uid = m_value.m_uid;
	*pid = m_value.m_pid;
	*portrait = m_value.m_portrait;
	*note = m_value.m_note;
	uid.Write(*optionNode);
	pid.Write(*optionNode);
	portrait.Write(*optionNode);
	note.Write(*optionNode);
}

// CLoopBanDlg 对话框

IMPLEMENT_DYNAMIC(CLoopBanDlg, CNormalListPage)

CLoopBanDlg::CLoopBanDlg(CLoopBanDlg*& pThis, CLoopBanConfig& config, CWnd* pParent /*=NULL*/) :
	CNormalListPage(_T("用户名："), IDD_LOOP_BAN_DLG, pParent),
	m_pThis(pThis),
	m_config(config)
{
}

#pragma region MFC
CLoopBanDlg::~CLoopBanDlg()
{
}

void CLoopBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK6, m_logCheck);
	DDX_Control(pDX, IDC_CHECK7, m_enableCheck);
	DDX_Control(pDX, IDC_EDIT9, m_banIntervalEdit);
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_STATIC3, m_static3);
	DDX_Control(pDX, IDC_CHECK8, m_autoLoopBanCheck);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CLoopBanDlg, CNormalListPage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanDlg 消息处理程序

// 初始化
BOOL CLoopBanDlg::OnInitDialog()
{
	CListPage::OnInitDialog();

	//D: override OnInitDialog, 因为要多加Colunm。且不知道父类有没有其他东西调用 = =||
	//m_list.ModifyStyle(0, LVS_NOCOLUMNHEADER);
	m_list.ModifyStyle(LVS_NOCOLUMNHEADER, 0);
	m_list.InsertColumn(COLUMN_INDEX_UID, _T("用户名/昵称"), LVCFMT_LEFT, 500);
	m_list.InsertColumn(COLUMN_INDEX_PORTRAIT, _T("portrait"), LVCFMT_LEFT, 500);
	m_list.InsertColumn(COLUMN_INDEX_NOTE, _T("备注"), LVCFMT_LEFT, 500);
	m_list.SetColumnWidth(COLUMN_INDEX_UID, 180);
	m_list.SetColumnWidth(COLUMN_INDEX_PORTRAIT, 320);

	m_resize.AddControl(&m_enableCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_logCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static2, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_banIntervalEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static3, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_autoLoopBanCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);

	// 显示配置
	CString tmp;
	ShowList(m_config.m_banlist);							// 用户名
	m_logCheck.SetCheck(m_config.m_log);					// 输出日志
	m_enableCheck.SetCheck(m_config.m_enable);				// 开启
	tmp.Format(_T("%g"), *m_config.m_banInterval);
	m_banIntervalEdit.SetWindowText(tmp);					// 封禁间隔
	m_autoLoopBanCheck.SetCheck(m_config.m_autoLoopBan);	// 自动循环封

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 取消
void CLoopBanDlg::OnCancel()
{
	DestroyWindow();
}

// 提示是否保存
void CLoopBanDlg::OnClose()
{
	int result = AfxMessageBox(_T("保存设置？"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;

	CNormalListPage::OnClose();
}

// 释放this
void CLoopBanDlg::PostNcDestroy()
{
	CNormalListPage::PostNcDestroy();

	m_pThis = NULL;
	delete this;
}
#pragma endregion

// overwrite，使用新的子输入窗口
BOOL CLoopBanDlg::SetItem(int index)
{
	CString uid = m_list.GetItemText(index, COLUMN_INDEX_UID);
	CString portrait = m_list.GetItemText(index, COLUMN_INDEX_PORTRAIT);
	CString note = m_list.GetItemText(index, COLUMN_INDEX_NOTE);

	CString pre_note = index > 0 ? m_list.GetItemText(index - 1, COLUMN_INDEX_NOTE) : _T("");
	CString next_note = index < m_list.GetItemCount() - 1 ? m_list.GetItemText(index + 1, COLUMN_INDEX_NOTE) : _T("");
	CLoopBanInputDlg dlg(uid, portrait, note, CLoopBanInputDlg::IDD, this, pre_note, next_note);
	if (dlg.DoModal() == IDOK && uid != _T(""))
	{
		// 判断是否已经在列表中，避免重复添加
		for (int i = 0; i < m_list.GetItemCount(); i++) {
			if (i == index)
				continue;
			if (portrait != _T("")) {
				// portrait 不为空 ，默认用portrait
				if (portrait == m_list.GetItemText(i, COLUMN_INDEX_PORTRAIT)) {
					if (AfxMessageBox(portrait + _T(" 已经在列表中！"), MB_ICONINFORMATION) == IDOK) {
						SetSelectedRow(i);
						ScrollToIndex(i);
						this->SetActiveWindow();
					}
					return FALSE;
				}
			}
			else {
				if (uid == m_list.GetItemText(i, COLUMN_INDEX_UID)) {
					if (AfxMessageBox(uid + _T(" 已经在列表中！"), MB_ICONINFORMATION) == IDOK) {
						SetSelectedRow(i);
						ScrollToIndex(i);
						this->SetActiveWindow();
					}
					return FALSE;
				}
			}
		}
		// 更新列表
		m_list.SetItemText(index, COLUMN_INDEX_UID, uid);
		m_list.SetItemText(index, COLUMN_INDEX_PORTRAIT, portrait);
		m_list.SetItemText(index, COLUMN_INDEX_NOTE, note);
		return TRUE;
	}
	return FALSE;
}

class CBanListFile : public CConfigBase
{
public:
	COption<std::vector<CLBUserInfo> > m_list;

	CBanListFile() : CConfigBase("Banlist"),
		m_list("Banlist")
	{
		m_options.push_back(&m_list);
	}
};

// 导出xml
BOOL CLoopBanDlg::Export(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Export(path);

	CBanListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

// 导入xml
BOOL CLoopBanDlg::Import(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Import(path);

	CBanListFile tmp;
	if (!tmp.Load(path)) //D: 需要同时考虑旧版和新版。
		return CNormalListPage::Import(path);
	ShowList(tmp.m_list);
	return TRUE;
}

// 添加
void CLoopBanDlg::OnAdd(int index)
{
	m_clearCache = TRUE;
}

// 删除
void CLoopBanDlg::OnDelete(int index)
{
}

// 确认
void CLoopBanDlg::OnOK()
{
	CString currentUserDir = GetCurrentUserDir();

	CString strBuf;
	// 循环封
	ApplyList(m_config.m_banlist);								// 将List应用回配置
	*m_config.m_log = m_logCheck.GetCheck();					// 输出日志
	*m_config.m_enable = m_enableCheck.GetCheck();				// 开启
	m_banIntervalEdit.GetWindowText(strBuf);
	*m_config.m_banInterval = (float)_ttof(strBuf);				// 封禁间隔
	*m_config.m_autoLoopBan = m_autoLoopBanCheck.GetCheck();	// 自动循环封

	m_config.Save(currentUserDir + _T("options2.xml"));

	if (m_clearCache)
		DeleteFile(currentUserDir + _T("LoopBanDate.xml"));

	DestroyWindow();
}

void CLoopBanDlg::ShowList(const std::vector<CLBUserInfo>& list)
{
	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++) {
		m_list.InsertItem(i, list[i].m_uid);
		m_list.SetItemText(i, COLUMN_INDEX_PORTRAIT, list[i].m_portrait);
		m_list.SetItemText(i, COLUMN_INDEX_NOTE, list[i].m_note);
	}
}

void CLoopBanDlg::ApplyList(std::vector<CLBUserInfo>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++) {
		list[i].m_uid = m_list.GetItemText(i, COLUMN_INDEX_UID);
		list[i].m_portrait = m_list.GetItemText(i, COLUMN_INDEX_PORTRAIT);
		list[i].m_note = m_list.GetItemText(i, COLUMN_INDEX_NOTE);
	}
}
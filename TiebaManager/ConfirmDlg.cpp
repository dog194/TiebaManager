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

// ConfirmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfirmDlg.h"
#include "ImageViewDlg.h"
#include <TBMCoreImageHelper.h>
#include <StringHelper.h>
#include "TBMGlobal.h"
#include "TiebaManagerDlg.h"
#include "TiebaManager.h"


// CConfirmDlg 对话框

DWORD CConfirmDlg::lastTime = 0;


IMPLEMENT_DYNAMIC(CConfirmDlg, CDialog)

CConfirmDlg::CConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmDlg::IDD, pParent),
	m_resize(this),
	m_ruleType(t)
{
	m_operation = NULL;
}

CConfirmDlg::CConfirmDlg(const Operation* operation, int& ruleType, CWnd* pParent)
	: CDialog(CConfirmDlg::IDD, pParent),
	m_resize(this),
	m_ruleType(ruleType)
{
	m_operation = operation;
}

#pragma region MFC
CConfirmDlg::~CConfirmDlg()
{
}

void CConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_contentEdit);
	DDX_Control(pDX, IDC_STATIC1, m_static);
	DDX_Control(pDX, IDC_STATIC_RULE, m_static_rule);
	DDX_Control(pDX, IDC_STATIC_CON_IS_BL, m_static_is_bl);
	DDX_Control(pDX, IDC_STATIC_CON_RULE_COUNT, m_static_break_rule_count);
	DDX_Control(pDX, IDC_STATIC_CON_OPE_COUNT, m_static_con_quene_count);
	DDX_Control(pDX, IDOK, m_yesButton);
	DDX_Control(pDX, IDCANCEL, m_noButton);
	DDX_Control(pDX, IDC_BUTTON1, m_explorerButton);
	DDX_Control(pDX, IDC_BUTTON_ADD_BL, m_addBlButton);
	DDX_Control(pDX, IDC_BUTTON_BAN_NOW, m_banDirectButton);
	DDX_Control(pDX, IDC_BUTTON_DELETE_ONLY, m_deleteOnlyButton);
}


BEGIN_MESSAGE_MAP(CConfirmDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CConfirmDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_ADD_BL, &CConfirmDlg::OnBnClickedButtonAddBl)
	ON_WM_TIMER()
	ON_STN_DBLCLK(IDC_STATIC_RULE, &CConfirmDlg::OnStnDblclickStaticRule)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ONLY, &CConfirmDlg::OnBnClickedButtonDeleteOnly)
	ON_BN_CLICKED(IDC_BUTTON_BAN_NOW, &CConfirmDlg::OnBnClickedButtonBanNow)
END_MESSAGE_MAP()
#pragma endregion

// CConfirmDlg 消息处理程序

#pragma region UI
// 改变尺寸
void CConfirmDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_resize.Resize();
}
// 控件颜色
HBRUSH CConfirmDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static_is_bl.m_hWnd)
		pDC->SetTextColor(RGB(255, 0, 0));

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
#pragma endregion

// 初始化
BOOL CConfirmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_resize.AddControl(&m_contentEdit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	m_resize.AddControl(&m_addBlButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_static);
	m_resize.AddControl(&m_banDirectButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_static);
	m_resize.AddControl(&m_deleteOnlyButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_static);
	m_resize.AddControl(&m_explorerButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	m_resize.AddControl(&m_yesButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	m_resize.AddControl(&m_noButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	//m_resize.AddControl(&m_static_rule, RT_NULL, NULL, RT_NULL, NULL);
	m_resize.AddControl(&m_static_is_bl, RT_KEEP_DIST_TO_RIGHT, this, RT_NULL, NULL);
	m_resize.AddControl(&m_static_break_rule_count, RT_KEEP_DIST_TO_RIGHT, this, RT_NULL, NULL);
	m_resize.AddControl(&m_static_con_quene_count, RT_KEEP_DIST_TO_RIGHT, this, RT_NULL, NULL);

	if (m_operation != NULL)
	{
		SetWindowText(m_operation->title);
		CString content = m_operation->object->GetContent() + _T("\r\n\r\n作者显示名：") + m_operation->object->authorShowName;
		switch (m_operation->object->m_type)
		{
		case TBObject::POST:
		{
			const PostInfo& post = (PostInfo&)*m_operation->object;
			content += _T("\r\n等级：") + post.authorLevel
					 + _T("\r\n楼层：") + post.floor;
			break;
		}
		case TBObject::LZL:
		{
			const LzlInfo& lzl = (LzlInfo&)*m_operation->object;
			content += _T("\r\n回复楼层：") + lzl.floor;
			break;
		}
		}
		content += _T("\r\n\r\n作者名：") + m_operation->object->author		//多加个换行，有昵称的时候，显示名是昵称，没昵称的时候，显示名等于用户名
			+ _T("\r\nPortrait：") + GetPortraitFromString(m_operation->object->authorPortraitUrl);
		if (m_operation->object->timestamp != 0) {
			//不是所有数据都带时间
			content += _T("\r\n\r\n时     间：") + GetYYMMDD_HHMMSS_FromTimeT(m_operation->object->timestamp);
		}

		m_contentEdit.SetWindowText(content);
		m_contentEdit.SetSel(m_operation->pos, m_operation->pos + m_operation->length);

		auto img = std::make_unique<std::vector<CString> >();
		GetImageUrls(*m_operation->object, *img);
		if (!img->empty())
		{
			m_imageViewDlg = new CImageViewDlg(m_imageViewDlg, this);
			m_imageViewDlg->Create(m_imageViewDlg->IDD, this);
			m_imageViewDlg->SetImages(std::move(img));
		}

		if (g_plan.m_windowPro) {
			m_static_rule.SetWindowTextW(m_operation->ruleName);
			m_addBlButton.ShowWindow(SW_SHOW);
			if (m_operation->isBlUser) {
				m_static_is_bl.ShowWindow(SW_SHOW);
			}
			CString tmp;
			tmp.Format(_T(" 缓存违规计次：%d"), m_operation->ruleBreakCount);
			m_static_break_rule_count.SetWindowTextW(tmp);
			m_static_break_rule_count.ShowWindow(SW_SHOW);
			tmp.Format(_T(" 确认线程：%d"), m_operation->confirmQueneLeft);
			m_static_con_quene_count.SetWindowTextW(tmp);
			m_static_con_quene_count.ShowWindow(SW_SHOW);
			m_banDirectButton.ShowWindow(SW_SHOW);
			m_deleteOnlyButton.ShowWindow(SW_SHOW);
		}

		DWORD curTime = GetTickCount();
		if (curTime - lastTime > 10 * 1000) // 10秒未确认则禁止确认一段时间防止误操作
		{
			m_yesButton.EnableWindow(FALSE);
			m_noButton.EnableWindow(FALSE);
			SetTimer(0, 1000, NULL);
		}
		lastTime = curTime;
	}
	MessageBeep(MB_ICONQUESTION);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CConfirmDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		KillTimer(0);
		m_yesButton.EnableWindow(TRUE);
		m_noButton.EnableWindow(TRUE);
		m_yesButton.SetFocus();
	} 

	CDialog::OnTimer(nIDEvent);
}

// 浏览器
void CConfirmDlg::OnBnClickedButton1()
{
	if (m_operation == NULL)
		return;

	CString url;
	if (m_operation->object->m_type == TBObject::THREAD) // 主题
		url = _T("https://tieba.baidu.com/p/") + m_operation->object->tid;
	else if (m_operation->object->m_type == TBObject::POST) // 帖子
	{
		PostInfo* post = (PostInfo*)m_operation->object.get();
		url.Format(_T("https://tieba.baidu.com/p/%s?pid=%s#%s"), (LPCTSTR)post->tid,
			(LPCTSTR)post->pid, (LPCTSTR)post->pid);
	}
	else if (m_operation->object->m_type == TBObject::LZL) // 楼中楼
	{
		LzlInfo* lzl = (LzlInfo*)m_operation->object.get();
		url.Format(_T("https://tieba.baidu.com/p/%s?pid=%s&cid=%s#%s"), (LPCTSTR)lzl->tid, 
			(LPCTSTR)lzl->cid, (LPCTSTR)lzl->cid, (LPCTSTR)lzl->cid);
	}

	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

// 加到黑名单
void CConfirmDlg::OnBnClickedButtonAddBl()
{
	if (m_operation == NULL)
		return;

	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->OnBnClickedButton5();
	dlg->m_settingDlg->ShowBlackListRulePage();
	dlg->m_settingDlg->m_blackListRulesPage->
		SetPreFillInfo(m_operation->object->authorShowName, GetPortraitFromString(m_operation->object->authorPortraitUrl));
	dlg->m_settingDlg->SetActiveWindow();
}

void CConfirmDlg::OnStnDblclickStaticRule()
{
	if (m_operation == NULL)
		return;
	if (m_operation->ruleType == RULE_TYPE_ILLEGA_RULE) {
		CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
		dlg->OnBnClickedButton5();
		dlg->m_settingDlg->ShowIllegalRulesPage();
		int index = dlg->m_settingDlg->m_illegalRulesPage->
			FindIndexFromStr(m_operation->ruleName);
		dlg->m_settingDlg->m_illegalRulesPage->ScrollToIndex(index);
		dlg->m_settingDlg->m_illegalRulesPage->SetSelectedRow(index);
		dlg->m_settingDlg->m_illegalRulesPage->OnClickedButton3();
	}
	else if (m_operation->ruleType == RULE_TYPE_BLACK_LIST) {
		CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
		dlg->OnBnClickedButton5();
		dlg->m_settingDlg->ShowBlackListRulePage();
		int index = dlg->m_settingDlg->m_blackListRulesPage->
			FindIndexFromStr(GetStringBefore(m_operation->ruleName, _T(" : ")));
		dlg->m_settingDlg->m_blackListRulesPage->ScrollToIndex(index);
		dlg->m_settingDlg->m_blackListRulesPage->SetSelectedRow(index);
		dlg->m_settingDlg->m_blackListRulesPage->OnClickedButton3();
	}
}

// 只删不封不计次
void CConfirmDlg::OnBnClickedButtonDeleteOnly()
{
	if (m_operation == NULL)
		return;
	m_ruleType = RULE_TYPE_DELETE_ONLY;
	CDialog::OnOK();
}

// 立即删封
void CConfirmDlg::OnBnClickedButtonBanNow()
{
	if (m_operation == NULL)
		return;
	m_ruleType = RULE_TYPE_BAN_DIRECTLY;
	CDialog::OnOK();
}
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
#include "TiebaManagerDlg.h"


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
	DDX_Control(pDX, IDC_ENHANCED_LZL,	m_acedEnhancedLzlCheck);
	DDX_Control(pDX, IDC_TEXT_CHECK_BAN,m_static_check_d2f);
	DDX_Control(pDX, IDC_CHECK_BAN,		m_acedBlackCheckBanCheck);
	DDX_Control(pDX, IDC_HIDDEN,		m_tempCheck);
	DDX_Control(pDX, IDC_BTN_CHECK_BAN, m_btnCheckBanNow);
}


BEGIN_MESSAGE_MAP(CAcedPage, CNormalDlg)
	ON_BN_CLICKED(IDC_ENHANCED_LZL,		&CAcedPage::OnBnClickedCheckLzl)
	ON_BN_CLICKED(IDC_BTN_CHECK_BAN,	&CAcedPage::OnBnClickedCheckBan)
	ON_STN_DBLCLK(IDC_TEXT_CHECK_BAN,	&CAcedPage::OnBnClickedStaticD2f)
	ON_WM_TIMER()
END_MESSAGE_MAP()
#pragma endregion

// CAcedPage 消息处理程序

// 进阶楼中楼 开启提示
void CAcedPage::OnBnClickedCheckLzl()
{
	if (m_acedEnhancedLzlCheck.GetCheck()) {
		CString tmp = L"1 开启后增加额外请求获取帖子前10不被折叠的楼中楼\n\r"
			L"2 流量增加，扫描时间增加，所以扫描间隔建议60s以上\n\r"
			L"3 获取的额外楼中楼用户等级可能无法获取，因此启用该功能后相关规则需要修改\n\r"
			L"4 相关规则建议全部重新检查，尤其是含有次数等的正则规则\n\r\n\r"
			L"是否开启？";
		int result = AfxMessageBox(tmp, MB_ICONINFORMATION | MB_YESNO);
		if (result != IDYES) {
			m_acedEnhancedLzlCheck.SetCheck(false);
		}
	}
}

// 解锁立即校验按钮
void CAcedPage::OnBnClickedStaticD2f()
{
	CString tmp = L"1 使用立即校验功能前先保存配置，开始后程序会往操作队列中增加查询请求\n\r"
		L"2 开始后不要点击设置界面的 应用改动，或确定按钮，否则查询结果会被重置并覆盖。\n\r"
		L"3 点击取消按钮关闭当前按钮可以避免设置覆盖\n\r"
		L"4 要修改设置请等待查询结束后，重新打开设置界面，这样才能加载最新结果\n\r\n\r"
		L"是否开启？";
	int result = AfxMessageBox(tmp, MB_ICONINFORMATION | MB_YESNO);
	if (result == IDYES) {
		m_btnCheckBanNow.ShowWindow(SW_SHOW);
	}

}

// 立即校验 黑名单 列表
void CAcedPage::OnBnClickedCheckBan()
{
	m_btnCheckBanNow.EnableWindow(FALSE);
	CTiebaManagerDlg::addUserD2fCheck();
	SetTimer(0, 2000, NULL);
}

void CAcedPage::ShowPlan(const CPlan& plan)
{
	// UpdateData(TRUE);
	m_acedEnhancedLzlCheck.SetCheck(plan.m_acedEnhancedLzl);		// Aced 楼中楼
	m_acedBlackCheckBanCheck.SetCheck(plan.m_acedBlackCheckBan);	// Aced 校验永封

	m_tempCheck.SetCheck(plan.m_hiddenFunction);					// 开发中暂用
}

void CAcedPage::ApplyPlanInDlg(CPlan& plan)
{
	*plan.m_acedEnhancedLzl = m_acedEnhancedLzlCheck.GetCheck();	// Aced 楼中楼
	*plan.m_acedBlackCheckBan = m_acedBlackCheckBanCheck.GetCheck();// Aced 校验永封

	*plan.m_hiddenFunction = m_tempCheck.GetCheck();				// 开发中暂用
}

void CAcedPage::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		KillTimer(0);
		m_btnCheckBanNow.EnableWindow(TRUE);
	}
	CDialog::OnTimer(nIDEvent);
}
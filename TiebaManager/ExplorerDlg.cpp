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

// ExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ExplorerDlg.h"

#include "TBMGlobal.h"
#include "TiebaManagerDlg.h"
#include "TiebaManager.h"
#include <TBMAPI.h>
#include <TiebaClawerProxy.h>
#include <TiebaOperate.h>
#include <update.h>

#include <Mmsystem.h>
#include <TBMCoreImageHelper.h>
#include "StringHelper.h"


// CExplorerDlg 对话框

IMPLEMENT_DYNAMIC(CExplorerDlg, CModelessDlg)

// 构造函数
CExplorerDlg::CExplorerDlg(CExplorerDlg*& pThis, CWnd* pParent /*=NULL*/) : CModelessDlg(CExplorerDlg::IDD, (CModelessDlg**)&pThis, pParent),
	m_pagesResize(&m_tab),
	m_exploreThreadPage(new CExploreThreadPage(GetTiebaOperate().GetForumName())),
	m_explorePostPage(new CExplorePostPage(GetTiebaOperate().GetForumID())),
	m_exploreLzlPage(new CExploreLzlPage())
{
	// 初始化m_pages
	int i = 0;
	m_pages[i++] = m_exploreThreadPage.get();
	m_pages[i++] = m_explorePostPage.get();
	m_pages[i++] = m_exploreLzlPage.get();

	m_imageViewDlg = NULL;
}

#pragma region MFC
CExplorerDlg::~CExplorerDlg()
{
}

void CExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_EDIT2, m_editInfo);
	DDX_Control(pDX, IDC_STATIC_IS_LZ, m_isLZ);
	DDX_Control(pDX, IDC_BUTTON1, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON2, m_banButton);
	DDX_Control(pDX, IDC_BUTTON3, m_explorerButton);
	DDX_Control(pDX, IDC_BUTTON_BL, m_addBlButton);
	DDX_Control(pDX, IDC_BTN_IMG_HEAD, m_imgHeadButton);
	DDX_Control(pDX, IDC_BTN_IMG_QR, m_imgQRButton);
}


BEGIN_MESSAGE_MAP(CExplorerDlg, CModelessDlg)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CExplorerDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON1, &CExplorerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CExplorerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CExplorerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_BL, &CExplorerDlg::OnBnClickedButtonAddBl)
	ON_BN_CLICKED(IDC_BTN_IMG_HEAD, &CExplorerDlg::OnBnClickedImgHead)
	ON_BN_CLICKED(IDC_BTN_IMG_QR, &CExplorerDlg::OnBnClickedImgQR)
END_MESSAGE_MAP()
#pragma endregion

// CExplorerDlg 消息处理程序

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 改变尺寸
void CExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	CModelessDlg::OnSize(nType, cx, cy);
	m_pagesResize.Resize();
}

// 控件颜色
HBRUSH CExplorerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->m_hWnd == m_isLZ.m_hWnd)
		pDC->SetTextColor(RGB(85, 139, 246));
	return hbr;
}

// 切换标签
void CExplorerDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// 初始化
BOOL CExplorerDlg::OnInitDialog()
{
	CModelessDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	// 初始化m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("主题"));
	m_tab.InsertItem(i++, _T("帖子"));
	m_tab.InsertItem(i++, _T("楼中楼"));

	// 初始化各页
#define CREATE_PAGE(page) page->Create(page->IDD, &m_tab)
	CREATE_PAGE(m_exploreThreadPage);
	CREATE_PAGE(m_explorePostPage);
	CREATE_PAGE(m_exploreLzlPage);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	m_resize.AddControl(&m_tab, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_deleteButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	m_resize.AddControl(&m_banButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	m_resize.AddControl(&m_explorerButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	m_resize.AddControl(&m_addBlButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	m_resize.AddControl(&m_imgHeadButton, RT_KEEP_DIST_TO_TOP, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_imgQRButton, RT_KEEP_DIST_TO_TOP, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_editInfo, RT_KEEP_DIST_TO_TOP, this, RT_KEEP_DIST_TO_BOTTOM, this);
	for (i = 0; i < _countof(m_pages); i++)
		m_pagesResize.AddControl(m_pages[i], RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_tab, RT_KEEP_DIST_TO_BOTTOM, &m_tab);

	SetWindowText(_T("贴吧浏览器-") + UPDATE_CURRENT_VERSION);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 删除
void CExplorerDlg::OnBnClickedButton1()
{
	int tabIndex = m_tab.GetCurSel(); 
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);


	CString code;
	if (tabIndex == 0) // 主题
	{
		TapiThreadInfo& thread = m_exploreThreadPage->m_threads[index];
		code = GetTiebaOperate().DeleteThread(thread.tid);
		if (code == _T("0"))
			g_userCache.m_deletedTID.insert(_ttoi64(thread.tid));
	}
	else if (tabIndex == 1) // 帖子
		code = GetTiebaOperate().DeletePost(m_explorePostPage->m_tid, m_explorePostPage->m_posts[index].pid);
	else // 楼中楼
		code = GetTiebaOperate().DeleteLZL(m_explorePostPage->m_tid, m_exploreLzlPage->m_lzls[index].cid);


	if (code != _T("0"))
		AfxMessageBox(_T("删除失败，错误代码" + code + _T("(") + GetTiebaErrorText(code) + _T(")")), MB_ICONERROR);
	else {
		if (g_pTbmCoreConfig->m_playSound)
			sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
	}
}

// 封禁
void CExplorerDlg::OnBnClickedButton2()
{
	int tabIndex = m_tab.GetCurSel();
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);


	CString author, portrait, nick_name;
	if (tabIndex == 0) // 主题
	{
		author = m_exploreThreadPage->m_threads[index].author;
		portrait = m_exploreThreadPage->m_threads[index].authorPortraitUrl;
		nick_name = m_exploreThreadPage->m_threads[index].authorShowName;
	}
	else if (tabIndex == 1) // 帖子
	{
		author = m_explorePostPage->m_posts[index].author;
		portrait = m_explorePostPage->m_posts[index].authorPortraitUrl;
		nick_name = m_explorePostPage->m_posts[index].authorShowName;
	}
	else // 楼中楼
	{
		author = m_exploreLzlPage->m_lzls[index].author;
		portrait = m_exploreLzlPage->m_lzls[index].authorPortraitUrl;
		nick_name = m_exploreLzlPage->m_lzls[index].authorShowName;
	}

	/*if (pid == _T(""))
	{
		AfxMessageBox(_T("封禁失败(获取帖子ID失败)"), MB_ICONERROR);
		return;
	}*/
	CString code = (g_plan.m_banClientInterface) ? GetTiebaOperate().BanIDClient(author, portrait, nick_name) : GetTiebaOperate().BanID(author, portrait, nick_name);
	if (code != _T("0"))
		AfxMessageBox(_T("封禁失败，错误代码" + code + _T("(") + GetTiebaErrorText(code) + _T(")")), MB_ICONERROR);
	else {
		if (g_pTbmCoreConfig->m_playSound)
			sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
	}
}

// 浏览器
void CExplorerDlg::OnBnClickedButton3()
{
	CString url;
	if (m_tab.GetCurSel() == 0)
	{
		POSITION pos = m_exploreThreadPage->m_list.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int index = m_exploreThreadPage->m_list.GetNextSelectedItem(pos);
		url = _T("https://tieba.baidu.com/p/") + m_exploreThreadPage->m_threads[index].tid;
	}
	else
	{
		POSITION pos = m_exploreThreadPage->m_list.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int index = m_exploreThreadPage->m_list.GetNextSelectedItem(pos);
		CString page;
		m_exploreThreadPage->m_edit.GetWindowText(page);
		url = _T("https://tieba.baidu.com/p/") + m_explorePostPage->m_tid + _T("?pn=") + page;
	}
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

// 快捷黑名单
void CExplorerDlg::OnBnClickedButtonAddBl()
{
	int tabIndex = m_tab.GetCurSel();
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);

	CString author, portrait, nick_name;
	if (tabIndex == 0) // 主题
	{
		author = m_exploreThreadPage->m_threads[index].author;
		portrait = m_exploreThreadPage->m_threads[index].authorPortraitUrl;
		nick_name = m_exploreThreadPage->m_threads[index].authorShowName;
	}
	else if (tabIndex == 1) // 帖子
	{
		author = m_explorePostPage->m_posts[index].author;
		portrait = m_explorePostPage->m_posts[index].authorPortraitUrl;
		nick_name = m_explorePostPage->m_posts[index].authorShowName;
	}
	else // 楼中楼
	{
		author = m_exploreLzlPage->m_lzls[index].author;
		portrait = m_exploreLzlPage->m_lzls[index].authorPortraitUrl;
		nick_name = m_exploreLzlPage->m_lzls[index].authorShowName;
	}

	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->OnBnClickedButton5();
	dlg->m_settingDlg->ShowBlackListRulePage();
	dlg->m_settingDlg->m_blackListRulesPage->
		SetPreFillInfo(nick_name, GetPortraitFromString(portrait));
	dlg->SetActiveWindow();
	dlg->m_settingDlg->SetActiveWindow();
}

// 图片文件头
void CExplorerDlg::OnBnClickedImgHead()
{
	int tabIndex = m_tab.GetCurSel();
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		m_editInfo.SetWindowText(_T("图片文件头：\r\n未选中"));
		return;
	}
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);
	std::vector<CString> urls;
	if (tabIndex == 0) // 主题
	{
		GetImageUrls(m_exploreThreadPage->m_threads[index], urls);
	}
	else if (tabIndex == 1) // 帖子
	{
		GetImageUrls(m_explorePostPage->m_posts[index], urls);
	}
	else // 楼中楼 没有图片
	{
		m_editInfo.SetWindowText(_T("图片文件头：\r\n楼中楼哪有图片？头像就不管了！"));
		return;
	}
	// urls 处理
	CString ret = _T("图片文件头：");
	for (const auto& i : urls) {
		if (i.Find(AUTHOR_PORTRAIT_LEFT) == -1) {
			ret += _T("\r\n图片：") + GetImgHead(i, false);
		}
		else {
			ret += _T("\r\n头像：") + GetImgHead(i, false);
		}
	}
	m_editInfo.SetWindowText(ret);
}

// 二维码识别
void CExplorerDlg::OnBnClickedImgQR()
{
	int tabIndex = m_tab.GetCurSel();
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		m_editInfo.SetWindowText(_T("二维码识别：\r\n未选中"));
		return;
	}
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);
	std::vector<CString> urls;
	if (tabIndex == 0) // 主题
	{
		GetImageUrls(m_exploreThreadPage->m_threads[index], urls, FALSE, TRUE);
	}
	else if (tabIndex == 1) // 帖子
	{
		GetImageUrls(m_explorePostPage->m_posts[index], urls, FALSE, TRUE);
	}
	else // 楼中楼 没有图片
	{
		m_editInfo.SetWindowText(_T("二维码识别：\r\n楼中楼哪有图片？头像就不管了！"));
		return;
	}
	// urls 处理
	CString ret = _T("二维码识别:");
	CString tmp;
	for (const auto& i : urls) {
		QRCodeScan(i, tmp, false);
		if (i.Find(AUTHOR_PORTRAIT_LEFT) == -1) {
			ret += _T("\r\n图片：") + tmp;
		}
		else {
			ret += _T("\r\n头像：") + tmp;
		}
	}
	m_editInfo.SetWindowText(ret);

}

// 打开浏览图片对话框
void CExplorerDlg::ViewImages(std::unique_ptr<std::vector<CString> > img)
{
	if (m_imageViewDlg == NULL)
	{
		m_imageViewDlg = new CImageViewDlg(m_imageViewDlg, this);
		m_imageViewDlg->Create(m_imageViewDlg->IDD, this);
	}
	m_imageViewDlg->SetImages(std::move(img));
}

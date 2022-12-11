/*
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

// ToolsPage.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolsPage.h"
#include "SettingDlg.h"
#include "TiebaManagerDlg.h"
#include "TiebaManager.h"
#include <Update.h>
#include <MiscHelper.h>


// CToolsPage 对话框

IMPLEMENT_DYNAMIC(CToolsPage, CNormalDlg)

CToolsPage::CToolsPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CToolsPage::IDD, pParent)
{
}

#pragma region MFC
CToolsPage::~CToolsPage()
{
}

void CToolsPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_HELP_V, m_staticHelpV);
	DDX_Control(pDX, IDC_STATIC_CURL_V, m_staticCurlV);
	DDX_Control(pDX, IDC_STATIC_CURL_EXE_V, m_staticCurlExeV);
	DDX_Control(pDX, IDC_STATIC_7Z_V, m_static7zV);
	DDX_Control(pDX, IDC_STATIC_CURL_V_NEW, m_staticCurlVnew);
	DDX_Control(pDX, IDC_STATIC_CURL_EXE_V_NEW, m_staticCurlExeVnew);
	DDX_Control(pDX, IDC_STATIC_7Z_V_NEW, m_static7zVnew);
}

BEGIN_MESSAGE_MAP(CToolsPage, CNormalDlg)
	ON_BN_CLICKED(IDC_BUTTON_OEPN_QQ, &CToolsPage::OnBnClickedButtonOepnQq)
	ON_BN_CLICKED(IDC_BUTTON_LIBCURL_UPDATE, &CToolsPage::OnBnClickedButtonLibcurlUpdate)
END_MESSAGE_MAP()
#pragma endregion

// CToolsPage 消息处理程序
// 扫描根目录，查找各个库基本信息    // 参考 https://blog.csdn.net/u012005313/article/details/46490437
void CToolsPage::InitFilesList()
{
	// 贴吧管理器辅助工具 v1.1.2.723.exe
	HANDLE hFile;
	CString tmp, lpFileName = _T(".\\贴吧管理器辅助工具*.exe");
	WIN32_FIND_DATA pNextInfo;
	hFile = FindFirstFile(lpFileName, &pNextInfo);
	BOOLEAN hasMultExe = FALSE;
	CString latestHelpV;
	if (hFile == INVALID_HANDLE_VALUE) {
		// 没有数据
		m_staticHelpV.SetWindowText(NOT_FOUND);
		latestHelpV = _T("");
	}
	else {
		tmp = GetFileVersionString(pNextInfo.cFileName, NULL);
		latestHelpV = tmp;
		m_staticHelpV.SetWindowText(tmp);
		while (FindNextFile(hFile, &pNextInfo)) {
			latestHelpV = GetHigherVersionString(latestHelpV, GetFileVersionString(pNextInfo.cFileName, NULL));
			hasMultExe = TRUE;
			tmp += _T(" / ") + GetFileVersionString(pNextInfo.cFileName, NULL);
			m_staticHelpV.SetWindowText(tmp);
		}
	}
	if (hasMultExe) {
		tmp += _T(" | 最高版本:") + latestHelpV;
		m_staticHelpV.SetWindowText(tmp);
	}

	// libcurl.dll 肯定会有，不然管理器都打不开
	lpFileName = _T("libcurl.dll");
	tmp = GetFileVersionString(lpFileName, NULL);
	m_staticCurlV.SetWindowText(tmp);

	// curl.exe
	lpFileName = _T(".\\curl.exe");
	hFile = FindFirstFile(lpFileName, &pNextInfo);
	if (hFile == INVALID_HANDLE_VALUE) {
		// 没有数据
		m_staticCurlExeV.SetWindowText(NOT_FOUND);
	}
	else {
		//tmp = GetFileVersionString(_T("curl.exe"), NULL);
		tmp = GetFileVersionString(pNextInfo.cFileName, NULL);
		m_staticCurlExeV.SetWindowText(tmp);
	}

	// 7z.exe
	lpFileName = _T(".\\7za.exe");
	hFile = FindFirstFile(lpFileName, &pNextInfo);
	if (hFile == INVALID_HANDLE_VALUE) {
		// 没有数据
		m_static7zV.SetWindowText(NOT_FOUND);
	}
	else {
		tmp = GetFileVersionString(pNextInfo.cFileName, NULL);
		m_static7zV.SetWindowText(tmp);
	}

}

void CToolsPage::UpdateVersionLabel() {
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	m_staticCurlVnew.SetWindowText(dlg->m_libcurlV);
	m_staticCurlExeVnew.SetWindowText(dlg->m_curlV);
	m_static7zVnew.SetWindowText(dlg->m_7zaV);
}

void CToolsPage::OnBnClickedButtonOepnQq()
{
	ShellExecute(NULL, _T("open"), QQ_QUN_URL, NULL, NULL, SW_SHOWNORMAL);
}


void CToolsPage::OnBnClickedButtonLibcurlUpdate()
{
	CString UPDATE_PACK_URL_CURL = _T("http://tieba.bakasnow.com/TiebaManager/?df&f=libcurl.dll");
	// exe所在目录，不一定是当前目录
	CString relativeDir;
	GetModuleFileName(GetModuleHandle(NULL), relativeDir.GetBuffer(MAX_PATH), MAX_PATH);
	relativeDir.ReleaseBuffer();
	int pos = relativeDir.ReverseFind(_T('\\'));
	relativeDir = pos == -1 ? _T(".\\") : relativeDir.Left(pos + 1);
	
	CString UPDATE_DIR_PATH = _T("Update\\");
	CString updateDir = relativeDir + UPDATE_DIR_PATH;
	// 需要更新的文件
	std::vector<CUpdateInfo::FileInfo> updateFiles;

	CreateDir(updateDir);
	CString filesPackUrl, fileName, cmdStr, tmpStr;
	cmdStr = _T("");
	filesPackUrl = UPDATE_PACK_URL_CURL;
	fileName = _T("libcurl.dll");

	// 关闭指令显示
	cmdStr += _T("@echo off");
	cmdStr += _T("\necho ============================================");
	cmdStr += _T("\necho 贴吧管理器更新辅助程序，请不要手动关闭本程序");
	cmdStr += _T("\necho ============================================");
	cmdStr += _T("\necho 开始下载libcurl.dll......");
	cmdStr += _T("\necho.");
	// 下载指令
	tmpStr.Format(_T("\"%scurl.exe\" -o \"%s%s\" \"%s\""), relativeDir, updateDir, fileName, filesPackUrl);
	cmdStr += _T("\n");
	cmdStr += tmpStr;
	cmdStr += _T("\necho.");
	cmdStr += _T("\necho 下载结束......");
	cmdStr += _T("\necho 延时5秒关闭......");
	cmdStr += _T("\nchoice /t 1 /d y /n >nul");
	cmdStr += _T("\necho 延时4秒关闭......");
	cmdStr += _T("\nchoice /t 1 /d y /n >nul");
	cmdStr += _T("\necho 延时3秒关闭......");
	cmdStr += _T("\nchoice /t 1 /d y /n >nul");
	cmdStr += _T("\necho 延时2秒关闭......");
	cmdStr += _T("\nchoice /t 1 /d y /n >nul");
	cmdStr += _T("\necho 延时1秒关闭......");
	cmdStr += _T("\nchoice /t 1 /d y /n >nul");
	cmdStr += _T("\ndel %0");
	WriteString(cmdStr, updateDir + _T("libcurl.bat"));
	tmpStr.Format(_T("\"%slibcurl.bat\""), updateDir);
	// 阻塞调用CMD，等待下载完成
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = tmpStr;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWNORMAL;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	if (!PathFileExists(updateDir + fileName)) {
		AfxMessageBox(_T("文件\"") + fileName + _T("\"不存在！请到群里反馈或重试！"), MB_ICONERROR);
		return;
	}
	
	if ((PathFileExists(updateDir + fileName + _T(".bak"))
		&& !DeleteFile(updateDir + fileName + _T(".bak")))
		|| (PathFileExists(relativeDir + fileName)
			&& !MoveFile(relativeDir + fileName, updateDir + fileName + _T(".bak"))))
	{
		AfxMessageBox(_T("移动文件\"") + relativeDir + fileName + _T("\"失败！"), MB_ICONERROR);
		return;
	}
	if (!MoveFile(updateDir + fileName, relativeDir + fileName))
	{
		AfxMessageBox(_T("移动文件\"") + updateDir + fileName + _T("\"失败！"), MB_ICONERROR);
		return;
	}
	AfxMessageBox(_T("更新完毕，重启本程序后生效"));


End:
	CoUninitialize();
}

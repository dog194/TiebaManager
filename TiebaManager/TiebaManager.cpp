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


// TiebaManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include <windows.h>
#include <wincrypt.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
//#include <vld.h> // Visual Leak Detector，检测内存泄漏的，可以不要

#include "TiebaManager.h"
#include "TiebaManagerDlg.h"
#include <Dbghelp.h>

#include "TBMConfigPath.h"

#include "TBMGlobal.h"
#include "TBMListeners.h"
#include "PluginManager.h"
#include "ConditionGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的一个 CTiebaManagerApp 对象
CTiebaManagerApp theApp;


// CTiebaManagerApp

BEGIN_MESSAGE_MAP(CTiebaManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTiebaManagerApp 构造
CTiebaManagerApp::CTiebaManagerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// CTiebaManagerApp 初始化
BOOL CTiebaManagerApp::InitInstance()
{
	// 基础库校验
	if (Check3rdDll() == FALSE) {
		// 基础库校验失败，直接结束程序
		exit(0);
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();


	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	CTiebaManagerDlg dlg;
	m_pMainWnd = &dlg;
	g_pLog = &dlg.m_log;

	// 初始化
	Init();

	// 载入主窗口
	dlg.DoModal();
	m_pMainWnd = NULL;
	g_pLog = NULL;


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

// 异常处理
LONG WINAPI CTiebaManagerApp::ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	CFile file;
	HMODULE hModule;
	WCHAR eModuleName[MAX_PATH] = { 0 };
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)ExceptionInfo->ExceptionRecord->ExceptionAddress, &hModule);
	GetModuleFileName(hModule, eModuleName, ARRAYSIZE(eModuleName));
	CString eModuleNameStr, eMessage;
	eModuleNameStr.Format(_T("%s"), eModuleName);
	if (StringIncludes(eModuleNameStr, _T("MMDevAPI.DLL"), false, true)) {
		AfxMessageBox(_T("MMDevAPI.DLL 这是一个已知的崩溃,不会影响正常使用,可以正常关闭程序重开,或者就这样放着."), MB_ICONERROR);
		return EXCEPTION_CONTINUE_EXECUTION;
		//return EXCEPTION_CONTINUE_EXECUTION;
		//EXCEPTION_EXECUTE_FAULT;
		// EXCEPTION_CONTINUE_SEARCH
	}
	eMessage.Format(_T("程序崩溃了，先别点确定，如果其他窗口运行正常，请无视崩溃信息，或正常关闭窗口保存缓存记录，重开，如果其他窗口卡死请把exception.dmp文件发到群里帮助调试.(%s)"), eModuleNameStr);
	if (file.Open(_T("exception.dmp"), CFile::modeCreate | CFile::modeWrite))
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = GetCurrentThreadId();
		einfo.ExceptionPointers = ExceptionInfo;
		einfo.ClientPointers = FALSE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithIndirectlyReferencedMemory,
			&einfo, NULL, NULL);
	}
	AfxMessageBox(eMessage, MB_ICONERROR);
	return EXCEPTION_EXECUTE_HANDLER;
}

// 初始化
void CTiebaManagerApp::Init()
{
	// 添加异常处理
	SetUnhandledExceptionFilter(ExceptionHandler);

	// 本地化输出，用于CStdioFile
	_tsetlocale(LC_ALL, _T(".936"));

	// 初始化配置文件路径
	CString cd;
	cd.ReleaseBuffer(GetCurrentDirectory(MAX_PATH, cd.GetBuffer(MAX_PATH)));
	GLOBAL_CONFIG_PATH = cd + GLOBAL_CONFIG_PATH;
	USERS_DIR_PATH = cd + USERS_DIR_PATH;

	// 判断一下有没有解压
	CString tmpDir;
	tmpDir.ReleaseBuffer(GetEnvironmentVariable(_T("TMP"), tmpDir.GetBuffer(MAX_PATH), MAX_PATH));
	if (cd.Find(tmpDir) != -1)
		AfxMessageBox(_T("请先解压再运行，否则无法保存设置"), MB_ICONINFORMATION);

	// 初始化各模块 ////////////////////////////////////////////////////////////////

	// 外部模块
	g_pTbmCoreConfig = &g_plan;
	g_pUserCache = &g_userCache;
	g_pTiebaOperate = &g_tiebaOperate;

	// 内部模块
	CTBMListeners::GetInstance();
	CConditionGUIManager::GetInstance();

	// 插件
	CPluginManager::GetInstance();
}

// 释放
int CTiebaManagerApp::ExitInstance()
{
	TRACE(_T("释放g_plan\n"));
	g_plan.m_illegalRules->clear();
	g_plan.m_trustedRules->clear();
	g_plan.m_blackListRules->clear();
	TRACE(_T("释放CConditionGUIManager\n"));
	CConditionGUIManager::GetInstance().Uninit();
	TRACE(_T("释放PluginManager\n"));
	CPluginManager::GetInstance().Uninit();
	TRACE(_T("退出程序\n"));

	return CWinApp::ExitInstance();
}

BOOL CTiebaManagerApp::Check3rdDll() {
	CString cPath, info;
	cPath.ReleaseBuffer(GetModuleFileName(NULL, cPath.GetBuffer(MAX_PATH), MAX_PATH));
	if (cPath.Replace(_T("TiebaManager.exe"), _T("")) != 1) {
		AfxMessageBox(_T("本体文件名异常 TiebaManager.exe"), MB_ICONERROR);
		return FALSE;
	}
	std::map<CString, std::tuple<std::string, CString>> dllList;
	dllList[L"7za.exe"] =			std::make_tuple("43141e85e7c36e31b52b22ab94d5e574", _T("19.0.0"));
	dllList[L"abseil_dll.dll"] =	std::make_tuple("e982db6ff3509ecc4e605952a09e3365", _T("搭配24.0的Protobuf的版本"));
	dllList[L"curl.exe"] =			std::make_tuple("a29f0441ebcc19dcf73352ce5f0e1f94", _T("7.78.0"));
	dllList[L"libcurl.dll"] =		std::make_tuple("98618ff785f6fa8d3c2022c5dc2241f3", _T("7.78.0"));
	dllList[L"libprotobuf.dll"] =	std::make_tuple("04e77a54562e24d4b2631f0579c54d84", _T("24.0.0"));
	dllList[L"libprotobuf-lite.dll"] =		std::make_tuple("77e7b0be675baa64457bed5a6b63d62b", _T("24.0.0"));
	dllList[L"opencv_core453.dll"] =		std::make_tuple("1e69b396576dc9eba0291454fbafda97", _T("4.5.3"));
	dllList[L"opencv_dnn453.dll"] =			std::make_tuple("96c9589e5ce0795d6df128188029fc24", _T("4.5.3"));
	dllList[L"opencv_imgcodecs453.dll"] =	std::make_tuple("adbd6f9c069f77a05fbf9ecaea424233", _T("4.5.3"));
	dllList[L"opencv_imgproc453.dll"] =		std::make_tuple("6674e91107254ac1898ee05da4510b3e", _T("4.5.3"));
	dllList[L"opencv_wechat_qrcode453.dll"] = std::make_tuple("858f0493df447f6adbbd7644cc267ac0", _T("4.5.3"));
	dllList[L"sqlite3.dll"] =	std::make_tuple("084fb64052c55679d78f122b9e5d2e1b", _T("3.41.2"));
	dllList[L"tinyxml2.dll"] =	std::make_tuple("b7a0648e9a40e9599a7ed2e4ee904c40", _T("9.0.0"));
	info = _T("");
	for (const auto& dll : dllList) {
		std::string dll_md5 = GetFileMd5(cPath, dll.first);
		CString c_dll_md5(dll_md5.c_str());
		CString c_md5_record(std::get<0>(dll.second).c_str());
		if (dll_md5 != std::get<0>(dll.second)) {
			info += _T("文件名：") + dll.first + _T(" 缺少 - ") + c_dll_md5 + _T("\n");
			info += _T("所需版本：") + std::get<1>(dll.second) + _T(" - ") + c_md5_record + _T("\n");
		}
	}
	if (info != _T("")) {
		info += _T("\n请到群里单独下载基础库包解压到根目录 ※覆盖更新※");
		AfxMessageBox(info, MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

std::string CTiebaManagerApp::GetFileMd5(const CString& fPath, const CString& fName) {
	std::ifstream file(fPath + fName, std::ios::binary);
	if (!file) {
		return "NO FOUND";
	}

	// Acquire a cryptographic provider context handle
	HCRYPTPROV hProv = 0;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		// "CryptAcquireContext failed"
		return "ERROR";
	}

	// Create an empty hash object
	HCRYPTHASH hHash = 0;
	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
		// "CryptCreateHash failed"
		CryptReleaseContext(hProv, 0);
		return "ERROR";
	}

	// Hash the data
	char buf[1024];
	while (file.read(buf, sizeof(buf))) {
		if (!CryptHashData(hHash, reinterpret_cast<BYTE*>(buf), sizeof(buf), 0)) {
			// "CryptHashData failed"
			CryptDestroyHash(hHash);
			CryptReleaseContext(hProv, 0);
			return "ERROR";
		}
	}
	if (!CryptHashData(hHash, reinterpret_cast<BYTE*>(buf), static_cast<DWORD>(file.gcount()), 0)) {
		// "CryptHashData failed"
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return "ERROR";
	}

	// Get the hash value
	DWORD cbHash = 16;
	BYTE rgbHash[16];
	if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
		// "CryptGetHashParam failed"
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return "ERROR";
	}

	std::ostringstream oss;
	// Print the result
	for (int i = 0; i < cbHash; i++) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rgbHash[i]);
	}
	std::string md5Str = oss.str();

	// Clean up
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return md5Str;
}
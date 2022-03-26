/*
Copyright (C) 2011-2021  xfgryujk Dog194
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

#include "stdafx.h"
#include <Update.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include <Md5.h>


// 当前版本，每次更新后修改，也可以不是日期
UPDATE_API const CString UPDATE_CURRENT_VERSION = _T("220327(2.52)");
UPDATE_API const int UPDATE_CURRENT_VERSION_NUM = 220327; //使用整数形式，更方便版本判断

static const CString MANUALLY_UPDATE_URL = _T("http://tieba.bakasnow.com/TiebaManager/?download&client=%d");
static const CString QQ_QUN_UPDATE_URL = _T("https://qm.qq.com/cgi-bin/qm/qr?k=IbZJQTp42ZNuEQJRKbyyn0LTD1iCgEtT");
UPDATE_API const CString UPDATE_INFO_URL = _T("http://tieba.bakasnow.com/TiebaManager/?update&client=%d");
static const CString UPDATE_PACK_URL = _T("http://tieba.bakasnow.com/TiebaManager/?download&client=%d");

static const CString UPDATE_DIR_PATH = _T("Update\\"); // 只能是一级目录，因为批处理里用了相对路径
static const CString UPDATE_BAT_NAME = _T("Update.bat");
static const CString BACKUP_DIR_PATH = _T("Backup\\"); // 只能是一级目录，因为批处理里用了相对路径
static const CString RESTORE_BAT_NAME = _T("Restore.bat");


#pragma region CUpdateInfo
UPDATE_API DECLEAR_READ(CUpdateInfo::FileInfo)
{
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<CString> dir("Dir");
	COption<CString> name("Name");
	COption<CString> url("URL");
	COption<CString> md5("MD5");
	dir.Read(*optionNode);
	name.Read(*optionNode);
	url.Read(*optionNode);
	md5.Read(*optionNode);

	m_value.dir = dir;
	m_value.name = name;
	m_value.url = url;
	m_value.md5 = md5;

	if (!IsValid(m_value))
		UseDefault();
}

UPDATE_API DECLEAR_WRITE(CUpdateInfo::FileInfo)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	tinyxml2::XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> dir("Dir");
	*dir = m_value.dir;
	dir.Write(*optionNode);
	COption<CString> name("Name");
	*name = m_value.name;
	name.Write(*optionNode);
	COption<CString> url("URL");
	*url = m_value.url;
	url.Write(*optionNode);
	COption<CString> md5("MD5");
	*md5 = m_value.md5;
	md5.Write(*optionNode);
}

CUpdateInfo::CUpdateInfo() : CConfigBase("UpdateInfo"),
	m_version("Version", _T("")),
	m_version_num("VersionNum", 0),
	m_updateLog("UpdateLog", _T("")),
	m_files("Files")
{
	m_options.push_back(&m_version);
	m_options.push_back(&m_version_num);
	m_options.push_back(&m_updateLog);
	m_options.push_back(&m_files);
}
#pragma endregion

// 用新版文件替换旧版的
static BOOL ReplaceFiles(const std::vector<CUpdateInfo::FileInfo>& files, const CString& relativeDir, const CString& updateDir)
{
	// 验证文件是否完整
	for (const auto& fileInfo : files)
	{
		if (!PathFileExists(updateDir + fileInfo.dir + fileInfo.name)) {
			AfxMessageBox(_T("文件\"") + fileInfo.dir + fileInfo.name + _T("\"不存在！请到群里反馈或重试！"), MB_ICONERROR);
			return FALSE;
		}
	}
;	// 原理：可移动正在运行的程序、DLL文件（但是不能删除），貌似可以用ReplaceFile这个API，但是没试过
	for (const auto& fileInfo : files)
	{
		if ((PathFileExists(updateDir + fileInfo.dir + fileInfo.name + _T(".bak"))
			&& !DeleteFile(updateDir + fileInfo.dir + fileInfo.name + _T(".bak")))
			|| (PathFileExists(relativeDir + fileInfo.dir + fileInfo.name)
			&& !MoveFile(relativeDir + fileInfo.dir + fileInfo.name, updateDir + fileInfo.dir + fileInfo.name + _T(".bak"))))
		{
			AfxMessageBox(_T("移动文件\"") + relativeDir + fileInfo.dir + fileInfo.name + _T("\"失败！"), MB_ICONERROR);
			return FALSE;
		}
		if (!MoveFile(updateDir + fileInfo.dir + fileInfo.name, relativeDir + fileInfo.dir + fileInfo.name))
		{
			AfxMessageBox(_T("移动文件\"") + updateDir + fileInfo.dir + fileInfo.name + _T("\"失败！"), MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
}

// 备份、写还原批处理
static BOOL Backup(const std::vector<CUpdateInfo::FileInfo>& updateFiles, const CString& relativeDir)
{
	CString backupDir = relativeDir + BACKUP_DIR_PATH;
	CreateDir(backupDir);

	CString bat = _T(R"(@echo off
:Restore
ping 127.0.0.1 -n 1 >nul
)");
	
	for (const auto& fileInfo : updateFiles)
	{
		if (PathFileExists(relativeDir + fileInfo.dir + fileInfo.name))
		{
			// 原来存在的文件，复制一份，在批处理中复制回去
			CreateDir(backupDir + fileInfo.dir);
			if (!CopyFile(relativeDir + fileInfo.dir + fileInfo.name, backupDir + fileInfo.dir + fileInfo.name, FALSE))
				return FALSE;
			bat += _T(R"(if exist ".\)") + fileInfo.dir + fileInfo.name
				+ _T(R"(" copy /y ".\)") + fileInfo.dir + fileInfo.name + _T(R"(" "..\)") + fileInfo.dir + fileInfo.name
				+ _T(R"(" || goto Restore
)");
		}
		else
		{
			// 原来不存在的文件，在批处理中删除掉
			bat += _T(R"(if exist "..\)") + fileInfo.dir + fileInfo.name
				+ _T(R"(" del /f "..\)") + fileInfo.dir + fileInfo.name
				+ _T(R"(" || goto Restore
)");
		}
	}

	bat += _T(R"(pause
)");

	return WriteString(bat, backupDir + RESTORE_BAT_NAME);
}

// 写更新批处理
//static BOOL WriteUpdateBat(const vector<CUpdateInfo::FileInfo>& files, const CString& updateDir)
//{
//	CString bat = _T(R"(@echo off
//:MoveFile
//ping 127.0.0.1 -n 1 >nul
//)");
//
//	// 移动文件
//	for (const auto& fileInfo : files)
//	{
//		bat += _T(R"(if exist ".\)") + fileInfo.dir + fileInfo.name 
//			+ _T(R"(" move /y ".\)") + fileInfo.dir + fileInfo.name + _T(R"(" "..\)") + fileInfo.dir + fileInfo.name 
//			+ _T(R"(" || goto MoveFile
//)");
//	}
//
//	// 删除更新目录
//	bat += _T(R"(cd ..
//rd /s /q )") + UPDATE_DIR_PATH + _T(R"(
//pause
//)");
//
//	return WriteString(bat, updateDir + UPDATE_BAT_NAME);
//}

// 下载需要更新的文件
static BOOL DownloadFiles(const std::vector<CUpdateInfo::FileInfo>& files, const CString& relativeDir,
	const CString& updateDir, std::vector<CUpdateInfo::FileInfo>& updateFiles)
{
	for (const auto& fileInfo : files)
	{
		CFile f;
		// 不需要更新此文件
		if (f.Open(relativeDir + fileInfo.dir + fileInfo.name, CFile::modeRead)
			&& GetMD5_File(f) == fileInfo.md5)
			continue;
		if (f.m_hFile != CFile::hFileNull)
			f.Close();

		updateFiles.push_back(fileInfo);

		// 已下载此文件
		if (f.Open(updateDir + fileInfo.dir + fileInfo.name, CFile::modeRead)
			&& GetMD5_File(f) == fileInfo.md5)
			continue;
		if (f.m_hFile != CFile::hFileNull)
			f.Close();

		// 下载
		std::unique_ptr<BYTE[]> buffer;
		ULONG size;
		if (HTTPGetRaw(fileInfo.url, &buffer, &size) != NET_SUCCESS)
		{
			AfxMessageBox(_T("下载文件\"") + fileInfo.dir + fileInfo.name + _T("\"失败！"), MB_ICONERROR);
			return FALSE;
		}
		CreateDir(updateDir + fileInfo.dir);
		if (!f.Open(updateDir + fileInfo.dir + fileInfo.name, CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox(_T("打开文件\"") + updateDir + fileInfo.dir + fileInfo.name + _T("\"失败！"), MB_ICONERROR);
			return FALSE;
		}
		f.Write(buffer.get(), size);
	}
	return TRUE;
}

// 更新线程
static void UpdateThread(CUpdateInfo* updateInfo_)
{
	std::unique_ptr<CUpdateInfo> updateInfo(updateInfo_);
	if (!CoInitializeHelper())
		return;

	// exe所在目录，不一定是当前目录
	CString relativeDir;
	GetModuleFileName(GetModuleHandle(NULL), relativeDir.GetBuffer(MAX_PATH), MAX_PATH);
	relativeDir.ReleaseBuffer();
	int pos = relativeDir.ReverseFind(_T('\\'));
	relativeDir = pos == -1 ? _T(".\\") : relativeDir.Left(pos + 1);

	CString updateDir = relativeDir + UPDATE_DIR_PATH;
	// 需要更新的文件
	std::vector<CUpdateInfo::FileInfo> updateFiles;
	
	/*
	// 下载需要更新的文件
	if (!DownloadFiles(updateInfo->m_files, relativeDir, updateDir, updateFiles))
		goto End;*/
	CreateDir(updateDir);
	CString filesPackUrl, fileName, cmdStr, tmpStr;
	cmdStr = _T("");
	filesPackUrl.Format(UPDATE_PACK_URL, UPDATE_CURRENT_VERSION_NUM);
	fileName = _T("TBGuanLiQi.zip");

	// 关闭指令显示
	cmdStr += _T("@echo off");
	cmdStr += _T("\necho ============================================");
	cmdStr += _T("\necho 贴吧管理器更新辅助程序，请不要手动关闭本程序");
	cmdStr += _T("\necho ============================================");
	cmdStr += _T("\necho 开始下载更新包......");
	cmdStr += _T("\necho.");
	// 下载指令
	tmpStr.Format(_T("\"%scurl.exe\" -o \"%s%s\" \"%s\""), relativeDir, updateDir, fileName, filesPackUrl);
	cmdStr += _T("\n");
	cmdStr += tmpStr;
	cmdStr += _T("\necho.");
	cmdStr += _T("\necho 下载结束......");
	cmdStr += _T("\necho 开始解压......");
	// 写出解压bat 调用7za进行解压
	tmpStr.Format(_T("\"%s7za.exe\" x \"%s%s\" -o\"%s\" -y"), relativeDir, updateDir, fileName, updateDir);
	cmdStr += _T("\n");
	cmdStr += tmpStr;
	cmdStr += _T("\necho 解压结束......");
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
	WriteString(cmdStr, updateDir + _T("unzip.bat"));
	tmpStr.Format(_T("\"%sunzip.bat\""), updateDir);
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

	// 写更新批处理
	/*if (!WriteUpdateBat(updateFiles, updateDir))
	{
		AfxMessageBox(_T("写更新批处理失败，你可以手动把\"") + updateDir + _T("\"下的所有文件移动到程序目录下完成更新"), MB_ICONERROR);
		goto End;
	}*/

	// 备份
	if (!Backup(updateInfo->m_files, relativeDir))
		AfxMessageBox(_T("创建备份失败，请手动备份！"), MB_ICONERROR);

	//AfxMessageBox(_T("更新文件下载完毕，关闭本程序以完成更新，不要关闭cmd窗口"));

	//// 执行批处理
	//ShellExecute(NULL, _T("open"), updateDir + UPDATE_BAT_NAME, NULL, updateDir, SW_SHOWNORMAL);

	
	// 替换旧文件
	// if (!ReplaceFiles(updateFiles, relativeDir, updateDir))
	if (!ReplaceFiles(updateInfo->m_files, relativeDir, updateDir))
		goto End;

	AfxMessageBox(_T("更新完毕，重启本程序后生效"));


End:
	CoUninitialize();
}

// 检查更新，如果需要更新则开一个线程自动更新
UPDATE_API CheckUpdateResult CheckUpdate(BOOL silent)
{
	// 取更新信息
	std::unique_ptr<BYTE[]> buffer;
	ULONG size;
	CString update_url;
	update_url.Format(UPDATE_INFO_URL, UPDATE_CURRENT_VERSION_NUM);
	if (HTTPGetRaw(update_url, &buffer, &size) != NET_SUCCESS) { // 用HTTPGetRaw防止转码
		return UPDATE_FAILED_TO_GET_INFO;
	} 
	auto updateInfo = std::make_unique<CUpdateInfo>();
	if (!updateInfo->LoadFromString((LPCSTR)buffer.get(), size))
		return UPDATE_FAILED_TO_GET_INFO;

	// 无更新
	if (updateInfo->m_version_num <= UPDATE_CURRENT_VERSION_NUM)
		return UPDATE_NO_UPDATE;

	if (silent) {
		// 静默拉取信息，然后在主界面提示
		return UPDATE_HAS_UPDATE;
	}
	else {
		// 用户取消更新
		if (AfxMessageBox(_T("最新版本") + updateInfo->m_version + _T("\r\n\r\n") + updateInfo->m_updateLog
			+ _T("\r\n\r\n是否更新？"), MB_ICONQUESTION | MB_YESNO) == IDNO)
			return UPDATE_HAS_UPDATE;

		// 在线程中更新
		std::thread(UpdateThread, updateInfo.release()).detach();
		//ManuallyUpdate();
		return UPDATE_NEED_RESTART;
	}
}

// 手动更新，打开一个URL
UPDATE_API void ManuallyUpdate()
{
	ShellExecute(NULL, _T("open"), QQ_QUN_UPDATE_URL, NULL, NULL, SW_SHOWNORMAL);
}

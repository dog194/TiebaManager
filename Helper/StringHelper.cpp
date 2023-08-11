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

#include "stdafx.h"
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <ConfigFile.h>
#include <document.h>
#import <msscript.ocx> no_namespace
using namespace tinyxml2;
using namespace rapidjson;

void RegexText::Set(const CString& _text, BOOL _isRegex, BOOL _ignoreCase)
{
	isRegex = _isRegex;
	ignoreCase = _ignoreCase;
	text = _text;
	textLower = text;
	textLower.MakeLower();
	try
	{
		regexp.assign(isRegex ? text : _T(""), std::regex_constants::syntax_option_type(std::wregex::ECMAScript
			| (ignoreCase ? std::wregex::icase : 0)));
	}
	catch (std::regex_error& e)
	{
		Set(_T(""), FALSE, FALSE);
		AfxMessageBox(_T("正则表达式错误：") + _text + _T("\r\n") + e.what(), MB_ICONERROR);
	}
}

HELPER_API DECLEAR_READ(RegexText)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<BOOL> isRegex("IsRegex");
	COption<BOOL> ignoreCase("IgnoreCase");
	COption<CString> text("Text");
	isRegex.Read(*optionNode);
	ignoreCase.Read(*optionNode);
	text.Read(*optionNode);

	m_value.Set(text, isRegex, ignoreCase);

	if (!IsValid(m_value))
		UseDefault();
}

HELPER_API DECLEAR_WRITE(RegexText)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<BOOL> isRegex("IsRegex");
	*isRegex = m_value.isRegex;
	isRegex.Write(*optionNode);
	COption<BOOL> ignoreCase("IgnoreCase");
	*ignoreCase = m_value.ignoreCase;
	ignoreCase.Write(*optionNode);
	COption<CString> text("Text");
	*text = m_value.text;
	text.Write(*optionNode);
}


// 分割字符串
HELPER_API void SplitString(CStringArray& dst, const CString& src, const CString& slipt)
{
	dst.RemoveAll();
	const int len = _tcslen(slipt);

	int start = 0, end = 0;
	while ((end = src.Find(slipt, end)) != -1)
	{
		dst.Add(src.Mid(start, end - start));
		start = end += len;
	}
	dst.Add(src.Right(src.GetLength() - start));
}


// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const CString& content, BOOL isRegex, BOOL ignoreCase)
{
	if (isRegex)
	{
		try
		{
			std::wregex reg(content, std::regex_constants::syntax_option_type(std::wregex::ECMAScript
				| (ignoreCase ? std::wregex::icase : 0)));
			return std::regex_search((LPCWSTR)str, reg);
		}
		catch (std::regex_error& e)
		{
			AfxMessageBox(_T("正则表达式错误：") + content + _T("\r\n") + e.what(), MB_ICONERROR);
			return FALSE;
		}
	}
	else
	{
		if (ignoreCase)
		{
			CString strLower = str, contentLower = content;
			strLower.MakeLower();
			contentLower.MakeLower();
			return StringIncludes(strLower, contentLower);
		}
		else
			return StringIncludes(str, content);
	}
}

// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const RegexText& content, int* _pos, int* length)
{
	BOOL result;
	if (content.isRegex)
	{
		std::wcmatch res;
		try
		{
			result = std::regex_search((LPCWSTR)str, res, content.regexp);
		}
		catch (std::regex_error& e)
		{
			AfxMessageBox(_T("正则表达式错误：") + content.text + _T("\r\n") + e.what(), MB_ICONERROR);
			return FALSE;
		}

		if (result && _pos != NULL && length != NULL)
		{
			*_pos = res.position();
			*length = res.length();
		}
	}
	else
	{
		int pos;
		if (content.ignoreCase)
		{
			CString strLower = str;
			strLower.MakeLower();
			pos = strLower.Find(content.textLower);
		}
		else
			pos = str.Find(content.text);
		result = pos != -1;
		if (result && _pos != NULL && length != NULL)
		{
			*_pos = pos;
			*length = content.text.GetLength();
		}
	}
	return result;
}

// 字符串匹配
HELPER_API BOOL StringMatchs(const CString& str, const CString& content, BOOL isRegex, BOOL ignoreCase)
{
	if (isRegex)
	{
		try
		{
			std::wregex reg(content, std::regex_constants::syntax_option_type(std::wregex::ECMAScript
				| (ignoreCase ? std::wregex::icase : 0)));
			return std::regex_match((LPCWSTR)str, reg);
		}
		catch (std::regex_error& e)
		{
			AfxMessageBox(_T("正则表达式错误：") + content + _T("\r\n") + e.what(), MB_ICONERROR);
			return FALSE;
		}
	}
	else
		return ignoreCase ? str.CompareNoCase(content) == 0 : str == content;
}

// 字符串匹配
HELPER_API BOOL StringMatchs(const CString& str, const RegexText& content)
{
	if (content.isRegex)
	{
		try
		{
			return std::regex_match((LPCWSTR)str, content.regexp);
		}
		catch (std::regex_error& e)
		{
			AfxMessageBox(_T("正则表达式错误：") + content.text + _T("\r\n") + e.what(), MB_ICONERROR);
			return FALSE;
		}
	}
	else
		return content.ignoreCase ? str.CompareNoCase(content.text) == 0 : str == content.text;
}

// 取字符串之间的字符串
HELPER_API CString GetStringBetween(const CString& src, const CString& left, const CString& right, int startPos)
{
	int leftPos = src.Find(left, startPos);
	if (leftPos == -1)
		return _T("");
	leftPos += left.GetLength();
	int rightPos = src.Find(right, leftPos);
	if (rightPos == -1)
		return _T("");
	return src.Mid(leftPos, rightPos - leftPos);
}

// 取字符串之间的字符串，包括左右的字符串
HELPER_API CString GetStringBetween2(const CString& src, const CString& left, const CString& right, int startPos)
{
	int leftPos = src.Find(left, startPos);
	if (leftPos == -1)
		return _T("");
	int rightPos = src.Find(right, leftPos + left.GetLength());
	if (rightPos == -1)
		return _T("");
	rightPos += right.GetLength();
	return src.Mid(leftPos, rightPos - leftPos);
}

// 取字符串之前的字符串
HELPER_API CString GetStringBefore(const CString& src, const CString& right, int startPos)
{
	int rightPos = src.Find(right, startPos);
	if (rightPos == -1)
		return _T("");
	return src.Left(rightPos);
}

// 取字符串之后的字符串
HELPER_API CString GetStringAfter(const CString& src, const CString& left, int startPos)
{
	int leftPos = src.Find(left, startPos);
	if (leftPos == -1)
		return _T("");
	leftPos += left.GetLength();
	return src.Right(src.GetLength() - leftPos);
}

// 写字符串到文件
HELPER_API BOOL WriteString(const CString& src, const CString& path)
{
	CStdioFile file;
	if (!file.Open(path, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
	file.WriteString(src);
	return TRUE;
}

// 写字符串到文件 续写
HELPER_API BOOL WriteStringCon(const CString& src, const CString& path)
{
	CStdioFile file;
	if (!file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
		return FALSE;
	if (file.GetLength() != 0) {
		file.SeekToEnd();
		file.WriteString(_T("\r\n"));
	}
	file.WriteString(src);
	return TRUE;
}

// 单字符16进制转10进制
HELPER_API int HexChar2Dec(const CCHAR c) {
	if ('0' <= c && c <= '9') {
		return (c - '0');
	}
	else if ('a' <= c && c <= 'f') {
		return (c - 'a' + 10);
	}
	else if ('A' <= c && c <= 'F') {
		return (c - 'A' + 10);
	}
	return -1;
}

// Unicode(UCS-2)转ANSI
HELPER_API CStringA W2ANSI(const CStringW& src, UINT codePage)
{
	int dstLen = WideCharToMultiByte(codePage, 0, src, src.GetLength(), NULL, 0, NULL, NULL);
	if (dstLen == 0)
		return "";
	CStringA res;
	WideCharToMultiByte(codePage, 0, src, src.GetLength(), res.GetBuffer(dstLen), dstLen, NULL, NULL);
	res.ReleaseBuffer(dstLen);
	return res;
}

// ANSI转Unicode(UCS-2)
HELPER_API CStringW ANSI2W(const CStringA& src, UINT codePage)
{
	int dstLen = MultiByteToWideChar(codePage, 0, src, src.GetLength(), NULL, 0);
	if (dstLen == 0)
		return L"";
	CStringW res;
	MultiByteToWideChar(codePage, 0, src, src.GetLength(), res.GetBuffer(dstLen), dstLen);
	res.ReleaseBuffer(dstLen);
	return res;
}

static CString EncodeURIBase(LPCTSTR functionName, const CString& src)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return _T("");
	script->put_Language(_bstr_t(_T("JScript")));
	_variant_t param = src;
	SAFEARRAY* params = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	LONG index = 0;
	if (FAILED(SafeArrayPutElement(params, &index, &param)))
		return _T("");
	_variant_t result;
	script->raw_Run(_bstr_t(functionName), &params, result.GetAddress());
	SafeArrayDestroy(params);
	return (LPCTSTR)(_bstr_t)result;
}

// URL编码
HELPER_API CString EncodeURI(const CString& src)
{
	return EncodeURIBase(_T("encodeURIComponent"), src);
}

// URL编码，不替换":/%=+"等字符
HELPER_API CString EncodeFullURI(const CString& src)
{
	CString result = EncodeURIBase(_T("encodeURI"), src);
	result.Replace(_T("%25"), _T("%"));
	return result;
}

// URL编码 GBK版
HELPER_API CString EncodeURI_GBK(const CString& _src)
{
	CString result, tmp;
	CStringA src = W2GBK(_src);
	const int len = src.GetLength();
	for (int i = 0; i < len; i++)
	{
		tmp.Format(_T("%%%02X"), src[i] & 0xFF);
		result += tmp;
	}
	return result;
}

// URL解码
HELPER_API CString DncodeURI(const CString& src)
{
	return EncodeURIBase(_T("decodeURIComponent"), src);
}

// URL解码 GBK版
HELPER_API CString DncodeURI_GBK(const CString& src)
{
	CStringA result;
	const int len = src.GetLength();
	for (int i = 0; i < len; i++) {
		char c = src[i];
		if (c != '%') {
			result += c;
		}
		else {
			char c1 = src[++i];
			char c0 = src[++i];
			int num = 0;
			num += HexChar2Dec(c1) * 16 + HexChar2Dec(c0);
			result += char(num);
		}
	}
	return GBK2W(result);
}

// HTML转义
HELPER_API CString HTMLEscape(const CString& src)
{
	CString result = src;
	result.Replace(_T("&"), _T("&amp;"));
	result.Replace(_T(" "), _T("&nbsp;"));
	result.Replace(_T("<"), _T("&lt;"));
	result.Replace(_T(">"), _T("&gt;"));
	return result;
}

// HTML反转义
HELPER_API CString HTMLUnescape(const CString& src)
{
	CString result = src;
	//result.Replace(_T("<br>"), _T("\r\n")); // 不转换行符
	result.Replace(_T("&nbsp;"), _T(" "));
	result.Replace(_T("&quot;"), _T("\""));
	result.Replace(_T("&&#039;"), _T("'"));
	result.Replace(_T("&lt;"), _T("<"));
	result.Replace(_T("&gt;"), _T(">"));
	result.Replace(_T("&amp;"), _T("&"));
	return result;
}

// totalComment API 清理多余 HTML
HELPER_API CString HTMLDelete4totalComment(const CString& src)
{
	CString result = src;
	// <a> 常规 a 标签
	result.Replace(_T(" rel=\"noopener noreferrer nofollow\"  class=\"j - no - opener - url\" "), _T(""));
	// 回复 xx 标签
	int left = result.Find(_T("回复 <a"));
	if (left == 0) {
		CString tmp = GetStringBetween2(result, _T("回复 <a"), _T(">"));
		result.Replace(tmp, _T("回复 "));
		result.Replace(_T("</a> :"), _T(" :"));
	}
	// @ 标签
	left = result.Find(_T("<a href=\"\"  onclick"));
	while (left != -1)
	{
		CString tmp = GetStringBetween2(result, _T("<a href=\"\"  onclick"), _T("hideattip(this)\" "));
		result.Replace(tmp, _T("<a href=\"\" "));
		result.Replace(_T("target=\"_blank\" class=\"at\""), _T("class=\"at\""));
		left = result.Find(_T("<a href=\"\"  onclick"));
	}
	// 贴吧帖子内部跳转 标签
	left = result.Find(_T("<a href="));
	if (left != -1) {
		CString tmp = GetStringBetween2(result, _T("<a href="), _T(">"));
		result.Replace(tmp, _T("<a>"));
	}
	return result;
}

// JS反转义，调用者应自行转义src里的双引号
HELPER_API CString JSUnescape(const CString& src)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return _T("");
	script->put_Language(_bstr_t(_T("JScript")));
	_variant_t result;
	if (FAILED(script->raw_Eval(_bstr_t((LPCTSTR)(_T("\"") + src + _T("\""))), result.GetAddress())))
		return _T("");
	return (LPCTSTR)(_bstr_t)result;
}

// 取Portrait，解决Portrait url历史遗留问题
HELPER_API CString GetPortraitFromString(const CString& src)
{
	//由于历史代码问题，获取的Portrait有以下两种形式。也不知道当初为啥会加这堆前面的url。。
	//http://tb.himg.baidu.com/sys/portrait/item/xxxxxxxxxxxxxxx?t=zzzzzzzzzz
	//http://tb.himg.baidu.com/sys/portrait/item/xxxxxxxxxxxxxxx
	//稳妥，还是TM的稳妥，多写点判断而不是直接改加url的代码。。。应对所有3种情况
	//xxxxxxxxxxxxxxx
	CString tmp;
	tmp = GetStringBetween(src, AUTHOR_PORTRAIT_LEFT, AUTHOR_PORTRAIT_RIGHT);
	if (tmp == _T("")) {
		tmp = GetStringAfter(src, AUTHOR_PORTRAIT_LEFT);
		if (tmp == _T("")) {
			if (tmp.Find(AUTHOR_PORTRAIT_LEFT) == -1) {
				tmp = src;
			}
		}
	}
	return tmp;
}

// 取UName，使用Portrait获取
HELPER_API CString GetNameUsingPortrait(const CString& pPortrait) {
	if (pPortrait.GetLength() < PORT_LEN_MIN) {
		return GET_NAME_ERROR_SHORT;
	}
	CString src = HTTPGet(_T("https://tieba.baidu.com/home/get/panel?ie=utf-8&id=") + pPortrait);
	if (src == NET_TIMEOUT_TEXT)
		return GET_NAME_ERROR_TIME_OUT;
	CString code = GetStringBetween(src, _T("no\":"), _T(","));
	if (code == _T(""))
		code = GetStringBetween(src, _T("code\":\""), _T("\""));
	if (code == _T("1130032"))
		return GET_NAME_ERROR_UID_BAN;
	if (code != _T("0")) {
		return GET_NAME_ERROR_INPUT_ERROR;
	}
	GenericDocument<UTF16<> > document;
	document.Parse(src);
	if (document.HasParseError() || !document.IsObject())
		return GET_NAME_ERROR_FORMAT_ERROR;

	CString u_id = document[L"data"][L"name"].GetString();
	CString u_showNickName = document[L"data"][L"show_nickname"].GetString();
	if (u_id == _T("")) {
		return u_showNickName;
	}
	else if (u_id == u_showNickName){
		return u_showNickName;
	}
	else {
		return u_showNickName + _T(" - ") + u_id;
	}
}

// time_t to string x年x月x日-xx:xx:xx
HELPER_API CString GetYYMMDD_HHMMSS_FromTimeT(const time_t &src)
{
	struct tm time_;
	CString tmp;
	if (src == NULL) {
		time_t t;
		time(&t);
		localtime_s(&time_, &t);
	}
	else {
		localtime_s(&time_, &src);
	}
	tmp.Format(_T("%02d-%02d-%02d %02d:%02d:%02d"), time_.tm_year + 1900, time_.tm_mon + 1, time_.tm_mday, time_.tm_hour, time_.tm_min, time_.tm_sec);
	return tmp;
}

// time_t to string x年x月x日
HELPER_API CString GetYYMMDD_FromTimeT(const time_t &src)
{
	struct tm time_;
	CString tmp;
	if (src == NULL) {
		time_t t;
		time(&t);
		localtime_s(&time_, &t);
	}
	else {
		localtime_s(&time_, &src);
	}
	tmp.Format(_T("%02d-%02d-%02d"), time_.tm_year + 1900, time_.tm_mon + 1, time_.tm_mday);
	return tmp;
}

// 获取时间戳毫秒
HELPER_API LONGLONG GetTimestampMS()
{
	// TODO 修复，目前只是秒数 * 1000
	time_t t;
	time(&t);
	time_t tMS = t * 1000;
	return tMS;
}

// Int to CString
HELPER_API CString Int2CString(const int num)
{
	CString tmp;
	tmp.Format(_T("%d"), num);
	return tmp;
}

// Int64 to CString
HELPER_API CString Int64oCString(const INT64 num)
{
	CString tmp;
	tmp.Format(_T("%I64d"), num);
	return tmp;
}

// 获取随机Tip
HELPER_API CString GetRandomTip() {
	CString tmp;
	std::srand(time(NULL));
	int index = std::rand() % m_tips_num;
	tmp.Format(_T("[%d/%d]%s"), index + 1, m_tips_num, m_tips[index]);
	return tmp;
}

//  pName可取值：
//  _T("CompanyName"),_T("FileDescription"),_T("FileVersion"),
//  _T("InternalName"),_T("LegalCopyright"),_T("OriginalFilename"),
//  _T("ProductName"),_T("ProductVersion"),
HELPER_API CString GetFileVersionString(LPCTSTR pFileName, LPCTSTR pName /* = NULL */)
{
	TCHAR* ptBuf = NULL;
	ptBuf = new TCHAR[1024];
	UINT lenBuf = 1024;

	DWORD   dwDummyHandle = 0; // will always be set to zero
	DWORD   dwLen = 0;
	BYTE* pVersionInfo = NULL;
	BOOL    bRetVal;

	VS_FIXEDFILEINFO    FileVersion;

	HMODULE        hVerDll;
	hVerDll = LoadLibrary(_T("VERSION.dll"));
	if (hVerDll == NULL)
		return _T("");

#ifdef _UNICODE
	typedef DWORD(WINAPI* Fun_GetFileVersionInfoSize)(LPCTSTR, DWORD*);
	typedef BOOL(WINAPI* Fun_GetFileVersionInfo)(LPCTSTR, DWORD, DWORD, LPVOID);
	typedef BOOL(WINAPI* Fun_VerQueryValue)(LPCVOID, LPCTSTR, LPVOID, PUINT);
#else
	typedef DWORD(WINAPI* Fun_GetFileVersionInfoSize)(LPCSTR, DWORD*);
	typedef BOOL(WINAPI* Fun_GetFileVersionInfo)(LPCSTR, DWORD, DWORD, LPVOID);
	typedef BOOL(WINAPI* Fun_VerQueryValue)(LPCVOID, LPCSTR, LPVOID, PUINT);
#endif
	Fun_GetFileVersionInfoSize        pGetFileVersionInfoSize;
	Fun_GetFileVersionInfo            pGetFileVersionInfo;
	Fun_VerQueryValue                pVerQueryValue;
#ifdef _UNICODE
	pGetFileVersionInfoSize = (Fun_GetFileVersionInfoSize)::GetProcAddress(hVerDll, "GetFileVersionInfoSizeW");
	pGetFileVersionInfo = (Fun_GetFileVersionInfo)::GetProcAddress(hVerDll, "GetFileVersionInfoW");
	pVerQueryValue = (Fun_VerQueryValue)::GetProcAddress(hVerDll, "VerQueryValueW");
#else
	pGetFileVersionInfoSize = (Fun_GetFileVersionInfoSize)::GetProcAddress(hVerDll, "GetFileVersionInfoSizeA");
	pGetFileVersionInfo = (Fun_GetFileVersionInfo)::GetProcAddress(hVerDll, "GetFileVersionInfoA");
	pVerQueryValue = (Fun_VerQueryValue)::GetProcAddress(hVerDll, "VerQueryValueA");
#endif

	struct TRANSLATION {
		WORD langID;            // language ID
		WORD charset;            // character set (code page)
	} Translation;

	Translation.langID = 0x0409;    //
	Translation.charset = 1252;        // default = ANSI code page

	dwLen = pGetFileVersionInfoSize(pFileName, &dwDummyHandle);
	if (dwLen == 0)
	{
		bRetVal = FALSE;
		goto End;
	}

	pVersionInfo = new BYTE[dwLen]; // allocate version info
	bRetVal = pGetFileVersionInfo(pFileName, 0, dwLen, pVersionInfo);
	if (bRetVal == FALSE)
	{
		goto End;
	}

	VOID* pVI;
	UINT    uLen;

	bRetVal = pVerQueryValue(pVersionInfo, _T("\\"), &pVI, &uLen);
	if (bRetVal == FALSE)
	{
		goto End;
	}

	memcpy(&FileVersion, pVI, sizeof(VS_FIXEDFILEINFO));

	bRetVal = pVerQueryValue(pVersionInfo, _T("\\VarFileInfo\\Translation"),
		&pVI, &uLen);
	if (bRetVal && uLen >= 4)
	{
		memcpy(&Translation, pVI, sizeof(TRANSLATION));
	}
	else
	{
		bRetVal = FALSE;
		goto End;
	}

	//  BREAKIF(FileVersion.dwSignature != VS_FFI_SIGNATURE);
	if (FileVersion.dwSignature != VS_FFI_SIGNATURE)
	{
		bRetVal = FALSE;
		goto End;
	}

	VOID* pVal;
	UINT        iLenVal;

	if (pName == NULL)
	{
		_stprintf_s(ptBuf, lenBuf, _T("%d.%d.%d.%d"),
			HIWORD(FileVersion.dwFileVersionMS), LOWORD(FileVersion.dwFileVersionMS),
			HIWORD(FileVersion.dwFileVersionLS), LOWORD(FileVersion.dwFileVersionLS));
	}
	else
	{
		TCHAR    szQuery[1024];
		_stprintf_s(szQuery, 1024, _T("\\StringFileInfo\\%04X%04X\\%s"),
			Translation.langID, Translation.charset, pName);

		bRetVal = pVerQueryValue(pVersionInfo, szQuery, &pVal, &iLenVal);
		if (bRetVal)
		{
			_stprintf_s(ptBuf, lenBuf, _T("%s"), (TCHAR*)pVal);
		}
		else
		{
			_stprintf_s(ptBuf, lenBuf, _T("%s"), _T(""));
		}
	}

End:
	FreeLibrary(hVerDll);
	hVerDll = NULL;
	delete[] pVersionInfo;
	pVersionInfo = NULL;
	CString str;
	str.Format(_T("%s"), ptBuf);

	delete ptBuf;
	ptBuf = NULL;
	return str;
}

// 比较版本值，返回更高的版本
HELPER_API CString GetHigherVersionString(const CString pVersionA, const CString pVersionB) {
	CStringArray argsA, argsB;
	SplitString(argsA, pVersionA, _T("."));
	SplitString(argsB, pVersionB, _T("."));
	if (argsA.GetSize() != 4 || argsB.GetSize() != 4) {
		// 参数有问题直接返回空。规范的版本号是  a.b.c.d
		return _T("");
	}
	if (argsA[0] == argsB[0]) {
		if (argsA[1] == argsB[1]) {
			if (argsA[2] == argsB[2]) {
				if (_ttoi(argsA[3]) > _ttoi(argsB[3])) {
					return pVersionA;
				}
				else {
					return pVersionB;
				}
			}
			else if (_ttoi(argsA[2]) > _ttoi(argsB[2])) {
				return pVersionA;
			}
			else {
				return pVersionB;
			}
		}
		else if (_ttoi(argsA[1]) > _ttoi(argsB[1])) {
			return pVersionA;
		}
		else {
			return pVersionB;
		}
	}
	else if (_ttoi(argsA[0]) > _ttoi(argsB[0])) {
		return pVersionA;
	}
	else {
		return pVersionB;
	}
}
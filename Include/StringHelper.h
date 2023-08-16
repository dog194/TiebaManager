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

#pragma once
#include "HelperCommon.h"
#include <regex>
#include <time.h>


class HELPER_API RegexText
{
public:
	BOOL isRegex = FALSE;
	BOOL ignoreCase = FALSE;
	CString text; // 为了避免忘记设置regexp要用Set方法赋值！！
	std::wregex regexp;
	CString textLower;

	void Set(const CString& _text, BOOL _isRegex = FALSE, BOOL _ignoreCase = FALSE);
};

const TCHAR AUTHOR_PORTRAIT_LEFT[] = _T(R"(/portrait/item/)");
const TCHAR AUTHOR_PORTRAIT_RIGHT[] = _T("?t=");
const TCHAR AUTHOR_PORTRAIT_URL_PREFIX[] = _T("http://tb.himg.baidu.com/sys/portrait/item/");

const int m_tips_num = 8;
const CString m_tips[m_tips_num] = {
	_T("更新群，交流群，736700862(没被封的话)"),
	_T("不喜欢复杂的确认界面，可以在设置里面关闭启用Pro窗口"),
	_T("扫描间隔，删帖封禁间隔都不能设置太短，会出验证码"),
	_T("确认窗口点击规则名，可以快捷打开对应规则修改"),
	_T("黑名单/循环封，头像ID输入用户主页网址可以自动转换哦"),
	_T("修改方案名，主窗口标题会跟着变哦！"),
	_T("封禁需谨慎"),
	_T("检查更新设置在 关于&更新 页面中")
};

const int PORT_LEN_MAX = 36;
const int PORT_LEN_MIN = 33;

const CString GET_NAME_ERROR_SHORT = _T("[长度不足]");
const CString GET_NAME_ERROR_UID_BAN = _T("[用户封禁]");
const CString GET_NAME_ERROR_TIME_OUT = _T("[访问超时]");
const CString GET_NAME_ERROR_INPUT_ERROR = _T("[参数错误]");
const CString GET_NAME_ERROR_FORMAT_ERROR = _T("[格式异常]");

const CString NOT_FOUND = _T("[未找到]");
const CString QQ_QUN_URL = _T("https://qm.qq.com/cgi-bin/qm/qr?k=ccjdz1q_9-VDat0FeKU9ov9nWrZD3naD");

// 分割字符串
HELPER_API void SplitString(CStringArray& dst, const CString& src, const CString& slipt);

// 字符串包含
inline BOOL StringIncludes(const CString& str, const CString& content) { return str.Find(content) != -1; }
// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const CString& content, BOOL isRegex, BOOL ignoreCase);
// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const RegexText& content, int* _pos = NULL, int* length = NULL);
// 字符串匹配
HELPER_API BOOL StringMatchs(const CString& str, const CString& content, BOOL isRegex, BOOL ignoreCase);
// 字符串匹配
HELPER_API BOOL StringMatchs(const CString& str, const RegexText& content);

// 取字符串之间的字符串
HELPER_API CString GetStringBetween(const CString& src, const CString& left, const CString& right, int startPos = 0);
// 取字符串之间的字符串，包括左右的字符串
HELPER_API CString GetStringBetween2(const CString& src, const CString& left, const CString& right, int startPos = 0);
// 取字符串之前的字符串
HELPER_API CString GetStringBefore(const CString& src, const CString& right, int startPos = 0);
// 取字符串之后的字符串
HELPER_API CString GetStringAfter(const CString& src, const CString& left, int startPos = 0);

// 写字符串到文件
HELPER_API BOOL WriteString(const CString& src, const CString& path);
// 写字符串到文件 续写
HELPER_API BOOL WriteStringCon(const CString& src, const CString& path);
// 单字符16进制转10进制
HELPER_API int HexChar2Dec(const CCHAR c);
// Unicode(UCS-2)转ANSI
HELPER_API CStringA W2ANSI(const CStringW& src, UINT codePage);
// ANSI转Unicode(UCS-2)
HELPER_API CStringW ANSI2W(const CStringA& src, UINT codePage);
// Unicode(UCS-2)转UTF-8
inline CStringA W2UTF8(const CStringW& src) { return W2ANSI(src, CP_UTF8); }
// UTF-8转Unicode(UCS-2)
inline CStringW UTF82W(const CStringA& src) { return ANSI2W(src, CP_UTF8); }
// UTF-8转Unicode(UCS-2)
inline CStringW strUTF82W(const std::string& src) { CStringA srcA(src.c_str(), src.size()); return ANSI2W(srcA, CP_UTF8); }
// Unicode(UCS-2)转GBK
inline CStringA W2GBK(const CStringW& src) { return W2ANSI(src, 936); }
// GBK转Unicode(UCS-2)
inline CStringW GBK2W(const CStringA& src) { return ANSI2W(src, 936); }
// URL编码
HELPER_API CString EncodeURI(const CString& src);
// URL编码，不替换":/%=+"等字符
HELPER_API CString EncodeFullURI(const CString& src);
// URL编码 GBK版
HELPER_API CString EncodeURI_GBK(const CString& src);
// URL解码
HELPER_API CString DncodeURI(const CString& src);
// URL解码 GBK版
HELPER_API CString DncodeURI_GBK(const CString& src);
// HTML转义
HELPER_API CString HTMLEscape(const CString& src);
// HTML反转义
HELPER_API CString HTMLUnescape(const CString& src);
// totalComment API 清理多余 HTML
HELPER_API CString HTMLDelete4totalComment(const CString& src);
// JS反转义，调用者应自行转义src里的双引号
HELPER_API CString JSUnescape(const CString& src);

// 取Portrait，解决Portrait url历史遗留问题
HELPER_API CString GetPortraitFromString(const CString& src);
// 取UName，使用Portrait获取
HELPER_API CString GetNameUsingPortrait(const CString& pPortrait);
// time_t to string x年x月x日-xx:xx:xx
HELPER_API CString GetYYMMDD_HHMMSS_FromTimeT(const time_t &src = NULL);
// time_t to string x年x月x日
HELPER_API CString GetYYMMDD_FromTimeT(const time_t &src = NULL);
// 获取时间戳毫秒
HELPER_API LONGLONG GetTimestampMS();
// 比较时间，返回秒数差值，排除日期, 正值，t1 大于 t2, t1 比 t2 晚
HELPER_API int GetTimeDiffInS(const time_t& time_1, const time_t& time_2);

// Int to CString
HELPER_API CString Int2CString(const int num);
// Int64 to CString
HELPER_API CString Int64oCString(const INT64 num);
// 获取随机Tip
HELPER_API CString GetRandomTip();
// 获取DLL文件信息
HELPER_API CString GetFileVersionString(LPCTSTR pFileName, LPCTSTR pName /* = NULL */);
// 比较版本值，返回更高的版本
HELPER_API CString GetHigherVersionString(const CString pVersionA, const CString pVersionB);
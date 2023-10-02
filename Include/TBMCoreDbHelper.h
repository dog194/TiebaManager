/*
Copyright (C) 2011-2023  xfgryujk Dog194
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
#include "string"
#include "map"

#include "TiebaManagerCoreCommon.h"
#include "Singleton.h"

#include "sqlite3.h"

const std::string TABLE_NAME_IMG_INFO = "ImgInfo";
const std::string C_NAME_IG_ID = "imgID";
const std::string C_NAME_IG_QR = "QR";
const std::string C_NAME_IG_HEAD = "imgHead";
const std::string C_NAME_IG_OCR = "OCR";
const std::string C_NAME_IG_TIME = "lastUpdate";

class CDbImgInfo {
public:
	CString m_id;			// img Name
	CString m_QR;			// QR code Scan
	BOOL is_QR_null;		// 用于区分 NULL 和 _T("")
	CString m_imgHead;		// img file Head
	CString m_OCR;			// OCR result
	BOOL is_OCR_null;		// 用于区分 NULL 和 _T("")
	time_t m_time;			// 记录最后访问时间，用于清理
	
	CDbImgInfo(const CString& cName);
	CDbImgInfo(const CString& cName, const std::string& cKey, const std::string& cValue);
};

class TBM_CORE_API CSqlDb final : public Singleton<CSqlDb>
{
	DECL_SINGLETON(CSqlDb);
public:
	CSqlDb();
	~CSqlDb();

public:
	static CString DB_PATH;
	BOOL m_dbInit = FALSE;
	sqlite3* m_db;

public:
	BOOL initTable();
	BOOL insert(const std::string& tbName, const std::string& sqlMain);
	BOOL insert2imgInfo(const CDbImgInfo& imgInfo);
	CDbImgInfo getImgInfo(const CString& cName);
	int db_delete(const std::string& tbName, const std::string& sqlConditions);
	int db_deleteImgInfo(const int& nDays = 30);
};

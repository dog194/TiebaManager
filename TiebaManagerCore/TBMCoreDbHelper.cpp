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

#include "stdafx.h"
#include <TBMCoreDbHelper.h>
#include <StringHelper.h>

CString CSqlDb::DB_PATH = _T("\\db");


CDbImgInfo::CDbImgInfo(const CString& cName) {
	m_id = cName;
	m_QR = _T("");
	is_QR_null = TRUE;
	m_imgHead = _T("");
	m_OCR = _T("");
	is_OCR_null = TRUE;
	time(&m_time);
}


CDbImgInfo::CDbImgInfo(const CString& cName, const std::string& cKey, const std::string& cValue) {
	m_id = cName;
	m_QR = _T("");
	is_QR_null = TRUE;
	m_imgHead = _T("");
	m_OCR = _T("");
	is_OCR_null = TRUE;
	time(&m_time);
	if (cKey == C_NAME_IG_QR) {
		m_QR = cValue.c_str();
		is_QR_null = FALSE;
	}
	else if (cKey == C_NAME_IG_HEAD) {
		m_imgHead = cValue.c_str();
	}
	else if (cKey == C_NAME_IG_OCR) {
		m_OCR = cValue.c_str();
		is_OCR_null = FALSE;
	}
	else {
		// 错误
		CString c_cKey, c_cValue;
		c_cKey = cKey.c_str();
		c_cValue = cValue.c_str();
		DebugRecord(_T("CDbImgInfo 初始化 key 异常"), c_cKey + L" " + c_cValue);
	}
}

// 创建表
BOOL CSqlDb::initTable() {
	/* 创建表格 ImgInfo ========================================================
	CREATE TABLE "ImgInfo" (
	"imgID"	TEXT NOT NULL UNIQUE,
	"QR"	TEXT,
	"imgHead"	TEXT,
	"OCR"	TEXT,
	"lastUpdate"	INTEGER NOT NULL,
	PRIMARY KEY("imgID")
	); 
	*/
	std::string sql = \
		"CREATE TABLE IF NOT EXISTS " +	TABLE_NAME_IMG_INFO + " (" + \
		C_NAME_IG_ID + " TEXT NOT NULL UNIQUE," + \
		C_NAME_IG_QR + " TEXT," + \
		C_NAME_IG_HEAD + " TEXT," + \
		C_NAME_IG_OCR + " TEXT," + \
		C_NAME_IG_TIME + " INTEGER NOT NULL," \
		"PRIMARY KEY(" + C_NAME_IG_ID + ")" \
		")";
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		AfxMessageBox(_T("表创建失败"), MB_ICONINFORMATION | MB_TOPMOST);
		sqlite3_free(errMsg);
		return FALSE;
	}

	// TODO 表格校验
	return TRUE;
}

// 插入或者更新数据
BOOL CSqlDb::insert(const std::string& tbName, const std::string& sqlMain) {
	if (m_dbInit == FALSE) {
		CString tmp, tmp2;
		tmp = tbName.c_str();
		tmp2 = sqlMain.c_str();
		DebugRecord(_T("未初始化，却访问 insert"), tmp + _T(" ") + tmp2);
		return FALSE;
	}
	// "INSERT OR REPLACE INTO table1 (column1, column2) VALUES (?, ?)"
	std::string sql = "INSERT OR REPLACE INTO " + tbName + " " + sqlMain;
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		CString tmp(errMsg);
		CString tmp2, tmp3;
		tmp2 = tbName.c_str();
		tmp3 = sqlMain.c_str();
		DebugRecord(_T("插入数据失败"), tmp + _T(" ") + tmp2 + _T(" ") + tmp3);
		sqlite3_free(errMsg);
		return FALSE;
	}
	return TRUE;
}


BOOL CSqlDb::insert2imgInfo(const CDbImgInfo& imgInfo) {
	std::string sql_col;
	std::string sql_val;
	sql_col = C_NAME_IG_ID;
	sql_val = "'" + W2UTF8(imgInfo.m_id) + "'";
	if (imgInfo.is_QR_null == FALSE || imgInfo.m_QR != _T("")) {
		sql_col += ", " + C_NAME_IG_QR;
		sql_val += ", '" + W2UTF8(imgInfo.m_QR) + "'";
	}
	if (imgInfo.m_imgHead != _T("")) {
		sql_col += ", " + C_NAME_IG_HEAD;
		sql_val += ", '" + W2UTF8(imgInfo.m_imgHead) + "'";
	}
	if (imgInfo.is_OCR_null == FALSE || imgInfo.m_OCR != _T("")) {
		sql_col += ", " + C_NAME_IG_OCR;
		sql_val += ", '" + W2UTF8(imgInfo.m_OCR) + "'";
	}
	sql_col += ", " + C_NAME_IG_TIME;
	sql_val += ", '" + W2UTF8(Int64oCString(GetTimestamp())) + "'";
	std::string sql;
	// "INSERT OR REPLACE INTO table1 (column1, column2) VALUES (?, ?)"
	sql = "(" + sql_col + ") VALUES (" + sql_val + ")";
	return insert(TABLE_NAME_IMG_INFO, sql);
}


CDbImgInfo CSqlDb::getImgInfo(const CString& cName) {
	CDbImgInfo imgInfo = CDbImgInfo(cName);
	if (m_dbInit == FALSE) {
		DebugRecord(_T("未初始化，却访问 getImgInfo"), cName);
		return imgInfo;
	}
	std::string sql = "SELECT " + C_NAME_IG_QR + ", " + C_NAME_IG_HEAD + ", " +
		C_NAME_IG_OCR + ", " + C_NAME_IG_TIME + " FROM " + TABLE_NAME_IMG_INFO +
		" WHERE " + C_NAME_IG_ID + "=?";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	std::string s_name = W2UTF8(cName);
	const char* a_Name = s_name.c_str();
	sqlite3_bind_text(stmt, 1, a_Name, -1, SQLITE_TRANSIENT);
	if (rc != SQLITE_OK) {
		const char* errMsg = sqlite3_errmsg(m_db);
		CString error(errMsg);
		DebugRecord(_T("prepare失败:"), error);
		sqlite3_finalize(stmt);
		return imgInfo;
	}
	while ((sqlite3_step(stmt)) == SQLITE_ROW) {
		if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
			imgInfo.is_QR_null = FALSE;
			CStringA a_qr(sqlite3_column_text(stmt, 0));
			CString c_qr = UTF82W(a_qr);
			imgInfo.m_QR = c_qr;
		}
		CStringA a_head(sqlite3_column_text(stmt, 1));
		CString c_head = UTF82W(a_head);
		imgInfo.m_imgHead = c_head;
		if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
			imgInfo.is_OCR_null = FALSE;
			CStringA a_ocr(sqlite3_column_text(stmt, 2));
			CString c_ocr = UTF82W(a_ocr);
			imgInfo.m_OCR = c_ocr;
		}
		INT64 c_time(sqlite3_column_int64(stmt, 3));
		imgInfo.m_time = c_time;
	}
	if (rc != SQLITE_OK) {
		const char* errMsg = sqlite3_errmsg(m_db);
		CString error(errMsg);
		DebugRecord(_T("查询失败:"), error);
		return imgInfo;
	}
	return imgInfo;
}


int CSqlDb::db_delete(const std::string& tbName, const std::string& sqlConditions) {
	if (m_dbInit == FALSE) {
		DebugRecord(_T("未初始化，却访问 delete"), _T(""));
		return FALSE;
	}
	// "DELETE FROM table_name WHERE column_name = value";
	std::string sql = "DELETE FROM " + tbName + " WHERE " + sqlConditions;
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql.c_str(), NULL, 0, &errMsg);
	if (rc != SQLITE_OK) {
		CString tmp(errMsg);
		CString tmp1, tmp2;
		tmp1 = tbName.c_str();
		tmp2 = sqlConditions.c_str();
		DebugRecord(_T("删除数据失败"), tmp + _T("") + tmp1 + _T("") + tmp2);
		sqlite3_free(errMsg);
		return -1;
	}
	else {
		int changes = sqlite3_changes(m_db);
		return changes;
	}
}


// 按照时间删除，删除超过指定天数之前的数据
int CSqlDb::db_deleteImgInfo(const int& nDays) {
	time_t tNow, tLimit;
	time(&tNow);
	tLimit = tNow - nDays * 24 * 60 * 60;
	std::string sqlConditions = C_NAME_IG_TIME + " < ";
	sqlConditions += CW2A(Int64oCString(tLimit));
	return db_delete(TABLE_NAME_IMG_INFO, sqlConditions);
}

CSqlDb::CSqlDb()
{
	if (m_dbInit == FALSE) {
		int rc = sqlite3_open("tb.db", &m_db);
		if (rc != SQLITE_OK) {
			const char* zErrMsg = sqlite3_errmsg(m_db);
			CString error(zErrMsg);
			AfxMessageBox(_T("初始化失败:") + error, MB_ICONINFORMATION | MB_TOPMOST);
		}
		else {
			m_dbInit = TRUE;
			initTable();
		}
	}
	else {
		AfxMessageBox(_T("早就链接"), MB_ICONINFORMATION | MB_TOPMOST);
	}
}

// 析构函数
CSqlDb::~CSqlDb()
{
	sqlite3_close(m_db);
	m_dbInit = FALSE;
}

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

#pragma once
#include <Singleton.h>
#include <TiebaClawer.h>


class CTBMCoreListeners final : public Singleton<CTBMCoreListeners>
{
	DECL_SINGLETON(CTBMCoreListeners);
private:
	CTBMCoreListeners::CTBMCoreListeners();

	
	// 扫描事件

	template<class TbObj>
	static void OnCheckIllegal(const TbObj& obj, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length, CString& ruleName, int& ruleType);

	static void OnPreScanThread(int threadID, const ThreadInfo& thread, BOOL& pass);
};

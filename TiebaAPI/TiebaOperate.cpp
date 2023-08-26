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
#include <TiebaOperate.h>
#include <TiebaClawer.h>
#include <NetworkHelper.h>
#include <TiebaClientHelper.h>

#include "TiebaProto/ProfileReqIdl.pb.h"
#include "TiebaProto/ProfileResIdl.pb.h"
#include "TiebaProto/User.pb.h"

CTiebaOperate::CTiebaOperate(CString& cookie, const int& banDuration, const CString& banReason) :
	m_cookie(cookie), 
	m_banDuration(banDuration), 
	m_banReason(banReason)
{
}

// 设置要操作的贴吧
CTiebaOperate::SetTiebaResult CTiebaOperate::SetTieba(const CString& forumName)
{
	if (forumName == _T(""))
		return SET_TIEBA_NOT_FOUND;

	CString src = this->HTTPGet(_T("https://tieba.baidu.com/f?ie=utf-8&kw=") + EncodeURI(forumName));
	if (src == NET_TIMEOUT_TEXT)
		return SET_TIEBA_TIMEOUT;

	// 采集贴吧信息
	CString tmp = GetStringBetween(src, _T("PageData.forum"), _T("}"));
	tmp.Replace(_T("\r\n"), _T(""));
	std::wcmatch res;
	if (!std::regex_search((LPCTSTR)tmp, res, FORUM_ID_NAME_REG))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NOT_FOUND;
	}

	// 取贴吧ID
	m_forumID = res[1].str().c_str();

	// 取贴吧名
	m_forumName = JSUnescape(res[2].str().c_str());
	m_encodedForumName = EncodeURI(m_forumName);

	// 取用户名
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG))
		m_userName = JSUnescape(res[1].str().c_str());
	if (m_userName == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NOT_LOGIN;
	}

	// 验证用户权限
	// 旧接口
	//CString src2 = ::HTTPGet(_T("http://tieba.baidu.com/f/bawu/admin_group?kw=") + EncodeURI_GBK(m_forumName));
	/* CString src2 = ::HTTPGet(_T("http://tieba.baidu.com/bawu2/platform/listBawuTeamInfo?word=") + m_encodedForumName + _T("&ie=utf-8"));
	if (src2 == NET_TIMEOUT_TEXT)
		return SET_TIEBA_TIMEOUT;
	CStringArray bawuList;
	SplitString(bawuList, src2, _T("class=\"bawu_single_type"));
	BOOL hasPower = FALSE;
	if (bawuList.GetSize() > 1)
	{
		bawuList[bawuList.GetSize() - 1] = GetStringBefore(bawuList[bawuList.GetSize() - 1], _T("</div></div>"));
		for (int i = 1; i < bawuList.GetSize(); i++)
		if ((bawuList[i].Find(_T("吧主<span")) != -1 // WTF，怎么有这么多种吧主
			|| bawuList[i].Find(_T(">语音小编<span")) != -1)
			&& bawuList[i].Find(_T(">") + m_userName + _T("<")) != -1)
		{
			hasPower = TRUE;
			break;
		}
	}
	if (!hasPower)
		WriteString(src2, _T("admin.txt"));
		*/
	BOOL hasPower = TRUE;

	// 取tbs(口令号)
	m_tbs = GetStringBetween(src, _TBS_LEFT, _TBS_RIGHT);
	if (m_tbs == _T("") && std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, TBS_REG))
		m_tbs = JSUnescape(res[1].str().c_str());
	if (m_tbs == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NO_TBS;
	}

	// 确定BDUSS
	m_bduss = GetStringBetween(m_cookie, _T("BDUSS="), _T(";"));
	
	return hasPower ? SET_TIEBA_OK : SET_TIEBA_NO_POWER;
}


// 带Cookie
CString CTiebaOperate::HTTPGet(const CString& URL)
{
	return ::HTTPGet(URL, &m_cookie);
}

// 带Cookie
CString CTiebaOperate::HTTPPost(const CString& URL, const CString& data)
{
	return ::HTTPPost(URL, data, &m_cookie);
}


// 取错误代码
static inline CString GetOperationErrorCode(const CString& src)
{
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-65536");
	CString code = GetStringBetween(src, _T("no\":"), _T(","));
	if (code == _T(""))
		code = GetStringBetween(src, _T("code\":\""), _T("\""));
	if (code != _T("0")) {
		WriteString(src, _T("operation.txt"));
		if (GetTiebaErrorText(code) == _T("未收录错误"))
			DebugRecord(_T("未知错误代码"), src);
	}
	return code;
}

// 封ID，返回错误代码 针对无用户名ID
CString CTiebaOperate::BanID(const CString& userName, const CString& portrait, const CString& nick_name)
{
	//兼容原版 有用户名直接封
	if (userName != _T("")) {
		return BanID(userName);
	}
	//不存在userName
	//portrait[]=xxxxxxxxxxxxxxxxxxxxxxx
	//http://tb.himg.baidu.com/sys/portrait/item/xxxxxxxxxxxxxxx?t=zzzzzzzzzz
	//http://tb.himg.baidu.com/sys/portrait/item/xxxxxxxxxxxxxxx
	//xxxxxxxxxxxxxxx
	if (portrait == _T("")) {
		//本身就空。
		return BanID(userName);
	}
	CString data, tmp;
	tmp = GetStringBetween(portrait, AUTHOR_PORTRAIT_LEFT, AUTHOR_PORTRAIT_RIGHT);
	if (tmp == _T("")) {
		tmp = GetStringAfter(portrait, AUTHOR_PORTRAIT_LEFT);
		if (tmp == _T("")) {
			//更新兼容LoopBan格式。
			tmp = portrait;
		}
	} /*
	if (nick_name == _T("") && userName == _T("")) {
		data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&pid%%5B%%5D=%s&reason=%s&portrait%%5B%%5D=%s"),
			m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)pid,
			m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"), (LPCTSTR)EncodeURI(tmp));
	}*/
	//else {
		data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&reason=%s&portrait%%5B%%5D=%s&nick_name%%5B%%5D=%s"),
			m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)EncodeURI(userName),
			m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("1"), (LPCTSTR)EncodeURI(tmp), (LPCTSTR)EncodeURI(nick_name));
	//}
	CString src = this->HTTPPost(_T("https://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// 封ID，返回错误代码，不用PID（用户必须为本吧会员）//好像现在已经没有这个限制了？
CString CTiebaOperate::BanID(const CString& userName)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&reason=%s"),
		m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)EncodeURI(userName),
		m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("1"));
	CString src = this->HTTPPost(_T("https://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// 封ID，返回错误代码，客户端接口
CString CTiebaOperate::BanIDClient(const CString& userName, const CString& portrait, const CString& nick_name)
{
	CString data, tmp;
	tmp = GetPortraitFromString(portrait);
	//兼容原版 有用户名直接封
	if (userName != _T("") && tmp == _T("")) {
		return BanIDClient(userName);
	}
	data.Format(_T("BDUSS=%s&day=%d&fid=%s&nick_name=%s&ntn=banid&portrait=%s&reason=%s&tbs=%s&un=%s&word=%s&z=9998732423"),
		(LPCTSTR)m_bduss, m_banDuration, (LPCTSTR)m_forumID,
		(LPCTSTR)nick_name, (LPCTSTR)tmp, m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("1"),
		(LPCTSTR)m_tbs, (LPCTSTR)userName, (LPCTSTR)m_forumName);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/c/bawu/commitprison"), data, NULL, TYPE_CLIENT_VERSION_12);
	return GetOperationErrorCode(src);
}

// 封ID，返回错误代码，客户端接口，不用PID，小吧可封10天
CString CTiebaOperate::BanIDClient(const CString& userName)
{
	// 客户端POST要带数字签名，参数按字典序排列，去掉&，加上"tiebaclient!!!"，转成UTF-8，取MD5
	CString data;
	data.Format(_T("BDUSS=%s&day=%d&fid=%s&ntn=banid&tbs=%s&un=%s&word=%s&z=1111111111"),
		(LPCTSTR)m_bduss, m_banDuration, (LPCTSTR)m_forumID,
		(LPCTSTR)m_tbs, userName, (LPCTSTR)m_forumName);
	CString src = TiebaClientHTTPPost(_T("https://c.tieba.baidu.com/c/c/bawu/commitprison"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// 拉黑，返回错误代码
CString CTiebaOperate::Defriend(const CString& userID)
{
	CString src = this->HTTPPost(_T("https://tieba.baidu.com/bawu2/platform/addBlack"), _T("ie=utf-8&tbs=") + m_tbs
		+ _T("&user_id=") + userID + _T("&word=") + m_encodedForumName);
	return GetOperationErrorCode(src);
}

// 删主题，返回错误代码
CString CTiebaOperate::DeleteThread(const CString& tid)
{
	CString data;
	data.Format(_T("BDUSS=%s&fid=%s&is_frs_mask=%d&tbs=%s&z=%s"),
		(LPCTSTR)m_bduss, (LPCTSTR)m_forumID, 0, (LPCTSTR)m_tbs, tid);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/c/bawu/delthread"), data, NULL, TYPE_CLIENT_VERSION_12);
	//CString src = this->HTTPPost(_T("https://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + m_encodedForumName
	//	+ _T("&fid=") + m_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + m_tbs);
	return GetOperationErrorCode(src);
}

// 删主题，返回错误代码
CString CTiebaOperate::DeleteThread_web(const CString& tid)
{
	CString src = this->HTTPPost(_T("https://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + m_encodedForumName
		+ _T("&fid=") + m_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + m_tbs);
	return GetOperationErrorCode(src);
}

// 删帖子，返回错误代码
CString CTiebaOperate::DeletePost(const CString& tid, const CString& pid)
{
	CString data;
	data.Format(_T("BDUSS=%s&fid=%s&pid=%s&tbs=%s&z=%s"),
		(LPCTSTR)m_bduss, (LPCTSTR)m_forumID, pid, (LPCTSTR)m_tbs, tid);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/c/bawu/delpost"), data, NULL, TYPE_CLIENT_VERSION_12);
	//data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"),
	//	(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, pid);
	//CString src = this->HTTPPost(_T("https://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 删帖子，返回错误代码
CString CTiebaOperate::DeletePost_web(const CString& tid, const CString& pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"),
		(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, pid);
	CString src = this->HTTPPost(_T("https://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 删楼中楼，返回错误代码
CString CTiebaOperate::DeleteLZL(const CString& tid, const CString& lzlid)
{
	CString data;
	data.Format(_T("BDUSS=%s&fid=%s&pid=%s&tbs=%s&z=%s"),
		(LPCTSTR)m_bduss, (LPCTSTR)m_forumID, lzlid, (LPCTSTR)m_tbs, tid);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/c/bawu/delpost"), data, NULL, TYPE_CLIENT_VERSION_12);
	//data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
	//	(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, lzlid);
	//CString src = this->HTTPPost(_T("https://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 头像ID 获取封禁信息 正常为0 永封为36500 获取失败 -1
TIEBA_API_API int GetUserAntiDay(const CString& u_portrait, CString& u_ret, CString& u_name)
{
	u_ret = _T("");
	// 请求构造
	ProfileReqIdl pbReq;
	ProfileReqIdl_DataReq* pbReqData = new ProfileReqIdl_DataReq();
	CommonReq* pbCom = new CommonReq();
	pbCom->set__client_version("12.12.1.0");
	pbCom->set__client_type(2);
	pbReqData->set_allocated_common(pbCom);
	CStringA a_portrait = W2UTF8(u_portrait);
	pbReqData->set_friend_uid_portrait(a_portrait);
	pbReqData->set_pn(1);
	pbReqData->set_need_post_count(0);
	pbReq.set_allocated_data(pbReqData);
	std::string pbData;
	pbReq.SerializeToString(&pbData);

	CStringA data;
	data = pbData.c_str();

	CStringA src = TiebaClientHTTPProtoPost(_T("http://c.tieba.baidu.com/c/u/user/profile?cmd=303012"), data);
	std::string src2 = std::string(src, src.GetLength());
	if (src == NET_TIMEOUT_TEXT) {
		u_ret = D2F_RET_TIME_OUT;
		return D2F_INT_TIME_OUT;
	}

	// 结果解析
	ProfileResIdl pbRes;
	ProfileResIdl_DataRes* pbResData;
	ProfileResIdl_DataRes_Anti* pbResDataAnti;
	Error* pbError;
	User* pbUser;
	pbRes.ParseFromString(src2);

	pbResData = pbRes.mutable_data();
	pbError = pbRes.mutable_error();
	pbUser = pbResData->mutable_user();
	pbResDataAnti = pbResData->mutable_anti_stat();
	int c_error_nu = pbError->errorno();
	std::string error_msg = pbError->errmsg();
	CString c_error_msg = strUTF82W(error_msg);
	//std::string usermsg = pbError->usermsg();
	//CString c_usermsg = strUTF82W(usermsg);
	//int block_stat = pbResDataAnti->block_stat();
	//int hide_stat = pbResDataAnti->hide_stat();
	int days_tofree = pbResDataAnti->days_tofree();
	std::string name = pbUser->name();
	CString c_name = strUTF82W(name);
	std::string name_show = pbUser->name_show();
	CString c_name_show = strUTF82W(name_show);
	std::string portrait = pbUser->portrait();
	CString c_portrait = strUTF82W(portrait);
	INT64 uid = pbUser->id();

	if (c_error_nu == 300003) {
		// 300003   D2F_RET_DELETE
		u_ret = D2F_RET_DELETE;
		if (c_portrait != L"") {
			DebugRecord(_T("GetUserAntiDay 300003 错误代码，但头像id不为空"), _T("头像ID：") + c_portrait);
		}
		return D2F_INT_DELETE;
	}
	else if (c_error_nu != 0) {
		DebugRecord(_T("GetUserAntiDay 错误代码不为0"), c_error_nu, _T("头像ID：") + c_portrait);
	}

	bool is_empty = c_portrait == L"";
	if (is_empty) {
		u_ret = D2F_RET_ERROR;
		return D2F_INT_ERROR;
	}
	if (days_tofree == 0)
		u_ret = D2F_RET_NORMAL;
	else if (days_tofree > 0)
		u_ret = D2F_RET_BAN + Int2CString(days_tofree);
	if (c_name == _T(""))
		u_name = c_name_show;
	else
		u_name = c_name + _T(" - ") + c_name_show;
	return days_tofree;
}

// 头像ID 获取封禁信息 正常为0 永封为36500 获取失败 -1
TIEBA_API_API int GetUserAntiDay(const CString& u_portrait, CString& u_ret)
{
	CString u_name = _T("");
	return GetUserAntiDay(u_portrait, u_ret, u_name);
}

// 取错误文本
TIEBA_API_API CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("-65536"))
		return _T("超时");
	if (errorCode == _T("-1"))
		return _T("权限不足，(如是吧务，尝试重新登录)");
	if (errorCode == _T("1"))
		return _T("未登陆(重新登陆账号)");
	if (errorCode == _T("4"))
		return _T("参数校验失败");
	if (errorCode == _T("11"))
		return _T("度娘抽了");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("已被系统封禁");
	if (errorCode == _T("72"))
		return _T("权限不足，(如是吧务，尝试重新登录)");
	if (errorCode == _T("74"))
		return _T("用户不存在(可能帖子已被删且用户已退出本吧会员且用户已隐藏动态)");
	if (errorCode == _T("77"))
		return _T("操作失败");
	if (errorCode == _T("78"))
		return _T("参数错误");
	if (errorCode == _T("308"))
		return _T("你被封禁或失去权限");
	if (errorCode == _T("871"))
		return _T("高楼不能删");
	if (errorCode == _T("872"))
		return _T("精品贴不能删");
	if (errorCode == _T("890"))
		return _T("贴子已删"); 
	if (errorCode == _T("1039"))
		return _T("这个帖子已经被删除了哦");
	if (errorCode == _T("4011"))
		return _T("需要验证码(操作太快？)");
	if (errorCode == _T("220034"))
		return _T("您的操作太频繁了");
	if (errorCode == _T("230308"))
		return _T("没有权限（扫到跨吧广告或登录信息失效）");
	if (errorCode == _T("224009"))
		return _T("帖子已被锁定");
	if (errorCode == _T("230871"))
		return _T("发帖太频繁，停下来喝杯茶休息下吧（应该是删帖太频繁触发）");
	if (errorCode == _T("300000"))
		return _T("由于系统升级，烦请您更新至最新版客户端处理违规用户，感谢您的理解与支持！(暂时无法用旧版客户端api封禁用户名为空用户)");
	if (errorCode == _T("1211039"))
		return _T("删除主题失败/这个帖子已经被删除了哦");
	if (errorCode == _T("1989005"))
		return _T("加载数据失败");
	if (errorCode == _T("2210002"))
		return _T("系统错误");
	return _T("未收录错误");
}

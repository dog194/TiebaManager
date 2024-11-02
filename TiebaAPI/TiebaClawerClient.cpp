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
#include "iostream"
#include "fstream"
#include <TiebaClawerClient.h>
#include <TBMCoreGlobal.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <TiebaClientHelper.h>

#include <document.h>
#include <stringbuffer.h>
#include <writer.h>

#include "TiebaProto/FrsPageReqIdl.pb.h"
#include "TiebaProto/FrsPageResIdl.pb.h"
#include "TiebaProto/PbPageReqIdl.pb.h"
#include "TiebaProto/PbPageResIdl.pb.h"
#include "TiebaProto/PbContent.pb.h"
#include "TiebaProto/PollInfo.pb.h"
#include "TiebaProto/TiebaPlusInfo.pb.h"
#include "TiebaProto/User.pb.h"

using namespace rapidjson;

#include <map>


// 使用客户端接口采集贴吧
BOOL TiebaClawerClient::GetThreads(const CString& forumName, const CString& ignoreThread, std::vector<TapiThreadInfo>& threads)
{
	CString data;
	data.Format(_T("_client_type=2&_client_version=5.7.0&from=tieba&kw=%s&net_type=1&pn=%d&q_type=2&rn=50&st_type=tb_forumlist&with_group=0"), 
		(LPCTSTR)forumName, _ttoi(ignoreThread) / 50 + 1);
	CString src = TiebaClientHTTPPost(_T("https://c.tieba.baidu.com/c/f/frs/page"), data);
	WriteString(src, _T("forum.txt"));
	
	threads.clear();
	GenericDocument<UTF16<> > document;
	document.Parse(src);
	if (document.HasParseError() || !document.IsObject())
		return FALSE;

	const auto& threadList = document[L"thread_list"];
	threads.resize(threadList.Size());
	int iThread = 0;
	for (const auto& rawThread : threadList.GetArray())
	{
		if (!rawThread.HasMember(L"tid")) // 去掉广告
			continue;

		auto& thread = threads[iThread];
		GenericStringBuffer<UTF16<> > buffer;
		Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
		rawThread.Accept(writer);
		thread.rawData = buffer.GetString();

		thread.tid = rawThread[L"tid"].GetString();
		thread.author = rawThread[L"author"][L"name"].GetString();
		thread.authorShowName = rawThread[L"author"][L"name_show"].GetString();
		thread.authorID = rawThread[L"author"][L"id"].GetString();
		thread.authorPortraitUrl = CString(AUTHOR_PORTRAIT_URL_PREFIX) + rawThread[L"author"][L"portrait"].GetString();
		if (rawThread.HasMember(L"create_time")) // 直播贴没有create_time
			thread.timestamp = _ttoi64(rawThread[L"create_time"].GetString());
		else
			thread.timestamp = 0;
		thread.reply = rawThread[L"reply_num"].GetString();
		thread.title = rawThread[L"title"].GetString();

		if (!rawThread[L"abstract"].Empty() && rawThread[L"abstract"][0].HasMember(L"text"))
			thread.preview = rawThread[L"abstract"][0][L"text"].GetString();
		else
			thread.preview = _T("");
		thread.preview += _T("\r\n");
		// 多媒体
		if (rawThread.HasMember(L"media")) // 直播贴没有media
		{
			for (const auto& media : rawThread[L"media"].GetArray())
			{
				if (!media.IsObject() || !media.HasMember(L"type"))
					continue;
				CString tmp;
				switch (_ttoi(media[L"type"].GetString()))
				{
				case 3: // 图片
					tmp.Format(_T(R"(<li><a class="thumbnail vpic_wrap"><img src="" attr="1234" data-original="%s"  bpic="%s" class="threadlist_pic j_m_pic "  /></a><div class="threadlist_pic_highlight j_m_pic_light"></div></li>)"),
						media[L"big_pic"].GetString(), media[L"src_pic"].GetString());
					break;
				case 5: // 视频
					tmp.Format(_T(R"(<li><div class="threadlist_video"><img src=""/><a href="#" data-threadid="1234" data-forumid="1234" data-isfive="0" data-video="%s" data-type="" data-duration="" class="threadlist_btn_play j_m_flash"></a>)"),
						media[L"src"].GetString());
					break;
				}
				thread.preview += tmp;
			}
		}
		
		if (rawThread.HasMember(L"last_replyer")) // 直播贴没有last_replyer
			thread.lastAuthor = rawThread[L"last_replyer"][L"name"].GetString();
		else
			thread.lastAuthor = _T("");

		++iThread;
	}
	threads.resize(iThread); // 去掉广告

	return TRUE;
}

TiebaClawer::GetPostsResult TiebaClawerClient::GetPosts(const CString& fid, const CString& tid, const CString& page,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	CString data;
	data.Format(_T("_client_type=2&_client_version=7.0.0&back=0&floor_rn=3&from=tieba&kz=%s&mark=0&net_type=1&pn=%s&rn=30&st_type=tb_bookmarklist&with_floor=1"), 
		(LPCTSTR)tid, (LPCTSTR)page);
	CString src = TiebaClientHTTPPost(_T("https://c.tieba.baidu.com/c/f/pb/page"), data);
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;
	//WriteString(src, _T("thread.txt"));
	return GetPosts(fid, tid, page, src, posts, lzls, addition);
}

// 取楼中楼
static void GetLzls(const CString& tid, const GenericDocument<UTF16<> >& document, std::map<CString, int>& userIndex, std::vector<LzlInfo>& lzls)
{
	const auto& userList = document[L"user_list"];

	// lzls.clear();
	int iLzls = 0;
	// 遍历楼层
	for (const auto& post : document[L"post_list"].GetArray())
	{
		if (!post[L"sub_post_list"].IsObject() || !post[L"sub_post_list"].HasMember(L"sub_post_list"))
			continue;

		CString pid = post[L"id"].GetString();
		// 遍历该层楼中楼
		const auto& subPostList = post[L"sub_post_list"][L"sub_post_list"];
		lzls.resize(lzls.size() + subPostList.Size());
		for (const auto& subPost : subPostList.GetArray())
		{
			auto& lzl = lzls[iLzls];
			GenericStringBuffer<UTF16<> > buffer;
			Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
			subPost.Accept(writer);
			lzl.rawData = buffer.GetString();

			lzl.tid = tid;
			lzl.authorID = subPost[L"author_id"].GetString();
			const auto& user = userList[userIndex[lzl.authorID]];
			lzl.author = user[L"name"].GetString();
			lzl.authorShowName = user[L"name_show"].GetString();
			lzl.authorPortraitUrl = CString(AUTHOR_PORTRAIT_URL_PREFIX) + user[L"portrait"].GetString();
			lzl.timestamp = _ttoi64(subPost[L"time"].GetString());
			lzl.cid = subPost[L"id"].GetString();
			lzl.floor = post[L"floor"].GetString();

			lzl.content = _T("");
			for (const auto& content : subPost[L"content"].GetArray())
			{
				CString tmp;
				switch (_ttoi(content[L"type"].GetString()))
				{
				case 0: // 文字
					tmp = content[L"text"].GetString();
					break;
				case 1: // 超链接
					tmp.Format(_T(R"(<a href="%s"  target="_blank">%s</a>)"), content[L"link"].GetString(), content[L"text"].GetString());
					break;
				case 2: // 表情
					tmp.Format(_T(R"(<img class="BDE_Smiley" width="30" height="30" changedsize="false" src="http://static.tieba.baidu.com/tb/editor/images/client/%s.png" >)"),
						content[L"text"].GetString());
					break;
				case 4: // @
					tmp.Format(_T(R"#(<a href=""  onclick="Stats.sendRequest('fr=tb0_forum&st_mod=pb&st_value=atlink');" onmouseover="showattip(this)" onmouseout="hideattip(this)" username="%s" target="_blank" class="at">%s</a>)#"),
						content[L"text"].GetString() + 1, content[L"text"].GetString());
					break;
				case 10: // 语音
					tmp.Format(_T("\r\n") _T(R"(<div class="voice_player voice_player_mini voice_player_lzl"><a class="voice_player_inner" href="#"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time" style="width: 65px;"><span class="second">%d</span>"</span></span><span class="after">&nbsp;</span></a></div>)"),
						_ttoi(content[L"during_time"].GetString()) / 1000);
					break;
				}
				lzl.content += tmp;
			}

			++iLzls;
		}
	}
}

TiebaClawer::GetPostsResult TiebaClawerClient::GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition, const CStringA& srcA)
{
	posts.clear();
	lzls.clear();
	GenericDocument<UTF16<> > document;
	document.Parse(src);
	if (document.HasParseError() || !document.IsObject() || wcscmp(document[L"error_code"].GetString(), L"0") != 0)
		return GET_POSTS_DELETED;

	const auto& userList = document[L"user_list"];
	// 映射用户ID和索引
	std::map<CString, int> userIndex;
	int size = userList.Size();
	for (int i = 0; i < size; ++i)
		userIndex[userList[i][L"id"].GetString()] = i;

	const auto& postList = document[L"post_list"];
	size = postList.Size();
	posts.resize(size);
	// 遍历楼层
	for (int i = 0; i < size; ++i)
	{
		const auto& rawPost = postList[i];
		auto& post = posts[i];
		GenericStringBuffer<UTF16<> > buffer;
		Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
		rawPost.Accept(writer);
		post.rawData = buffer.GetString();

		post.tid = tid;
		post.authorID = rawPost[L"author_id"].GetString();
		const auto& user = userList[userIndex[post.authorID]];
		if (user.HasMember(L"name")) // 远古时期的IP发帖作者
			post.author = user[L"name"].GetString();
		else
			post.author = _T("");
		post.authorShowName = user[L"name_show"].GetString();
		post.pid = rawPost[L"id"].GetString();
		post.floor = rawPost[L"floor"].GetString();
		if (user.HasMember(L"level_id")) // 远古时期的IP发帖作者
			post.authorLevel = user[L"level_id"].GetString();
		else
			post.authorLevel = _T("1");
		post.authorPortraitUrl = CString(AUTHOR_PORTRAIT_URL_PREFIX) + user[L"portrait"].GetString();
		post.timestamp = _ttoi64(rawPost[L"time"].GetString());

		post.content = _T("");
		for (const auto& content : rawPost[L"content"].GetArray())
		{
			CString tmp;
			if (content.IsNull()) {
				continue;
			}
			else if (content.IsString()) {
				continue;
			}
			else if (!content.IsObject()) {
				continue;
			}
			switch (_ttoi(content[L"type"].GetString()))
			{
			case 0: // 文字
				tmp = content[L"text"].GetString();
				break;
			case 1: // 超链接
				tmp.Format(_T(R"(<a href="%s"  target="_blank">%s</a>)"), content[L"link"].GetString(), content[L"text"].GetString());
				break;
			case 2: // 表情
				tmp.Format(_T(R"(<img class="BDE_Smiley" width="30" height="30" changedsize="false" src="http://static.tieba.baidu.com/tb/editor/images/client/%s.png" >)"),
					content[L"text"].GetString());
				break;
			case 3: // 图片
			{
				CStringArray size;
				SplitString(size, content[L"bsize"].GetString(), _T(","));
				tmp.Format(_T(R"(<img class="BDE_Image" pic_type="0" width="%s" height="%s" src="%s" >)"), (LPCTSTR)size[0],
					(LPCTSTR)size[1], content[L"origin_src"].GetString());
				break;
			}
			case 4: // @
				tmp.Format(_T(R"#(<a href=""  onclick="Stats.sendRequest('fr=tb0_forum&st_mod=pb&st_value=atlink');" onmouseover="showattip(this)" onmouseout="hideattip(this)" username="%s" target="_blank" class="at">%s</a>)#"),
					content[L"text"].GetString() + 1, content[L"text"].GetString());
				break;
			case 5: // 视频
				tmp.Format(_T(R"(<embed class="BDE_Flash" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" wmode="transparent" play="true" loop="false" menu="false" src="%s" width="500" height="450" allowscriptaccess="never" allowfullscreen="true" scale="noborder">)"),
					content[L"text"].GetString());
				break;
			case 9: // 电话号码
				tmp = content[L"text"].GetString();
				break;
			case 10: // 语音
				tmp.Format(_T(R"(<div class="voice_player voice_player_pb"><a href="#" class="voice_player_inner"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time"><span class="second">%d</span>&quot;</span></span><span class="after">&nbsp;</span></a></div><img class="j_voice_ad_gif" src="http://tb2.bdstatic.com/tb/static-pb/img/voice_ad.gif" alt="下载贴吧客户端发语音！" /><br/>)"),
					_ttoi(content[L"during_time"].GetString()) / 1000);
				break;
			}
			post.content += tmp;
		}
		// 小尾巴
		if (rawPost[L"signature"].IsObject()
			&& rawPost[L"signature"][L"content"].Size() > 0
			&& rawPost[L"signature"][L"content"][0].HasMember(L"text"))
		{
			post.content += _T("\r\n");
			post.content += rawPost[L"signature"][L"content"][0][L"text"].GetString();
		}
	}

	// 取楼中楼
	GetLzls(tid, document, userIndex, lzls);

	// 附加信息
	if (addition != NULL)
	{
		addition->src = src;
		addition->fid = document[L"forum"][L"id"].GetString();
		addition->pageCount = document[L"page"][L"total_page"].GetString();
	}

	return GET_POSTS_SUCCESS;
}

// ==============新版接口区域===============
const static void decodeUser(const User& rawUser, TBUserObj& user) {
	// 数据读取
	CString id = Int64oCString(rawUser.id());
	CString name = strUTF82W(rawUser.name());
	CString nameShow = strUTF82W(rawUser.name_show());
	CString portrait = strUTF82W(rawUser.portrait());
	CString portraitUrl = AUTHOR_PORTRAIT_URL_PREFIX + portrait;
	int level = rawUser.level_id();
	int glevel = rawUser.user_growth().level_id();
	int is_bawu = rawUser.is_bawu();
	CString bawuType = strUTF82W(rawUser.bawu_type());
	int post_num = rawUser.post_num();
	CString tb_age = strUTF82W(rawUser.tb_age());
	int is_default_avatar = rawUser.is_default_avatar();
	CString tb_uid = strUTF82W(rawUser.tieba_uid());
	int reply_type = rawUser.priv_sets().reply();
	// 虚拟形象，user_list 不带，需要另外地方获取
	User_VirtualImageInfo pbVirtualImageInfo = rawUser.virtual_image_info();
	User_VirtualImageInfo_StateInfo pbStateInfo = pbVirtualImageInfo.personal_state();
	int is_set_virtual = pbVirtualImageInfo.isset_virtual_image();
	CString virtual_info = strUTF82W(pbStateInfo.text());
	CString ip_address = strUTF82W(rawUser.ip_address());

	// 赋值
	user.id = id;
	user.name = name;
	user.ShowName = nameShow;
	user.Portrait = portrait;
	user.PortraitUrl = portraitUrl;
	user.level = level;
	user.glevel = glevel;
	user.is_bawu = is_bawu;
	user.bawu_type = bawuType;
	user.post_num = post_num;
	user.tb_age = tb_age;
	user.is_default_avatar = is_default_avatar;
	user.tieba_uid = tb_uid;
	user.reply_type = reply_type;
	user.virtual_info = virtual_info;
	user.ip_address = ip_address;
}

// 解析UserList
const static void decodeUserList(const ::google::protobuf::RepeatedPtrField<User>* pbUserList, 
	std::vector<TBUserObj>& userList) {
	userList.clear();
	if (pbUserList->size() == 0) {
		return;
	}
	userList.resize(pbUserList->size());
	int iUser = 0;
	for (auto& rawUser = pbUserList->begin(); rawUser != pbUserList->end(); ++rawUser)
	{
		// 初判断
		INT64 id64 = rawUser->id();
		CString id;
		id.Format(_T("%I64d"), id64);
		if (id == _T("0")) { // id 为空
			CString debugStr;
			debugStr = _T("随便写，用来插断点");
			continue;
		}
		auto& user = userList[iUser];
		decodeUser(*rawUser, user);
		//WriteStringCon(user.GetContent(), _T("userlist.txt"));
		++iUser;
	}
	userList.resize(iUser);
}

// 进行ContentDebug
const static void debugContent(PbContent pbContent, const CString& tid = _T(""), const CString& pid = _T("")) {
	UINT type = pbContent.type();
	CString tmp, output = _T(""), fname = _T("debugContent.txt");
	tmp.Format(_T("%s TYPE:%d\tTID: %s\tPID: %s"), GetYYMMDD_HHMMSS_FromTimeT(), type, tid, pid);
	WriteStringCon(tmp, fname);
	std::string textStr = pbContent.text();
	CString text = strUTF82W(textStr);
	std::string linkStr = pbContent.link();
	CString link = strUTF82W(linkStr);
	std::string srcStr = pbContent.src();
	CString src = strUTF82W(srcStr);
	std::string bsizeStr = pbContent.bsize();
	CString bsize = strUTF82W(bsizeStr);
	tmp.Format(_T("text: %s\tlink: %s\tsrc: %s\tbsize: %s\n"), text, link, src, bsize);
	output += tmp;

	std::string imgtypeStr = pbContent.imgtype();
	CString imgtype = strUTF82W(imgtypeStr);
	std::string voice_md5Str = pbContent.voice_md5();
	CString voice_md5 = strUTF82W(voice_md5Str);
	UINT32 width = pbContent.width();
	UINT32 height = pbContent.height();
	tmp.Format(_T("imgtype: %s\tvoice_md5: %s\twidth: %d\theight: %d\n"), imgtype, voice_md5, width, height);
	output += tmp;

	std::string packet_nameStr = pbContent.packet_name();
	CString packet_name = strUTF82W(packet_nameStr);
	std::string phonetypeStr = pbContent.phonetype();
	CString phonetype = strUTF82W(phonetypeStr);
	UINT32 e_type = pbContent.e_type();
	UINT32 during_time = pbContent.during_time();
	tmp.Format(_T("packet_name: %s\tphonetype: %s\te_type: %d\tduring_time: %d"), packet_name, phonetype, e_type, during_time);
	output += tmp;
	WriteStringCon(output, fname);
}

// 解析Content
const static CString decodeContent(::google::protobuf::RepeatedPtrField<PbContent>* pbContentList, 
	const CString& tid = _T(""), const CString& pid = _T(""), const BOOLEAN textOnly = FALSE) {
	CString ret = _T("");
	if (pbContentList->size() == 0) {
		return ret;
	}
	for (auto& rawContent = pbContentList->begin(); rawContent != pbContentList->end(); ++rawContent)
	{
		UINT type = rawContent->type();
		CString content, tmp, tmpSize;
		std::string tmpStr, tmpSizeStr;
		TiebaPlusInfo* pbTbPlusInfo;
		switch (type)
		{
		case 0: // 文字
		case 8: // 不知道是啥 link: bdhd
		case 9: // 电话号码
		case 18: // 话题
		case 27: // 词条
		case 40: // 梗百科
			content = strUTF82W(rawContent->text());
			break;
		case 1: // 超链接
			if (textOnly) {
				content = _T("");
			}
			else {
				tmp = strUTF82W(rawContent->link());
				content.Format(_T(R"(<a href="%s"  target="_blank">%s</a>)"), tmp, tmp);
			}
			break;
		case 2: // 表情
			if (textOnly) {
				content = _T("");
			}
			else {
				tmp = strUTF82W(rawContent->text());
				content.Format(_T(R"(<img class="BDE_Smiley" width="30" height="30" changedsize="false" src="http://static.tieba.baidu.com/tb/editor/images/client/%s.png" >)"),
					tmp);
			}
			break;
		case 3: // 图片
		{
			if (textOnly) {
				content = _T("");
			}
			else {
				tmpSize = strUTF82W(rawContent->bsize());
				tmp = strUTF82W(rawContent->origin_src());
				CStringArray size;
				SplitString(size, tmpSize, _T(","));
				content.Format(_T(R"(<img class="BDE_Image" pic_type="0" width="%s" height="%s" src="%s" >)"), (LPCTSTR)size[0],
					(LPCTSTR)size[1], tmp);
			}
			break;
		}
		case 4: // @
			if (textOnly) {
				content = strUTF82W(rawContent->text());
			}
			else {
				tmp = strUTF82W(rawContent->text());
				tmpSize = Int64oCString(rawContent->uid());
				content.Format(_T(R"#(<a href="" username="%s" class="at">%s</a>)#"), tmpSize, tmp);
			}
			break;
		case 5: // 视频
			if (textOnly) {
				content = _T("");
			}
			else {
				tmp = strUTF82W(rawContent->text());
				content.Format(_T(R"(<embed class="BDE_Flash" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" wmode="transparent" play="true" loop="false" menu="false" src="%s" width="500" height="450" allowscriptaccess="never" allowfullscreen="true" scale="noborder">)"),
					tmp);
			}
			//debugContent(*rawContent, tid, pid);
			break;
		case 10: // 语音
			if (textOnly) {
				content = _T("");
			}
			else {
				content.Format(_T(R"(<div class="voice_player voice_player_pb"><a href="#" class="voice_player_inner"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time"><span class="second">%d</span>&quot;</span></span><span class="after">&nbsp;</span></a></div><img class="j_voice_ad_gif" src="http://tb2.bdstatic.com/tb/static-pb/img/voice_ad.gif" alt="下载贴吧客户端发语音！" /><br/>)"),
					rawContent->during_time() / 1000);
			}
			break;
		case 34: // TiebaPlusInfo 应该也是
		case 35: // TiebaPlusInfo
			pbTbPlusInfo = rawContent->mutable_tiebaplus_info();
			content.Format(_T(R"(%s<tpcl>%s</tpcl>)"),
				strUTF82W(rawContent->text()), strUTF82W(pbTbPlusInfo->h5_jump_number()));
			break;
		case 36: // TODO
		case 37:
			pbTbPlusInfo = rawContent->mutable_tiebaplus_info();
			content = _T("");
			break;
		case 11: // 特殊表情，哈米猫，滑稽，鸡年滑稽等
		case 20: // 大表情，图片表情
			content = _T("");
			break;
		default:
			debugContent(*rawContent, tid, pid);
			content = _T("");
		}
		ret += content;
	}
	return ret;
}

// 解析Lzl
const static void decodeLzl(SubPostList& pbSubPost, LzlInfo& lzl, const CString floor, const std::vector<TBUserObj>& PostuserList,
	std::map<CString, int>& userIndex, const CString& tid = _T(""))
{
	// 数据读取
	CString pid = Int64oCString(pbSubPost.id());
	UINT32 time = pbSubPost.time();
	CString title = strUTF82W(pbSubPost.title());
	// UINT32 floor = pbSubPost.floor();
	CString uid = Int64oCString(pbSubPost.author_id());
	
	/* User 里面并没有东西，浪费感情
	User* pbUser = pbSubPost.mutable_author();
	TBUserObj* user = new TBUserObj();
	decodeUser(*pbUser, *user); */

	::google::protobuf::RepeatedPtrField<PbContent>* pbContentList;
	pbContentList = pbSubPost.mutable_content();
	CString content = decodeContent(pbContentList, tid, pid);

	// 赋值
	lzl.rawData = _T("");
	lzl.tid = tid;
	lzl.authorID = PostuserList[userIndex[uid]].id;
	lzl.author = PostuserList[userIndex[uid]].name;
	lzl.authorShowName = PostuserList[userIndex[uid]].ShowName;
	lzl.authorPortraitUrl = PostuserList[userIndex[uid]].PortraitUrl;
	lzl.authorLevel = Int2CString(PostuserList[userIndex[uid]].level);
	lzl.timestamp = time;
	lzl.cid = pid;
	lzl.floor = floor;
	lzl.content = content;
}

const CString STR_THREAD_VOTE = _T("\n[请更新到贴吧App最新版本查看投票模块]");
const CString STR_FAN_ONLY = _T("\r\n[由于楼主的设置，你需要关注TA才能发表评论]");
const CString STR_ONLY_F = _T("\r\n[由于楼主设置了评论权限，你无法评论此贴哦]");

// 使用新版客户端接口采集贴吧
BOOL TiebaClawerClientNickName::GetThreads(const CString& forumName, const CString& ignoreThread, 
	std::vector<TapiThreadInfo>& threads)
{
	// 请求构造
	FrsPageReqIdl pbReq;
	FrsPageReqIdl_DataReq* pbReqData = new FrsPageReqIdl_DataReq();
	CommonReq* pbCom = new CommonReq();
	pbCom->set__client_version("12.53.1.0");
	pbCom->set__client_type(2);
	pbReqData->set_allocated_common(pbCom);
	CStringA t_fn = W2UTF8(forumName);
	//std::string kw = CT2A(forumName);
	pbReqData->set_kw(t_fn);
	pbReqData->set_pn(_ttoi(ignoreThread) / 50 + 1);
	// 暂时改为10.应对贴吧数据修改
	pbReqData->set_rn(10);
	pbReqData->set_rn_need(10);
	pbReqData->set_cid(0);
	pbReqData->set_is_good(0);
	pbReqData->set_q_type(2);
	pbReqData->set_sort_type(5);
	pbReq.set_allocated_data(pbReqData);
	std::string pbData;
	pbReq.SerializeToString(&pbData);

	CStringA data;
	data = pbData.c_str();
	/*
	std::ofstream file("outputPbData.txt", std::ios::binary);
	file.write(pbData.c_str(), sizeof(char) * (pbData.size()));
	file.close();*/
	// CStringA to std::string 
	/* int iLen = data.GetLength() + 1;
	char * pSec = data.GetBuffer(iLen * sizeof(char));
	std::string psecstr = std::string(pSec);
	data.ReleaseBuffer();*/
	
	CStringA src = TiebaClientHTTPProtoPost(_T("http://c.tieba.baidu.com/c/f/frs/page?cmd=301001"), data);
	std::string src2 = std::string(src, src.GetLength());
	//std::ofstream filesrc("forum12.21.1.txt", std::ios::binary);
	//filesrc.write(src, sizeof(char) * (src.GetLength()));
	//filesrc.close();

	// 结果解析
	FrsPageResIdl pbRes;
	FrsPageResIdl_DataRes* pbResData;
	::google::protobuf::RepeatedPtrField<ThreadInfo>* pbThreadList;
	::google::protobuf::RepeatedPtrField<User>* pbUserList;
	Error* error;

	pbRes.ParseFromString(src2);
	error = pbRes.mutable_error();
	pbResData = pbRes.mutable_data();
	pbThreadList = pbResData->mutable_thread_list();
	pbUserList = pbResData->mutable_user_list();

	// 用户列表解析
	std::vector<TBUserObj> userList;
	decodeUserList(pbUserList, userList);
	// 映射用户ID和索引
	std::map<CString, int> userIndex;
	int uSize = userList.size();
	for (int i = 0; i < uSize; ++i)
		userIndex[userList[i].id] = i;
	
	threads.clear();
	if (pbThreadList->size() == 0) {
		return FALSE;
	}

	threads.resize(pbThreadList->size());
	int iThread = 0;
	for (auto& rawThread = pbThreadList->begin(); rawThread != pbThreadList->end(); ++rawThread)
	{
		// 初判断
		INT64 tid64 = rawThread->id();
		CString tid;
		tid.Format(_T("%I64d"), tid64);
		if (tid == _T("0")) // tid 为空
			continue;
		int is_global_top = rawThread->is_global_top();
		if (is_global_top != 0) // is_global_top 不为 0 应该是跨吧置顶，跳过
			continue;

		// 数据读取
		std::string titleStr = rawThread->title();
		CString title = strUTF82W(titleStr);
		int reply_numInt = rawThread->reply_num();
		CString reply_num;
		reply_num.Format(_T("%d"), reply_numInt);
		//int view_num = rawThread->view_num();
		//int last_time_int = rawThread->last_time_int();
		//int is_top = rawThread->is_top();
		//int is_good = rawThread->is_good();
		UINT32 is_ad = rawThread->is_ad();
		INT64 fid64 = rawThread->fid();
		CString fid;
		fid.Format(_T("%I64d"), fid64);
		//INT64 first_post_id = rawThread->first_post_id();
		//int is_global_top = rawThread->is_global_top();
		int create_time = rawThread->create_time();
		INT64 author_id64 = rawThread->author_id();
		CString author_id;
		author_id.Format(_T("%I64d"), author_id64);
		::google::protobuf::RepeatedPtrField<PbContent>* pbContentList;
		pbContentList = rawThread->mutable_first_post_content();
		CString preview = decodeContent(pbContentList, tid);
		ThreadInfo_CustomState pbCustomState = rawThread->custom_state();
		CString customState = strUTF82W(pbCustomState.content());
		//int tab_id = rawThread->tab_id();
		
		//CString TMPP;
		//TMPP.Format(_T("标题：%s"), title);
		//WriteStringCon(TMPP, _T("row.txt"));
		/*
		TMPP.Format(_T("size %d \nreply_n %d \nview_n %d \ntime %d \nis top %d is good %d  is_ad %s\nis_global_top %d \nctime %d \ntab_id %d"),
			titleSize, reply_num, view_num, last_time_int,
			is_top, is_good, is_ad, is_global_top, create_time, tab_id);
		WriteStringCon(TMPP, _T("row.txt"));
		TMPP.Format(_T("tid %I64d\nfid %I64d\nfirst_post_id %I64d\nauthor_id %I64d"), 
			tid, fid, first_post_id, author_id);
		WriteStringCon(TMPP, _T("row.txt"));*/

		// 变量赋值
		auto& thread = threads[iThread];
		
		thread.rawData = _T(""); // rawData 也是二进制 没啥用了
		thread.tid = tid;
		thread.timestamp = create_time;
		thread.reply = reply_num;
		thread.customState = customState;
		if (title == _T("")) {
			thread.title = decodeContent(pbContentList, tid, _T(""), TRUE).Left(30);
		}
		else {
			thread.title = title;
		}
		if (userIndex.find(author_id) != userIndex.end()) {
			thread.author = userList[userIndex[author_id]].name;
			thread.authorShowName = userList[userIndex[author_id]].ShowName;
			thread.authorID = userList[userIndex[author_id]].id;
			thread.authorPortraitUrl = userList[userIndex[author_id]].PortraitUrl;
			if (userList[userIndex[author_id]].level == 0)
				thread.authorLevel = _T("");
			else
				thread.authorLevel = Int2CString(userList[userIndex[author_id]].level);
			thread.isTidAuthor = TRUE;
		}
		else {
			thread.author = _T("[数据错误]");
			thread.authorShowName = _T("[数据错误]");
			thread.authorID = _T("");
			thread.authorPortraitUrl = _T("[数据错误]");
			thread.authorLevel = _T("");
			thread.isTidAuthor = FALSE;
		}
		thread.preview = preview;
		//thread.preview += _T("\r\n");
		/*
		// 多媒体
		if (rawThread.HasMember(L"media")) // 直播贴没有media
		{
			for (const auto& media : rawThread[L"media"].GetArray())
			{
				if (!media.IsObject() || !media.HasMember(L"type"))
					continue;
				CString tmp;
				switch (_ttoi(media[L"type"].GetString()))
				{
				case 3: // 图片
					tmp.Format(_T(R"(<li><a class="thumbnail vpic_wrap"><img src="" attr="1234" data-original="%s"  bpic="%s" class="threadlist_pic j_m_pic "  /></a><div class="threadlist_pic_highlight j_m_pic_light"></div></li>)"),
						media[L"big_pic"].GetString(), media[L"src_pic"].GetString());
					break;
				case 5: // 视频
					tmp.Format(_T(R"(<li><div class="threadlist_video"><img src=""/><a href="#" data-threadid="1234" data-forumid="1234" data-isfive="0" data-video="%s" data-type="" data-duration="" class="threadlist_btn_play j_m_flash"></a>)"),
						media[L"src"].GetString());
					break;
				}
				thread.preview += tmp;
			}
		}*/

		thread.lastAuthor = _T("");
		++iThread;
	}
	threads.resize(iThread);
	return TRUE;
}

TiebaClawer::GetPostsResult TiebaClawerClientNickName::GetPosts(const CString& fid, const CString& tid, const CString& page,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	// 请求构造
	PbPageReqIdl pbReq;
	PbPageReqIdl_DataReq* pbReqData = new PbPageReqIdl_DataReq();
	CommonReq* pbCom = new CommonReq();

	pbCom->set__client_version("12.53.1.0");
	pbCom->set__client_type(2);
	pbReqData->set_allocated_common(pbCom);
	INT64 kz = _ttoi64(tid); 
	pbReqData->set_kz(kz);
	int pn = _ttoi(page);
	pbReqData->set_pn(pn);
	pbReqData->set_rn(30);
	pbReqData->set_q_type(2);
	pbReqData->set_with_floor(1);
	pbReqData->set_floor_rn(5);
	pbReqData->set_floor_sort_type(1);
	pbReq.set_allocated_data(pbReqData);
	std::string pbData;
	pbReq.SerializeToString(&pbData);
	
	CStringA data;
	data = pbData.c_str();

	CStringA srcA = TiebaClientHTTPProtoPost(_T("http://c.tieba.baidu.com/c/f/pb/page?cmd=302001"), data);
	if (srcA == NET_TIMEOUT_TEXT) {
		return GET_POSTS_TIMEOUT;
	}
	CString src = _T("");
	
	if (g_pTbmCoreConfig->m_acedEnhancedLzl) {
		src = HTTPGet(_T("https://tieba.baidu.com/p/totalComment?tid=") + tid + _T("&pn=") + page);
		if (src == NET_TIMEOUT_TEXT)
			return GET_POSTS_TIMEOUT;
	}

	return GetPosts(fid, tid, page, src, posts, lzls, addition, srcA);
}

TiebaClawer::GetPostsResult TiebaClawerClientNickName::GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition, const CStringA& srcA)
{
	std::string src2 = std::string(srcA, srcA.GetLength());

	// 结果解析
	PbPageResIdl pbRes;
	PbPageResIdl_DataRes* pbResData;
	Error* error;
	ThreadInfo* threadInfo;
	SimpleForum* simpleForum;
	Page* pbPage;
	User* threadUser;
	::google::protobuf::RepeatedPtrField<Post>* pbPostList;
	::google::protobuf::RepeatedPtrField<User>* pbPostUserList;
	
	pbRes.ParseFromString(src2);
	error = pbRes.mutable_error();
	pbResData = pbRes.mutable_data();
	threadInfo = pbResData->mutable_thread();
	threadUser = threadInfo->mutable_author();
	simpleForum = pbResData->mutable_forum();
	pbPage = pbResData->mutable_page();
	pbPostList = pbResData->mutable_post_list();
	pbPostUserList = pbResData->mutable_user_list();

	posts.clear();
	lzls.clear();

	//CString tmp;
	//tmp.Format(_T("%d - %d"), pbPostList->size(), pbUserList->size());
	//WriteString(tmp, _T("posts.txt"));

	int error_no = error->errorno();
	if (pbPostList->size() == 0 || error_no != 0) {
		return GET_POSTS_DELETED;
	}
	CString threadUid = Int64oCString(threadUser->id());
	if (threadUid == _T(""))
		threadUid = _T("? Error ?");
	// 用户列表解析
	std::vector<TBUserObj> PostuserList;
	decodeUserList(pbPostUserList, PostuserList);
	// 映射用户ID和索引
	std::map<CString, int> userIndex;

	// 解析 Json 前10楼 楼中楼
	bool aced_lzl_info = true;
	bool use_addl_lzl_info;
	if (src == _T(""))
		use_addl_lzl_info = false;
	else
		use_addl_lzl_info = true;
	std::vector<LzlInfo> jsonLzl;
	jsonLzl.clear();
	if (use_addl_lzl_info == true) {
		GenericDocument<UTF16<> > document;
		document.Parse(src);
		if (document.HasParseError() || !document.IsObject() || document[L"errno"].GetInt() != 0) {
			aced_lzl_info = false;
		}
		else if (document[L"errno"].IsInt() && document[L"errno"].GetInt() != 0) {
			aced_lzl_info = false;
		}
		else if (!document[L"errno"].IsInt()) {
			aced_lzl_info = false;
		}
		else {
			// "data":{"comment_list":[],"user_list":[]}}
			if (document[L"data"][L"comment_list"].IsArray()) {
				// "comment_list":[] 楼中楼为空
				aced_lzl_info = false;
			}
			else if (document[L"data"][L"comment_list"].IsObject()) {
				// "comment_list":{"148225253760":
				const auto& lzl_comment_list = document[L"data"][L"comment_list"];
				const auto& lzl_user_list = document[L"data"][L"user_list"];
				// lzl 用户 列表转换
				int lzl_ul_size = lzl_user_list.Size();
				int i = 0;
				for (auto& v : lzl_user_list.GetObject()) {
					CString v_id = v.name.GetString();
					// 判断是否已经有用户信息。
					bool is_add = true;
					for (auto& t : PostuserList) {
						if (t.id.CompareNoCase(v_id) == 0) {
							// 已经有
							is_add = false;
							break;
						}
					}
					if (is_add) {
						TBUserObj tmp_user = TBUserObj();
						tmp_user.id = v.name.GetString();
						tmp_user.name = v.value[L"user_name"].GetString();
						tmp_user.ShowName = v.value[L"show_nickname"].GetString();
						tmp_user.Portrait = v.value[L"portrait"].GetString();
						tmp_user.PortraitUrl = AUTHOR_PORTRAIT_URL_PREFIX + tmp_user.Portrait;
						tmp_user.level = 0;
						tmp_user.is_bawu = 0;
						tmp_user.bawu_type = L"";
						tmp_user.post_num = 0;
						tmp_user.tb_age = L"";
						tmp_user.is_default_avatar = 0;
						tmp_user.tieba_uid = L"";
						tmp_user.reply_type = 0;
						PostuserList.push_back(tmp_user);
					}					
				}
				// 更新 userIndex =================================
				int uSize = PostuserList.size();
				for (int i = 0; i < uSize; ++i)
					userIndex[PostuserList[i].id] = i;
				// ================================================
				// lzl_comment_list 用户 列表转换
				for (auto& v : lzl_comment_list.GetObject()) {
					// 第一层解析 
					CString v_key = v.name.GetString();
					for (auto& vv : v.value[L"comment_info"].GetArray()) {
						// 第二层遍历
						CString tmp_uid = Int64oCString(vv[L"user_id"].GetInt64());
						LzlInfo tmp_lzl = LzlInfo();
						// 赋值
						tmp_lzl.rawData = _T("");
						tmp_lzl.tid = tid;
						tmp_lzl.authorID = PostuserList[userIndex[tmp_uid]].id;
						tmp_lzl.author = PostuserList[userIndex[tmp_uid]].name;
						tmp_lzl.authorShowName = PostuserList[userIndex[tmp_uid]].ShowName;
						tmp_lzl.authorPortraitUrl = PostuserList[userIndex[tmp_uid]].PortraitUrl;
						if (PostuserList[userIndex[tmp_uid]].level == 0)
							tmp_lzl.authorLevel = _T("");
						else 
							tmp_lzl.authorLevel = Int2CString(PostuserList[userIndex[tmp_uid]].level);
						tmp_lzl.timestamp = vv[L"now_time"].GetInt64();
						tmp_lzl.cid = Int64oCString(vv[L"comment_id"].GetInt64());
						// floor 后续更新
						tmp_lzl.floor = v_key;
						tmp_lzl.content = _T("[aceLzl]") + HTMLDelete4totalComment(HTMLUnescape(vv[L"content"].GetString()));
						jsonLzl.push_back(tmp_lzl);
					}
				}
			}
		}
	}
	if (use_addl_lzl_info == false || aced_lzl_info == false) {
		// 没开启 楼中楼，或者楼中楼 获取失败
		int uSize = PostuserList.size();
		for (int i = 0; i < uSize; ++i)
			userIndex[PostuserList[i].id] = i;
	}

	int size = pbPostList->size();
	posts.resize(size);
	// 遍历楼层
	int iPost = 0;
	int iLzl = 0, tLzl = 0;
	for (auto& rawPost = pbPostList->begin(); rawPost != pbPostList->end(); ++rawPost)
	{
		// 数据读取
		CString pid = Int64oCString(rawPost->id());
		CString title = strUTF82W(rawPost->title());
		CString floor = Int2CString(rawPost->floor());
		UINT32 time = rawPost->time();
		CString author_id = Int64oCString(rawPost->author_id());
		Post_CustomState pbCustomState = rawPost->custom_state();
		CString customState = strUTF82W(pbCustomState.content());

		::google::protobuf::RepeatedPtrField<PbContent>* pbContentList;
		pbContentList = rawPost->mutable_content();
		CString content = decodeContent(pbContentList, tid, pid);

		auto& post = posts[iPost];
		post.tid = tid;
		post.authorID = PostuserList[userIndex[author_id]].id;
		post.author = PostuserList[userIndex[author_id]].name;
		post.authorShowName = PostuserList[userIndex[author_id]].ShowName;
		post.authorPortraitUrl = PostuserList[userIndex[author_id]].PortraitUrl;
		post.pid = pid;
		post.floor = floor;
		post.authorLevel = Int2CString(PostuserList[userIndex[author_id]].level);
		post.timestamp = time;
		post.rawData = _T("");
		post.content = content;
		post.customState = customState;
		if (post.authorID == threadUid)
			post.isTidAuthor = TRUE;
		
		// 特殊贴解析
		if (post.floor == _T("1")) { // 仅需要处理1楼
			// 投票贴判断
			PollInfo* pbPollInfo = threadInfo->mutable_poll_info();
			::google::protobuf::RepeatedPtrField<PollInfo_PollOption>* pbPollOptions;
			pbPollOptions = pbPollInfo->mutable_options();
			// 分享贴判断
			const int is_share_thread = threadInfo->is_share_thread();
			if (is_share_thread != 0) {
				ThreadInfo_OriginThreadInfo* pbOriThreadInfo = threadInfo->mutable_origin_thread_info();
				// 分享贴强特征
				const CString oriTid = strUTF82W(pbOriThreadInfo->tid());
				CString STR_SHARE_THREAD;
				STR_SHARE_THREAD.Format(_T(R"(<a href="http://tieba.baidu.com/p/%s"  target="_blank"> 点击查看</a>)"), oriTid);
				post.content += STR_SHARE_THREAD;
				// 原帖标题
				const CString oriTitle = strUTF82W(pbOriThreadInfo->title());
				post.content += _T("\r\n\r\n") + oriTitle;
				// 是否被删帖
				const int ori_is_deleted = pbOriThreadInfo->is_deleted();
				if (ori_is_deleted == 0) {
					::google::protobuf::RepeatedPtrField<PbContent>* oriPbContent;
					oriPbContent = pbOriThreadInfo->mutable_content();
					post.content += _T("\r\n") + decodeContent(oriPbContent);
				}
				// 更新投票贴情况
				pbPollInfo = pbOriThreadInfo->mutable_poll_info();
				pbPollOptions = pbPollInfo->mutable_options();
			}
			// 投票贴解析
			if (pbPollOptions->size() > 0) {
				const CString pollTitle = strUTF82W(pbPollInfo->title());
				CString pollOption = _T("");
				for (auto& rawOption = pbPollOptions->begin(); rawOption != pbPollOptions->end(); ++rawOption)
				{
					pollOption += strUTF82W(rawOption->text());
				}
				// 投票贴强特征
				post.content += STR_THREAD_VOTE;
				// 投票内容标题
				post.content += _T("\r\n");
				post.content += pollTitle;
				// 投票内容
				post.content += _T("\r\n");
				post.content += pollOption;
			}
			// 个人隐私设置解析
			if (PostuserList[userIndex[author_id]].reply_type == 5) {
				// == 1 仅粉丝可回复模式
				// 添加强特征   
				post.content += STR_FAN_ONLY;
			}
			else if (PostuserList[userIndex[author_id]].reply_type == 6) {
				// == 1 仅楼主关注可回复模式
				// 添加强特征   
				post.content += STR_ONLY_F;
			}
		}
		
		// 小尾巴
		Post_SignatureData* pbSignatureData = rawPost->mutable_signature();
		::google::protobuf::RepeatedPtrField<Post_SignatureData_SignatureContent>* pbSignatureContent;
		pbSignatureContent = pbSignatureData->mutable_content();
		if (pbSignatureContent->size() > 0) {
			CString signStr = _T("");
			for (auto& rawCotent = pbSignatureContent->begin(); rawCotent != pbSignatureContent->end(); ++rawCotent)
			{
				signStr += strUTF82W(rawCotent->text());
			}
			post.content += _T("\r\n\r\n");
			post.content += signStr;
		}

		// 楼中楼信息处理
		UINT32 sub_post_number = rawPost->sub_post_number(); // 显示总的楼中楼，暂时没用到
		Post_SubPost* post_SubPost = rawPost->mutable_sub_post_list();
		::google::protobuf::RepeatedPtrField<SubPostList>* pbSubPostList;
		pbSubPostList = post_SubPost->mutable_sub_post_list();
		if (pbSubPostList->size() > 0) {
			tLzl += pbSubPostList->size();
			for (auto& rawLzl = pbSubPostList->begin(); rawLzl != pbSubPostList->end(); ++rawLzl)
			{	
				LzlInfo lzl = LzlInfo();
				decodeLzl(*rawLzl, lzl, floor, PostuserList, userIndex, tid);
				if (lzl.authorID == threadUid)
					lzl.isTidAuthor = TRUE;
				lzls.push_back(lzl);
			}
		}
		// 附加获取完整前10楼 楼中楼？
		if (aced_lzl_info && use_addl_lzl_info && jsonLzl.size() && sub_post_number > 0) {
			for (auto& t : jsonLzl) {
				if (t.floor.Compare(post.pid) == 0) {
					t.floor = floor;
					// 判断是附加，还是插入
					bool is_push_back = true;
					for (auto& i : lzls) {
						if (t.cid.Compare(i.cid) == 0) {
							// 找到对应楼中楼，进行合并
							i.content += _T("\r\n\r\n") + t.content;
							is_push_back = false;
							break;
						}
					}
					if (is_push_back) {
						if (t.authorID == threadUid)
							t.isTidAuthor = TRUE;
						lzls.push_back(t);
					}
				}
			}
		}
		++iPost;
	}

	// 附加信息
	if (addition != NULL)
	{
		CString tmp;
		tmp.Format(_T("%d"), pbPage->total_page());
		addition->src = src;
		addition->srcA = srcA;
		addition->fid = fid;
		addition->pageCount = tmp;
	}

	return GET_POSTS_SUCCESS;
}
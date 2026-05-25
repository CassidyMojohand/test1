LVPPHttpI_URL_POST_BLOCK
HFileUpTrapper::pins()->TrapperWithHeaderOld(http_param, buf_content, buf_len, true, vt_str_file);

http_param, buf_content, buf_len  重要参数
http_param 来自GetHttpParam
buf_content 来自HProtocolCallback::GetPFStream

最终是ProcessHttpFile处理文件 他根据HFileUpTrapper::pins()->TrapperWithHeaderOld 判断是否有需要处理的文件
TrapperWithHeaderOld的vt_str_file参数怎么来的？
->1. 复杂 暂时不考虑         
2.	for (size_t i = 0; i < m_vt_file_alldata_urllist.size(); ++i)
	{
		HString  str_url = http_param.http_param_.str_url;
		if (str_url.is_in_pattern(m_vt_file_alldata_urllist[i], false))
		{
			//先尝试一次看看能不能拿到
			LVImportFileNode  node;
			HAllImportFileShareMemCallBack::pins()->get_node_safe(L"", i_len_req_body, HString(L""),
				HAllImportFileShareMemCallBack::UPLOAD_FILE_FILE, node);
			if (node.str_file_path.not_empty())
			{
				vt_str_file.push_back(node.str_file_path);
			}

			return 0;
		}
	}
2.1 首先 m_vt_file_alldata_urllist 来自哪里？--配置列表lv-csp-webinfo-set-file.arc
文件主要来自m_lt_node   来自注入插件传的

ProcessHttpFull里有两个点调用ProcessHttpFile
第1点：通过注入关联处理的 目前实现不了
第2点：判断boundry
2.1 先判断url是否在肯定无boundry列表里 （上网审计使用了boundary传参数，却不是文件的例外处理
2.2 通过IsAnyHttpFileOn判断HHttpParamWrapper参数 
2.2.1 函数里关联m_lt_policy_ab  -注册的回调 HHttpPolicyArbitrator
const HHttpParamWrapper& http_param （一些url参数）是怎么来的？GetHttpParam 根据id来的-来自 m_map_http_param
m_map_http_param来自 1. HttpRequestFirstPart  2. SetHttpParam<-DataAvaliable (GetHttpParam失败时调用) 

2.2 就是把参数给各个回调去调用了
IsAnyHttpFileOn -> return GetFuncPoint(http_param)& FUNC_HTTP_FILE; -> GetFuncPoint中遍历 m_queue_func_point
->SubscribeHttpFile->HWebMailPolicyArbitrator::HttpFirst 

2.3 如果 2.2 命中了 if (IsAnyHttpFileOn(http_param)) 从哪里获取文件路径？ 
HFileUpTrapper::pins()->TrapperWithHeaderNew(http_param, out_params, b_need_to_file, str_charset, vt_str_file);
2.4 TrapperWithHeaderNew
2.4.1 put
2.4.2 other  直接找out_params中的str_file
out_params 从哪来->ProcessHttpFull 的 HPartTransformer::TransformPart(http_param, buf_content, buf_len, out_params);

3. TransformPart:
3.1 i_type == CONTENT_TYPE_MULTIPART str_file来源于http_param.http_param_.str_head -> http_param

buf_content 比较重要  来自 HProtocolCallback::GetPFStream(pObject, ProtocolFilters::HS_CONTENT, buf_content, buf_len);

3.2 other 


HTTP协议中:ml-search-more[Content-Type]头部字段用于声明传输数据的媒体类型，常见取值包括以下类型：

文本类型
‌text/plain‌：普通文本，不包含格式（默认类型）。 ‌
‌text/html‌：HTML文档。 ‌
‌text/css‌：CSS样式表。 ‌
‌text/javascript‌：JavaScript代码。 ‌
JSON与XML格式
‌application/json‌：JSON数据（RESTful 常用格式）。 ‌
‌application/xml‌：XML数据。 ‌
表单提交类型
‌application/x-www-form-urlencoded‌：URL编码的表单数据（默认提交方式）。 
‌multipart/form-data‌：支持文件上传（如图片、视频等二进制数据）。 ‌
其他类型
‌image/jpeg‌：JPEG图片。 ‌
‌image/png‌：PNG图片。 ‌
‌audio/mpeg‌：MP3音频文件。 ‌
‌video/mp4‌：MP4视频文件。 ‌

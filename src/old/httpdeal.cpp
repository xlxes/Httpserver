#include "httpdeal.h"
#include <iostream>

using std::to_string;

/*
@brief 解析http协议
@input param : 
    http_data 所有http的信息
    htto_request 用于存放解析后的数据
*/
void deal_request(char *http_data, struct http_request *request)
{
    string httpdata = http_data; //新建string类型保存原数据
    vector<string> lines;        //将string按每一行分割
    StringSplit::Split(httpdata, lines, "\r\n");
    if (lines.size() < 1 || lines[0].empty())
        return;
    //解析第一行请求行
    vector<string> chunk;
    StringSplit::Split(lines[0], chunk, " ");
    if (chunk.size() < 3) //至少有三个参数
        return;
    // std::cout<<"运行了此处"<<request->method<<std::endl;
    request->method = chunk[0];
    request->url = chunk[1];
    request->version = chunk[2];

    //解析第二行后的key和value,不能用map存储，防止排序
    int i = 1;
    vector<string> header;
    for (; lines[i] != "\r\n"; ++i)
    {
        StringSplit::Split(lines[i], header, ":"); //按照:分割
        if (header.size() == 2)
        {
            request->headers.insert(pair<string, string>(header[0], header[1]));
        }
        header.clear(); //清空
    }

    //解析body,此时lines[i]="\r\n"
    for (i = i + 1; i < lines.size(); i++)
    {
        request->body += lines[i];
        if (i != lines.size() - 1)
            request->body += "\r\n"; //补上每行的换行符
    }
}

/*
@brief 清空响应http的结构体
*/
void initHttpResponse(struct http_response *response)
{
    response->version.clear();
    response->desc.clear();
    response->code.clear();
    response->body.clear();
    response->headers.clear();
}

void doRespone(string &httpresponse)
{
    //填写http信息结构体
    struct http_response response;
    initHttpResponse(&response);
    response.version = "HTTP/1.1";
    response.code = "200";
    response.desc = "OK";
    string content = "<html>hello everyone</html>";
    string content_len = to_string(content.size());
    response.headers.insert(pair<string, string>("Content-Length", content_len));
    response.body = content;

    //将所有信息转为一个string类型
    httpresponse = response.version + " " + response.code + " " + response.desc + "\r\n";
    for (auto it : response.headers)
    {
        httpresponse = httpresponse + it.first + ":" + it.second + "\r\n";
    }
    httpresponse = httpresponse + "\r\n" + response.body;
}

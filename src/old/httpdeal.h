#include <vector>
#include <string>
#include <string.h>
#include <unordered_map>
#include "stringsplit.h"

using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

//记录接受的http
struct http_request
{
    string method;
    string url;
    string version;
    unordered_map<string, string> headers;
    string body;
};

//发送的http结构
struct http_response
{
    string version;
    string code; //返回状态码
    string desc; //描述
    unordered_map<string, string> headers;
    string body;
};

//解析http请求
void deal_request(char *http_data, struct http_request *request);
//初始话响应
void initHttpResponse(struct http_response *response);
//响应http请求
void doRespone(string& httpresponse);


#include <string.h>
#include "stringsplit.h"


/*
根据输入的delimiter分隔符，将str分割，结果存在数组v中
如果第一个即为分割符号或者连续两个分隔符，存入\r\n，此处是为了分割出header和body间的那个空行
分隔符默认"|"
*/
void stringsplit::split(const string &str, vector<string> &v, const char *delimiter)
{
    if (delimiter == NULL || str.empty())
        return;
    string buf = str;
    size_t pos = string::npos; //npos表示为未找到时的返回值
    string substring;
    int del_len = strlen(delimiter);
    while (1)
    {
        pos = buf.find(delimiter);
        if (pos != string::npos) //找到分隔符
        {
            substring = buf.substr(0, pos); //分割前半部分
            if (!substring.empty())
                v.push_back(substring);
            else
                v.push_back("\r\n");
            buf = buf.substr(pos + del_len); //更新buf;
        }
        else //处理最后一个字符串
        {
            if (!buf.empty())
                v.push_back(buf);
            break;
        }
    }
}

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h> //用于解析命令行参数
#include <vector>
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>


#define LOGGER_WARN
#define LOGGER_INFO
//#define LOGGER_DEBUG
#include "logger.h"

using boost::property_tree::read_xml;   
using boost::property_tree::xml_parser::trim_whitespace;
using boost::property_tree::ptree;
using boost::property_tree::ptree_bad_path;
using boost::property_tree::ptree_error;
using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace httpserver
{
const string SERVER_NAME = "a http server of xlxes";

//固定默认参数设置
const int LISTEN_PORT = 8080;
const int TIME_OUT = 10;
const int MAX_CLIENT = 10000;
const int INIT_WORK_NUM = 5;
const int MAX_WORK_NUM = 100000;

//声明一个参数类,包含固定必备的参数
class Parameters
{
private:
    bool loadconfig();
    string cgi_root_;
    string doc_root_;
    string default_file_;
    string config_file_;
    int listen_port_;
    int max_client_;
    int time_out_;
    int init_work_num_;
    int max_work_num_;
    vector<string> file_path_lists_; //默认由doc_root_ + default_file_组成
    ptree xml_tree_;

public:
    Parameters(int argc, char *argv[]); //构造函数声明
    /*获取成员函数*/
    int get_listen_port_() { return listen_port_; }
    int get_init_work_num_() { return init_work_num_; }
    int get_max_work_num_() { return max_work_num_; }
    string& get_defaul_file_() { return default_file_; }
    string& get_config_file_() { return config_file_; }
    string& get_doc_root_() { return doc_root_; }
    string& get_cgi_root_() { return cgi_root_; }
    vector<string>& get_file_path_lists_() { return file_path_lists_; }
    //显示函数
    void displayConfig();
};

} // namespace httpserver

#endif //PARAMETER_H_
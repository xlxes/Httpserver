#include "parameters.h"

//全项目的命名空间即为httpserver
namespace httpserver
{
//短命令表示可以接受的命令参数
static const char *short_cmd_opt = "c:o:d:f:l:a:t:i:m:h"; //后面带:表示带参数
//长参数名*参数个数*返回标志位（为0返回val）*val
static struct option long_cmd_opt[] = {
    {"cgioot", required_argument, 0, 'c'},
    {"docroot", required_argument, 0, 'o'},
    {"defaultfile", required_argument, 0, 'd'},
    {"configfile", required_argument, 0, 'f'},
    {"listenport", required_argument, 0, 'l'},
    {"maxclient", required_argument, 0, 'a'},
    {"timeout", required_argument, 0, 't'},
    {"initworknum", required_argument, 0, 'i'},
    {"maxworknum", required_argument, 0, 'm'},
    {"help", no_argument, 0, 'h'},
};

//构造函数定义,给定默认值
Parameters::Parameters(int argc, char *argv[])
    : cgi_root_("../doc/cgi"),
      doc_root_("../doc"),
      default_file_("/index.html"),
      config_file_("../doc/config.xml"),
      listen_port_(LISTEN_PORT),
      max_client_(MAX_CLIENT),
      time_out_(TIME_OUT),
      init_work_num_(INIT_WORK_NUM),
      max_work_num_(MAX_WORK_NUM)
{
    bool isload = loadconfig(); //加载xml配置
    if (isload == false)
        WARN("unable to load config.xml\n");
    if (argc >= 2 && argv != nullptr) //读取命令行配置
    {
        int c = -1;
        //getopt函数帮助解析命令行参数，返回c为val即（c:o:d:f:l:a:t:i:m:h），而对应的值存在optarg中
        //全部分析结束返回-1
        while ((c = getopt_long(argc, argv, short_cmd_opt, long_cmd_opt, NULL)) != -1)
        {
            switch (c)
            {
            case 'c':
                cout << "set cgiroot:" << optarg << endl;
                cgi_root_ = optarg;
                break;
            case 'o':
                cout << "set docroot:" << optarg << endl;
                doc_root_ = optarg;
                break;
            case 'd':
                cout << "set defaultfile:" << optarg << endl;
                default_file_ = optarg;
                break;
            case 'f':
                cout << "set configfile:" << optarg << endl;
                config_file_ = optarg;
                break;
            case 'l':
                cout << "set listenport:" << optarg << endl;
                listen_port_ = atoi(optarg);
                break;
            case 'a':
                cout << "set maxclient:" << optarg << endl;
                max_client_ = atoi(optarg);
                break;
            case 't':
                cout << "set timeout:" << optarg << endl;
                time_out_ = atoi(optarg);
                break;
            case 'i':
                cout << "set initworknum:" << optarg << endl;
                init_work_num_ = atoi(optarg);
                break;
            case 'm':
                cout << "set maxworknum:" << optarg << endl;
                max_work_num_ = atoi(optarg);
                break;
            case 'h':
                cout << "cgioot ------- c" << endl
                     << "docroot ------ o" << endl
                     << "defaultfile -- d" << endl
                     << "configfile --- f" << endl
                     << "listenport --- l" << endl
                     << "maxclient ---- a" << endl
                     << "timeout ------ t" << endl
                     << "initworknum -- i" << endl
                     << "maxworknum --- m" << endl;
                cout << "参数命令表如上" << endl;
                exit(0); //结束程序
            }
        }
    }
    string path = doc_root_ + default_file_; //合成路径
    DEBUG("file path is:%s\n", path.data());
    file_path_lists_.push_back(path);
}

//显示基本信息
void Parameters::displayConfig()
{
    cout << "cgioot      is: " << cgi_root_ << endl;
    cout << "docroot     is: " << doc_root_ << endl;
    cout << "defaultfile is: " << default_file_ << endl;
    cout << "configfile  is: " << config_file_ << endl;
    cout << "listenport  is: " << listen_port_ << endl;
    cout << "maxclient   is: " << max_work_num_ << endl;
    cout << "timeout     is: " << time_out_ << endl;
    cout << "initworknum is: " << init_work_num_ << endl;
    cout << "maxworknum  is: " << max_work_num_ << endl;
}
/**
*  @brief 从xml文件中加载基本信息
*/
bool Parameters::loadconfig()
{
    try
    {
        //读取xml文件到xml_tree_中，同时去除空格
        //..表示当前文件夹上一层
        read_xml("../doc/config.xml", xml_tree_, trim_whitespace);
    }
    catch (const ptree_error &e)
    {
        WARN("read xml error:%s\n", e.what());
        return false;
    }
    try
    {
        //尝试读取监听端口
        int listen_port = xml_tree_.get_child("root.httpserver.listen_port").get<int>("<xmlattr>.value");
        listen_port_ = listen_port;
    }
    catch (const ptree_error &e)
    {
        WARN("read xml listen port error:%s\n", e.what());
    }
    try
    {
        int init_work_num = xml_tree_.get_child("root.httpserver.init_work_num").get<int>("<xmlattr>.value");
        init_work_num_ = init_work_num;
    }
    catch (const ptree_error &e)
    {
        WARN("read xml init work num error:%s\n", e.what());
    }
    try
    {
        int max_work_num = xml_tree_.get_child("root.httpserver.max_work_num").get<int>("<xmlattr>.value");
        max_work_num_ = max_work_num;
    }
    catch (const ptree_error &e)
    {
        WARN("read xml init work num error:%s\n", e.what());
    }
    try
    {
        //保证最后一个为终止符
        doc_root_ = xml_tree_.get_child("root.httpserver.document_root").get<string>("<xmlattr>.value");
    }
    catch (const ptree_error &e)
    {
        WARN("read xml doc root error:%s\n", e.what());
    }
    try
    {
        default_file_ = xml_tree_.get_child("root.httpserver.default_file").get<string>("<xmlattr>.value");
    }
    catch (const ptree_error &e)
    {
        WARN("read xml default file error:%s\n", e.what());
    }
    return true;
}

} // namespace httpserver
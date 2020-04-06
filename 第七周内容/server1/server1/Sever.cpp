#include"Server.h"

SOCKET Server::s_server;
SOCKET Server::s_accept;
//服务端地址客户端地址
SOCKADDR_IN Server::server_addr;
SOCKADDR_IN Server::accept_addr;

string Server::file_path;


void Server::initialization()
{
    //初始化套接字库
    WORD w_req = MAKEWORD(2, 2);//版本号
    WSADATA wsadata;
    int err;
    err = WSAStartup(w_req, &wsadata);
    if (err != 0) {
        cout << "初始化套接字库失败！" << endl;
    }
    else {
        cout << "初始化套接字库成功！" << endl;
    }
    //检测版本号
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        cout << "套接字库版本号不符！" << endl;
        WSACleanup();
    }
    else {
        cout << "套接字库版本正确！" << endl;
    }
}

int Server::get_operate_number(const char* buf)
{
    return (buf[0] - 48) * 100 + (buf[1] - 48) * 10 + (buf[2] - 48);
}

bool Server::get_file_from_host(string file_name)
{
    //cout << "flag" << endl;
    int recv_len;
    // char buf[1025];
    char buf[1027] = { 0 };
    int send_len;
    ofstream outfile(file_name, ios::out | ios::binary);
    if (!outfile)
    {
        cout << "找不到文件" << file_name << endl;
        return false;
    }
    while (true)
    {
        recv_len = recv(s_accept, buf, BUFFER_SIZE + HEAD_SIZE, 0);
        //cout << buf << endl;
        if (get_operate_number(buf) == 2)
        {
            cout << "读到传输结束标志，退出" << endl;
            send(s_accept, "repeat", 7, 0);
            return true;
        }
        if (recv_len < 0)
        {
            cout << "接受失败" << endl;
            return false;
        }
        /*if (strcmp(buf, "file_transmission_end") == 0)
        {
            cout << "文件接收成功" << endl;
            outfile.close();
            return true;
            break;
        }*/
        char* infor_p = buf + 3;
        outfile.write(infor_p, recv_len - 3);
        send(s_accept, "repeat", 7, 0);
    }
    cout << "没有收到传输结束标志" << endl;
    outfile.close();
    return false;
}

void Server::accept_files_from_the_host(string file_path)
{
    int recv_len;
    char buf[1027] = { 0 };
    //string file_path = "ooxx/";
    if (_access(file_path.c_str(), 0) == -1)	//如果文件夹不存在
    {
        cout << "你要储存的文件夹不存在" << endl;
        return;
    }
    char* infor_p;
    while (true)
    {
        //char* infor_p;
        recv_len = recv(s_accept, buf, BUFFER_SIZE + HEAD_SIZE, 0);
        if (recv_len < 0)
        {
            cout << "接受失败" << endl;
            return;
        }
        switch (get_operate_number(buf))
        {
        case 0:
        {
            cout << "OK,接受完全部信息" << endl;
            return;
            break;
        }
        case 1:
        {
            cout << "你不应该出现在这里1" << endl;
            break;
        }
        case 2:
        {
            cout << "你也不应该出现在这里2" << endl;
            break;
        }
        case 3:
        {
            infor_p = buf + 3;
            string prefix = file_path + infor_p;
            if (_access(prefix.c_str(), 0) == -1)	//如果文件夹不存在
                _mkdir(prefix.c_str());
            if (send(s_accept, "repeat", 7, 0) < 0)
            {
                cout << "发送回复失败" << endl;
            }
            break;
        }
        case 4:
        {
            infor_p = buf + 3;
            string file_name = file_path + infor_p;
            cout << "对文件" << file_name << "进行写入" << endl;
            if (send(s_accept, "repeat", 7, 0) < 0)
            {
                cout << "发送回复失败" << endl;
            }
            get_file_from_host(file_name);
            cout << "写入结束" << endl;
            break;
        }
        default:
        {
            cout << "那必然是出错了，还没有这个操作数呢！" << endl;
        }
        }

    }
}

void Server::__start()
{
    int port;
    string server_ip;
    int len;
    initialization();

    cout << "请输入【文件地址】 【ip】 【端口号】中间以一个空格隔开" << endl;
    cin >> file_path >> server_ip >> port;
    cout << "服务器已经启动" << endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip.c_str()); //htonl(INADDR_ANY);192.168.31.212
    server_addr.sin_port = htons(port);	//8307
    //创建套接字	
    s_server = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        cout << "套接字绑定失败！" << endl;		WSACleanup();
    }
    else
    {
        cout << "套接字绑定成功！" << endl;
    }
    //设置套接字为监听状态	
    if (listen(s_server, SOMAXCONN) < 0)
    {
        cout << "设置监听状态失败！" << endl;
        WSACleanup();
    }
    else
    {
        cout << "设置监听状态成功！" << endl;
    }
    cout << "服务端正在监听连接，请稍候...." << endl;
    //接受连接请求
    len = sizeof(SOCKADDR);
    s_accept = accept(s_server, (SOCKADDR*)&accept_addr, &len);
    if (s_accept == SOCKET_ERROR)
    {
        cout << "连接失败！" << endl;
        WSACleanup();
        return;
    }
    cout << "连接建立，准备接受数据" << endl;
}

void Server::__trans()
{
    int send_len = 0;
    int recv_len = 0;
    //int len = 0;
    char recv_buf[100];

    if (_access(file_path.c_str(), 0) == -1)	//如果文件夹不存在
    {
        cout << "不存在该文件夹或者文件" << endl;
        exit(-1);
    }

    if (file_path[file_path.length() - 1] != '/' && file_path[file_path.length() - 1] != '\\')
    {
        file_path += '/';
    }
    accept_files_from_the_host(file_path);
    cout << "接收完毕" << endl;
}

void Server::__end()
{
    //关闭套接字
    closesocket(s_server);	closesocket(s_accept);
    //释放DLL资源
    WSACleanup();
}
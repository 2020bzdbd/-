#include"Client.h"

SOCKET Client::s_server;
//SOCKET Client::s_accept;
//服务端地址客户端地址
SOCKADDR_IN Client::server_addr;
//SOCKADDR_IN Client::accept_addr;
string Client::file_path;

void Client::initialization()
{
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号	
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0)
	{
		cout << "初始化套接字库失败！" << endl;
	}
	else
	{
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2)
	{
		cout << "套接字库版本号不符！" << endl;		WSACleanup();
	}
	else
	{
		cout << "套接字库版本正确！" << endl;
	}
	//填充服务端地址信息
}

bool Client::file_transmision(string file_name)
{
	int recv_len;
	char recv_buf[100];
	int readlen;
	//char buf[1025];
	char buf[1027] = { 0 };
	int send_len;
	ifstream in_file(file_name, ios::in | ios::binary);
	//in_file >> noskipws;
	if (!in_file.is_open())
	{
		cout << "找不到文件" << file_name << endl;
		in_file.close();
		return false;
	}
	while (!in_file.eof())
	{
		cout << 1;
		buf[0] = '0'; buf[1] = '0'; buf[2] = '1';
		char* infor_p = buf + 3;
		in_file.read(infor_p, BUFFER_SIZE);
		//cout << buf << endl;
		readlen = in_file.gcount();
		send_len = send(s_server, buf, readlen + HEAD_SIZE, 0);
		if (send_len < 0)
		{
			cout << "传输失败" << endl;
			in_file.close();
			return false;
		}
		if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
		{
			cout << "出错了,没有正常收到回复" << endl;
		}
	}
	//cout << "文件 " << file_name << " 传输完成"<<endl;
	in_file.close();
	//buf[0] = '0'; buf[1] = '0'; buf[2] = '2';
	send_len = send(s_server, "002file_transmission_end", 25, 0);
	cout << endl;
	if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
	{
		cout << "出错了,没有正常收到回复" << endl;
	}
	return true;
}

void Client::sent_create_file_messege(string file_path, string mother_file)
{
	for (int i = 0; i <= file_path.length() - mother_file.length(); i++)
	{
		if (file_path.substr(i, mother_file.length()) == mother_file)
		{
			file_path = file_path.substr(i);
			break;
		}
	}

	int send_len;
	if (send_len = send(s_server, ("003" + file_path).c_str(), 200, 0))
	{
		cout << "发送创建文件夹成功" << file_path << endl;
	}
	else
	{
		cout << "发送创建文件夹指令失败" << file_path << endl;
	}

}

void Client::sent_file_name(string file_name, string mother_file)
{
	for (int i = 0; i < file_name.length() - mother_file.length(); i++)
	{
		if (file_name.substr(i, mother_file.length()) == mother_file)
		{
			file_name = file_name.substr(i);
			break;
		}
	}

	int send_len;
	if (send_len = send(s_server, ("004" + file_name).c_str(), 200, 0))
	{
		cout << "发送创建文件成功" << file_name << endl;
	}
	else
	{
		cout << "发送创建文件夹指令失败" << file_name << endl;
	}
}

void Client::sent_end_transmission()
{
	int send_len;
	string temp = "file_transmission_end";
	if (send_len = send(s_server, ("000" + temp).c_str(), 200, 0))
	{
		cout << "已经传输结束传输指令" << endl;
	}
	else
	{
		cout << "传输结束指令出错了" << endl;
	}
}

void Client::getAllFiles(string path, vector<string>& files, vector<int>& is_file_packet, int hierarchy) {
	//文件句柄
	long hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本

	if (hierarchy == 0)
	{
		if (_findfirst(p.assign(path).c_str(), &fileinfo) != -1)
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				cout << "源文件是文件夹" << endl;
				files.push_back(p.assign(path));
				is_file_packet.push_back(1);
			}
			else
			{
				cout << "源文件就是一个文件" << endl;
				files.push_back(p.assign(path));
				is_file_packet.push_back(0);
			}
		}
	}

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
		do {
			//cout << fileinfo.name << endl;
			if ((fileinfo.attrib & _A_SUBDIR)) {  //比较文件类型是否是文件夹
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					files.push_back(p.assign(path).append("/").append(fileinfo.name));
					is_file_packet.push_back(1);
					getAllFiles(p.assign(path).append("/").append(fileinfo.name), files, is_file_packet, hierarchy + 1);

				}

			}
			else {
				files.push_back(p.assign(path).append("/").append(fileinfo.name));
				is_file_packet.push_back(0);
			}

		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);

	}

}

void Client::__connect()
{
	string server_ip;
	int port;
	initialization();
	//填充服务端信息

	cout << "请输入【文件地址】 【ip】 【端口号】中间以一个空格隔开" << endl;
	cin >> file_path >> server_ip >> port;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip.c_str()); //192.168.20.1     127.0.0.1
	server_addr.sin_port = htons(port);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "服务器连接失败！" << endl;
		WSACleanup();
		exit(-1);
	}
	else {
		cout << "服务器连接成功！" << endl;
	}

	//发送,接收数据


	/*if (file_path[file_path.length() - 1] != '/' && file_path[file_path.length() - 1] != '\\')
	{
		file_path += '/';
	}*/
	if (_access(file_path.c_str(), 0) == -1)	//如果文件夹不存在
	{
		cout << "不存在该文件夹或者文件" << endl;
		exit(-1);
	}
}

void Client::__trans()
{
	vector<string> files;
	vector<int> is_file_packet;
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	char recv_buf[100];
	int file_name_head = 0, file_name_end = file_path.length() - 1;
	string mother_file;
	int mother_file_length;
	for (int j = file_path.length() - 1; j >= 0; j--)
	{
		if (file_path[j] != '\\' && file_path[j] != '/')
		{
			file_name_end = j;
			break;
		}
	}
	for (int j = file_name_end; j >= 0; j--)
	{
		if (file_path[j] == '\\' || file_path[j] == '/')
		{
			file_name_head = j + 1;
			break;
		}
	}
	mother_file = file_path.substr(file_name_head, file_name_end - file_name_head + 1);//获取mother_file,哪怕你填的是绝对路径,也可以正确发送相对路径
	cout << "mother file is " << mother_file << endl;


	getAllFiles(file_path, files, is_file_packet, 0);

	cout << files.size() << " " << is_file_packet.size() << endl;
	for (int i = 0; i < files.size(); i++)
	{
		if (is_file_packet[i])
		{
			sent_create_file_messege(files[i], mother_file);
			if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
			{
				cout << "出错了,没有正常收到回复" << endl;
			}
		}
		else
		{
			sent_file_name(files[i], mother_file);
			if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
			{
				cout << "出错了,没有正常收到回复" << endl;
			}
			file_transmision(files[i]);
		}
	}
	sent_end_transmission();
	cout << "文件传输完成" << endl;
}

void Client::__exit()
{
	//关闭套接字
	closesocket(s_server);	//closesocket(s_server);
	//释放DLL资源
	WSACleanup();
}

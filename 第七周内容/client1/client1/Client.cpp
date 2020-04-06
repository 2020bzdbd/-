#include"Client.h"

SOCKET Client::s_server;
//SOCKET Client::s_accept;
//����˵�ַ�ͻ��˵�ַ
SOCKADDR_IN Client::server_addr;
//SOCKADDR_IN Client::accept_addr;
string Client::file_path;

void Client::initialization()
{
	//��ʼ���׽��ֿ�
	WORD w_req = MAKEWORD(2, 2);//�汾��	
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0)
	{
		cout << "��ʼ���׽��ֿ�ʧ�ܣ�" << endl;
	}
	else
	{
		cout << "��ʼ���׽��ֿ�ɹ���" << endl;
	}
	//���汾��
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2)
	{
		cout << "�׽��ֿ�汾�Ų�����" << endl;		WSACleanup();
	}
	else
	{
		cout << "�׽��ֿ�汾��ȷ��" << endl;
	}
	//������˵�ַ��Ϣ
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
		cout << "�Ҳ����ļ�" << file_name << endl;
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
			cout << "����ʧ��" << endl;
			in_file.close();
			return false;
		}
		if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
		{
			cout << "������,û�������յ��ظ�" << endl;
		}
	}
	//cout << "�ļ� " << file_name << " �������"<<endl;
	in_file.close();
	//buf[0] = '0'; buf[1] = '0'; buf[2] = '2';
	send_len = send(s_server, "002file_transmission_end", 25, 0);
	cout << endl;
	if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
	{
		cout << "������,û�������յ��ظ�" << endl;
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
		cout << "���ʹ����ļ��гɹ�" << file_path << endl;
	}
	else
	{
		cout << "���ʹ����ļ���ָ��ʧ��" << file_path << endl;
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
		cout << "���ʹ����ļ��ɹ�" << file_name << endl;
	}
	else
	{
		cout << "���ʹ����ļ���ָ��ʧ��" << file_name << endl;
	}
}

void Client::sent_end_transmission()
{
	int send_len;
	string temp = "file_transmission_end";
	if (send_len = send(s_server, ("000" + temp).c_str(), 200, 0))
	{
		cout << "�Ѿ������������ָ��" << endl;
	}
	else
	{
		cout << "�������ָ�������" << endl;
	}
}

void Client::getAllFiles(string path, vector<string>& files, vector<int>& is_file_packet, int hierarchy) {
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;  //�����õ��ļ���Ϣ��ȡ�ṹ
	string p;  //string�������˼��һ����ֵ����:assign()���кܶ����ذ汾

	if (hierarchy == 0)
	{
		if (_findfirst(p.assign(path).c_str(), &fileinfo) != -1)
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				cout << "Դ�ļ����ļ���" << endl;
				files.push_back(p.assign(path));
				is_file_packet.push_back(1);
			}
			else
			{
				cout << "Դ�ļ�����һ���ļ�" << endl;
				files.push_back(p.assign(path));
				is_file_packet.push_back(0);
			}
		}
	}

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
		do {
			//cout << fileinfo.name << endl;
			if ((fileinfo.attrib & _A_SUBDIR)) {  //�Ƚ��ļ������Ƿ����ļ���
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

		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
		_findclose(hFile);

	}

}

void Client::__connect()
{
	string server_ip;
	int port;
	initialization();
	//���������Ϣ

	cout << "�����롾�ļ���ַ�� ��ip�� ���˿ںš��м���һ���ո����" << endl;
	cin >> file_path >> server_ip >> port;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip.c_str()); //192.168.20.1     127.0.0.1
	server_addr.sin_port = htons(port);
	//�����׽���
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "����������ʧ�ܣ�" << endl;
		WSACleanup();
		exit(-1);
	}
	else {
		cout << "���������ӳɹ���" << endl;
	}

	//����,��������


	/*if (file_path[file_path.length() - 1] != '/' && file_path[file_path.length() - 1] != '\\')
	{
		file_path += '/';
	}*/
	if (_access(file_path.c_str(), 0) == -1)	//����ļ��в�����
	{
		cout << "�����ڸ��ļ��л����ļ�" << endl;
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
	mother_file = file_path.substr(file_name_head, file_name_end - file_name_head + 1);//��ȡmother_file,����������Ǿ���·��,Ҳ������ȷ�������·��
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
				cout << "������,û�������յ��ظ�" << endl;
			}
		}
		else
		{
			sent_file_name(files[i], mother_file);
			if (recv_len = recv(s_server, recv_buf, 100, 0) < 0)
			{
				cout << "������,û�������յ��ظ�" << endl;
			}
			file_transmision(files[i]);
		}
	}
	sent_end_transmission();
	cout << "�ļ��������" << endl;
}

void Client::__exit()
{
	//�ر��׽���
	closesocket(s_server);	//closesocket(s_server);
	//�ͷ�DLL��Դ
	WSACleanup();
}

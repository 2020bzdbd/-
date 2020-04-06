#include"Server.h"

SOCKET Server::s_server;
SOCKET Server::s_accept;
//����˵�ַ�ͻ��˵�ַ
SOCKADDR_IN Server::server_addr;
SOCKADDR_IN Server::accept_addr;

string Server::file_path;


void Server::initialization()
{
    //��ʼ���׽��ֿ�
    WORD w_req = MAKEWORD(2, 2);//�汾��
    WSADATA wsadata;
    int err;
    err = WSAStartup(w_req, &wsadata);
    if (err != 0) {
        cout << "��ʼ���׽��ֿ�ʧ�ܣ�" << endl;
    }
    else {
        cout << "��ʼ���׽��ֿ�ɹ���" << endl;
    }
    //���汾��
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        cout << "�׽��ֿ�汾�Ų�����" << endl;
        WSACleanup();
    }
    else {
        cout << "�׽��ֿ�汾��ȷ��" << endl;
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
        cout << "�Ҳ����ļ�" << file_name << endl;
        return false;
    }
    while (true)
    {
        recv_len = recv(s_accept, buf, BUFFER_SIZE + HEAD_SIZE, 0);
        //cout << buf << endl;
        if (get_operate_number(buf) == 2)
        {
            cout << "�������������־���˳�" << endl;
            send(s_accept, "repeat", 7, 0);
            return true;
        }
        if (recv_len < 0)
        {
            cout << "����ʧ��" << endl;
            return false;
        }
        /*if (strcmp(buf, "file_transmission_end") == 0)
        {
            cout << "�ļ����ճɹ�" << endl;
            outfile.close();
            return true;
            break;
        }*/
        char* infor_p = buf + 3;
        outfile.write(infor_p, recv_len - 3);
        send(s_accept, "repeat", 7, 0);
    }
    cout << "û���յ����������־" << endl;
    outfile.close();
    return false;
}

void Server::accept_files_from_the_host(string file_path)
{
    int recv_len;
    char buf[1027] = { 0 };
    //string file_path = "ooxx/";
    if (_access(file_path.c_str(), 0) == -1)	//����ļ��в�����
    {
        cout << "��Ҫ������ļ��в�����" << endl;
        return;
    }
    char* infor_p;
    while (true)
    {
        //char* infor_p;
        recv_len = recv(s_accept, buf, BUFFER_SIZE + HEAD_SIZE, 0);
        if (recv_len < 0)
        {
            cout << "����ʧ��" << endl;
            return;
        }
        switch (get_operate_number(buf))
        {
        case 0:
        {
            cout << "OK,������ȫ����Ϣ" << endl;
            return;
            break;
        }
        case 1:
        {
            cout << "�㲻Ӧ�ó���������1" << endl;
            break;
        }
        case 2:
        {
            cout << "��Ҳ��Ӧ�ó���������2" << endl;
            break;
        }
        case 3:
        {
            infor_p = buf + 3;
            string prefix = file_path + infor_p;
            if (_access(prefix.c_str(), 0) == -1)	//����ļ��в�����
                _mkdir(prefix.c_str());
            if (send(s_accept, "repeat", 7, 0) < 0)
            {
                cout << "���ͻظ�ʧ��" << endl;
            }
            break;
        }
        case 4:
        {
            infor_p = buf + 3;
            string file_name = file_path + infor_p;
            cout << "���ļ�" << file_name << "����д��" << endl;
            if (send(s_accept, "repeat", 7, 0) < 0)
            {
                cout << "���ͻظ�ʧ��" << endl;
            }
            get_file_from_host(file_name);
            cout << "д�����" << endl;
            break;
        }
        default:
        {
            cout << "�Ǳ�Ȼ�ǳ����ˣ���û������������أ�" << endl;
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

    cout << "�����롾�ļ���ַ�� ��ip�� ���˿ںš��м���һ���ո����" << endl;
    cin >> file_path >> server_ip >> port;
    cout << "�������Ѿ�����" << endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip.c_str()); //htonl(INADDR_ANY);192.168.31.212
    server_addr.sin_port = htons(port);	//8307
    //�����׽���	
    s_server = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        cout << "�׽��ְ�ʧ�ܣ�" << endl;		WSACleanup();
    }
    else
    {
        cout << "�׽��ְ󶨳ɹ���" << endl;
    }
    //�����׽���Ϊ����״̬	
    if (listen(s_server, SOMAXCONN) < 0)
    {
        cout << "���ü���״̬ʧ�ܣ�" << endl;
        WSACleanup();
    }
    else
    {
        cout << "���ü���״̬�ɹ���" << endl;
    }
    cout << "��������ڼ������ӣ����Ժ�...." << endl;
    //������������
    len = sizeof(SOCKADDR);
    s_accept = accept(s_server, (SOCKADDR*)&accept_addr, &len);
    if (s_accept == SOCKET_ERROR)
    {
        cout << "����ʧ�ܣ�" << endl;
        WSACleanup();
        return;
    }
    cout << "���ӽ�����׼����������" << endl;
}

void Server::__trans()
{
    int send_len = 0;
    int recv_len = 0;
    //int len = 0;
    char recv_buf[100];

    if (_access(file_path.c_str(), 0) == -1)	//����ļ��в�����
    {
        cout << "�����ڸ��ļ��л����ļ�" << endl;
        exit(-1);
    }

    if (file_path[file_path.length() - 1] != '/' && file_path[file_path.length() - 1] != '\\')
    {
        file_path += '/';
    }
    accept_files_from_the_host(file_path);
    cout << "�������" << endl;
}

void Server::__end()
{
    //�ر��׽���
    closesocket(s_server);	closesocket(s_accept);
    //�ͷ�DLL��Դ
    WSACleanup();
}
#pragma once
#include"public_using_head.h"
class Client
{
private:
	static SOCKET s_server;
	//static SOCKET s_accept;
	//服务端地址客户端地址
	static SOCKADDR_IN server_addr;
	//static SOCKADDR_IN accept_addr;
	static string file_path;
	static void initialization();
	static bool file_transmision(string file_name);
	static void sent_create_file_messege(string file_path, string mother_file);
	static void sent_file_name(string file_name, string mother_file);
	static void sent_end_transmission();
	static void getAllFiles(string path, vector<string>& files, vector<int>& is_file_packet, int hierarchy);
public:
	static void __connect();
	static void __trans();
	static void __exit();
};
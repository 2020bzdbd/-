#pragma once
#include"public_using_head.h"

class Server
{
private:
	//static SOCKET s_client;
	//static SOCKADDR_IN client_addr;
	static SOCKET s_server;
	static SOCKET s_accept;
	//服务端地址客户端地址
	static SOCKADDR_IN server_addr;
	static SOCKADDR_IN accept_addr;
	static string file_path;
	static int get_operate_number(const char* buf);
	static bool get_file_from_host(string file_name);
	static void accept_files_from_the_host(string file_path);
	static void initialization();
public:
	static void __start();
	static void __trans();
	static void __end();
};
#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <string.h>

#ifdef _WIN32
#include <Winsock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#endif // _WIN32

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close
#endif // __linux__

enum sthWrong
{
	SOCKET_SUCCESS,
	SOCKET_CREATE_FAILED,
	SOCKET_WSADATA_INIT_FAILED,
	SOCKET_CONNECT_FAILED,
	SOCKET_BIND_FAILED,
	SOCKET_LISTEN_FAILED,
	SOCKET_ACCEPT_FAILED,
	SOCKET_SEND_FAILED,
	SOCKET_RECEIVE_FAILED,
};

class SocketClass
{
public:
	SocketClass(const char* ip, int port); //如果做server，给"0.0.0.0"就行（其实无所谓）
	~SocketClass();
	int Init_Server();
	int Init_Client();
	int SendCmd(std::string cmd);
	int Receive();
	int ServerDealSocket(std::string cmd);
	int ClientDealSocket(std::string cmd);
private:
	bool isServer; //区别是Server还是Client
	std::string ip;
	int port;
	int sHost; // socket
	int sClient; //socket
	int* sSwitch; //sHost or sClient decided by Server or Client
#ifdef __linux__ 
	unsigned 
#endif 
	int clientAddrLength;
	sockaddr_in servAddr;
	sockaddr_in clientAddr;
	std::string receiveInfo;
	bool keepReceiveFlag = true;
	
};
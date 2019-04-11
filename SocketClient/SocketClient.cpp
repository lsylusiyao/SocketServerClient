#include "SocketClient.h"

#define QUEUE 1
#define BUF_SIZE 1000

using std::cout;
using std::endl;
using std::string;
using std::to_string;
SocketClass::SocketClass(const char* ip, int port)
{
	this->ip = ip;
	this->port = port;
	//Linux为Server, Windows为Client
}

SocketClass::~SocketClass()
{
#ifdef _WIN32
	closesocket(sHost);
	WSACleanup();
#endif // _WIN32

#ifdef __linux__
	close(sHost);
#endif // __linux__
	
}

#ifdef _WIN32
int SocketClass::Init_Server()
{
	//win下初始化Socket
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "Winsock load faild!\n" << endl;
		return SOCKET_WSADATA_INIT_FAILED;
	}
	//初始化赋值Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sHost == INVALID_SOCKET)
	{
		cout << "Socket Failed" << endl;
		WSACleanup();
		return SOCKET_CREATE_FAILED;
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);

	//绑定套接字
	if (bind(sHost, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		cout << "Bind Error" << endl;
		closesocket(sHost);
		WSACleanup();
		return SOCKET_BIND_FAILED;
	}
	isServer = true;

	//监听套接字，数字表示最多能监听客户个数
	if (listen(sHost, QUEUE) == SOCKET_ERROR) {
		cout << "Listen Failed" << endl;
		closesocket(sHost);
		WSACleanup();
		return SOCKET_LISTEN_FAILED;
	}

	clientAddrLength = sizeof(clientAddr);
	sClient = accept(sHost, (sockaddr *)&clientAddr, &clientAddrLength);

	if (sClient == INVALID_SOCKET) {
		cout << "Accept Failed" << endl;
		closesocket(sHost);
		WSACleanup();
		return SOCKET_ACCEPT_FAILED;
	}
	sSwitch = &sClient;
	closesocket(sHost);

	return 0;
}

int SocketClass::Init_Client()
{
	//win下初始化Socket
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "Winsock load faild!\n" << endl;
		return SOCKET_WSADATA_INIT_FAILED;
	}
	//初始化赋值Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons(port);

	isServer = false;
	//连接服务器
	if (connect(sHost, (sockaddr *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		cout << "connect failed!\n" << endl;
		closesocket(sHost);
		WSACleanup();
		return SOCKET_CONNECT_FAILED;
	}
	else
	{
		cout << "Connect succeeded" << endl;
	}
	sSwitch = &sHost;

	return 0;
}
#endif //_Win32

#ifdef __linux__
int SocketClass::Init_Server()
{
	//初始化赋值Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sHost == -1)
	{
		cout << "Socket Failed" << endl;
		return SOCKET_CREATE_FAILED;
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);

	//绑定套接字
	if (bind(sHost, (sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
		cout << "Bind Error" << endl;
		close(sHost);
		return SOCKET_BIND_FAILED;
	}

	isServer = true;
	//监听套接字，数字表示最多能监听客户个数
	if (listen(sHost, QUEUE) == -1) {
		cout << "Listen Failed" << endl;
		close(sHost);
		return SOCKET_LISTEN_FAILED;
	}

	clientAddrLength = sizeof(clientAddr);
	sClient = accept(sHost, (sockaddr *)&clientAddr, &clientAddrLength);
	if (sClient == -1) {
		cout << "Accept Failed" << endl;
		close(sHost);
		return SOCKET_ACCEPT_FAILED;
	}

	sSwitch = &sClient;
	close(sHost);
	return 0;
}

int SocketClass::Init_Client()
{
	//初始化赋值Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	servAddr.sin_port = htons(port);

	isServer = false;
	//连接服务器
	if (connect(sHost, (sockaddr *)&servAddr, sizeof(servAddr)) < 0 ) {
		cout << "connect failed!\n" << endl;
		close(sHost);
		return SOCKET_CONNECT_FAILED;
	}
	else
	{
		cout << "Connect succeeded" << endl;
	}
	sSwitch = &sHost;

	return 0;
}
#endif //__linux__


int SocketClass::SendCmd(string cmd)
{
	if (cmd == "exit")
	{
		if (keepReceiveFlag)	keepReceiveFlag = false;
		else return 0;
	}
	// 真正的发送信息
	int retVal = send(*sSwitch, cmd.c_str(), cmd.size() + 1, 0);
	if (retVal == -1) {
		cout << "Send Error" << endl;
		return SOCKET_SEND_FAILED;
	}
	
	return 0;
}

int SocketClass::Receive()
{
	int retVal;
	char bufRecv[BUF_SIZE] = { 0 };
	while (keepReceiveFlag)
	{
		memset(bufRecv, 0, sizeof(bufRecv));
		retVal = recv(*sSwitch, bufRecv, BUF_SIZE, 0);
		if (retVal == -1) {
			cout << "Receive Failed" << endl;
			return SOCKET_RECEIVE_FAILED;
		}
		else {
			receiveInfo = bufRecv;
			if (receiveInfo == "exit")
			{
				SendCmd("exit");
				continue;
			}
			else
			{
				if (isServer)
				{
					SendCmd(to_string(ServerDealSocket(receiveInfo)));
				}
				else //isClient
				{
					cout << ClientDealSocket(receiveInfo) << endl;
				}
				
			}
			//处理收到的信息，写处理功能函数，要考虑到前后端两种
			//如果一旦receive中断，则直接对整个架构析构，一切重来就行
			//记得获取到某个特定字符串的时候就停止，先发给服务端后自己停止，记得先停止自动读取数据
		}
	}
	return 0;
}

int SocketClass::ServerDealSocket(string cmd)
{
	if (cmd == "abc")
	{
		cout << "get abc from Client" << endl;
		return 123;
	}
	return 0;

}

int SocketClass::ClientDealSocket(string cmd)
{
	if (cmd == "123")
	{
		cout << "get 123 from Server, abc is got, return: " << endl;
		return 123;
	}
	return 234;
}
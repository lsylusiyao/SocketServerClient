//
//#include <sys/types.h>
//#include <sys/socket.h>
#include <stdio.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <sys/shm.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#define PORT 7000
#define QUEUE 20
using namespace std;

int main() {
	fd_set rfds;
	struct timeval tv;
	int retval, maxfd;
	WORD SocketVersion = MAKEWORD(2, 2);
	WSADATA wsd;
	if (WSAStartup(SocketVersion, &wsd) != 0)
	{
		cout << "绑定Socket库失败" << endl;
	}
	int ss = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	//printf("%d\n",INADDR_ANY);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(ss, (struct sockaddr*) &server_sockaddr, sizeof(server_sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}
	if (listen(ss, QUEUE) == -1) {
		perror("listen");
		exit(1);
	}

	struct sockaddr_in client_addr;
	int length = sizeof(client_addr);
	///成功返回非负描述字，出错返回-1
	int conn = accept(ss, (struct sockaddr*)&client_addr, &length);
	if (conn < 0) {
		perror("connect");
		exit(1);
	}
	while (1) {
		/*把可读文件描述符的集合清空*/
		FD_ZERO(&rfds);
		/*把标准输入的文件描述符加入到集合中*/
		FD_SET(0, &rfds);
		maxfd = 0;
		/*把当前连接的文件描述符加入到集合中*/
		FD_SET(conn, &rfds);
		/*找出文件描述符集合中最大的文件描述符*/
		if (maxfd < conn)
			maxfd = conn;
		/*设置超时时间*/
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		/*等待聊天*/
		retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
		if (retval == -1) {
			printf("select出错，客户端程序退出\n");
			break;
		}
		else if (retval == 0) {
			printf("服务端没有任何输入信息，并且客户端也没有信息到来，waiting...\n");
			continue;
		}
		else {
			/*客户端发来了消息*/
			if (FD_ISSET(conn, &rfds)) {
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				int len = recv(conn, buffer, sizeof(buffer), 0);
				if (strcmp(buffer, "exit\n") == 0) break;
				printf("%s", buffer);
				//send(conn, buffer, len , 0);把数据回发给客户端
			}
			/*用户输入信息了,开始处理信息并发送*/
			if (FD_ISSET(0, &rfds)) {
				char buf[1024];
				fgets(buf, sizeof(buf), stdin);
				//printf("you are send %s", buf);
				send(conn, buf, sizeof(buf), 0);
			}
		}
	}
	closesocket(conn);
	closesocket(ss);
	return 0;
}


//
//#include <iostream>
//#include <WinSock2.h>
//#pragma comment(lib,"ws2_32.lib")
//using namespace std;
//
//int main()
//{
//	int RetVal;
//	WORD SocketVersion = MAKEWORD(2, 2);
//	WSADATA wsd;
//	if (WSAStartup(SocketVersion, &wsd) != 0)
//	{
//		cout << "绑定Socket库失败" << endl;
//	}
//	SOCKET ServerSocket;
//	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (ServerSocket == INVALID_SOCKET)
//	{
//		cout << "创建服务器套接字失败" << endl;
//		WSACleanup();
//		return -1;
//	}
//	SOCKADDR_IN ServerAddr;
//	ServerAddr.sin_family = AF_INET;
//	ServerAddr.sin_port = htons(23450);
//	ServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;
//	RetVal = bind(ServerSocket, (SOCKADDR *)&ServerAddr, sizeof(SOCKADDR_IN));
//	if (RetVal == SOCKET_ERROR)
//	{
//		cout << "套接字绑定失败" << endl;
//		closesocket(ServerSocket);
//		WSACleanup();
//		return -1;
//	}
//	RetVal = listen(ServerSocket, 2);
//	if (RetVal == SOCKET_ERROR)
//	{
//		cout << "套接字监听失败" << endl;
//		closesocket(ServerSocket);
//		WSACleanup();
//		return -1;
//	}
//	SOCKET ClientSocket;
//	SOCKADDR_IN ClientAddr;
//	int ClientAddrLen = sizeof(ClientAddr);
//	ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &ClientAddrLen);
//	if (ClientSocket == INVALID_SOCKET)
//	{
//		cout << "接收客户端请求失败" << endl;
//		closesocket(ServerSocket);
//		WSACleanup();
//		return -1;
//	}
//	char ReceiveBuff[BUFSIZ];
//	char SendBuff[BUFSIZ];
//	while (true)
//	{
//		ZeroMemory(ReceiveBuff, BUFSIZ);
//		RetVal = recv(ClientSocket, ReceiveBuff, BUFSIZ, 0);
//		if (RetVal == SOCKET_ERROR)
//		{
//			cout << "接收数据失败" << endl;
//			closesocket(ServerSocket);
//			closesocket(ClientSocket);
//			WSACleanup();
//			return -1;
//		}
//		cout << "接收自客户端数据：" << ReceiveBuff << endl;
//		cout << "向客户端发送数据：";
//		cin >> SendBuff;
//		send(ClientSocket, SendBuff, strlen(SendBuff), 0);
//	}
//	closesocket(ServerSocket);
//	closesocket(ClientSocket);
//	WSACleanup();
//	return 0;
//}



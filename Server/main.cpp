#include<WinSock2.h>
#include<iostream>
#include<WS2tcpip.h>
#include<list>
#include"Message.hpp"
#pragma comment(lib, "ws2_32.lib")
int main() {
	// 应用程序或DLL只能在一次成功的WSAStartup()调用之后才能调用进一步的Windows Sockets API函数
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2,2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup func error, error num is" << WSAGetLastError() << std::endl;
		return 1;
	}
	
	// socket -> bind -> listen -> accept
	
	// Socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)代表tcp协议栈
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 函数成功返回协议栈句柄（文件描述符），失败返回-1（INVALID_SOCKET）
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, error num is : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in servAddr{};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(2345);
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr.S_un.S_addr);

	// bind 没有错误返回0，有错误，返回-1（SOCKET_ERROR）
	if (bind(servSock, (sockaddr*)&servAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		std::cout << "bind func error, error num is : " << WSAGetLastError() << std::endl;
		// 如果出错了就关闭sock
		closesocket(servSock);
		WSACleanup();
		return 1;
	}

	// listen 没有错误返回0，有错误，返回-1（SOCKET_ERROR）
	if (listen(servSock, 128) == SOCKET_ERROR) {
		std::cout << "listen func error, error num is : " << WSAGetLastError() << std::endl;
		// 如果出错了就关闭sock
		closesocket(servSock);
		WSACleanup();
		return 1;
	}

	//用select多路复用监听（accpet）
	std::list<SOCKET> clientList;
	while (1) {
		/* int select(
			int nfds,                  //windows中无意义
			fd_set* fdRead,            //检查可读性
			fd_set* fdWrite,           //检查可写性
			fd_set* fdExcept,          //检查异常
			const timeval* timeout     //设定超时时间
			)
		*/

		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(servSock, &fdRead);
		for (auto client : clientList) {
			FD_SET(client, &fdRead);
		}
		timeval tv{ 1, 0 };
		
		int selectRes = select(0, &fdRead, nullptr, nullptr, &tv);

		// 返回值：
		//> 0 : 表示有客户端链接或有客户端发送消息
		//= 0 : 表示超时无客户端链接，也没有客户端发送消息
		//< 0 : 表示select函数出现错误
		if (selectRes > 0) {
			//有消息，遍历，来看到底是哪些有消息，还要看这些是 新连接 还是 消息
			for (unsigned i = 0; i < fdRead.fd_count; i++) {
				//说明是一个新连接
				if (fdRead.fd_array[i] == servSock) {
					SOCKET clientSock = accept(servSock, nullptr, nullptr);
					if (clientSock == INVALID_SOCKET) {
						std::cout << "accept func error, the error num is " << WSAGetLastError() << std::endl;
						break;
					}
					clientList.push_back(clientSock);
				}
				//收到新消息
				else {
					Header header;
					/*	返回值：
						>0，接收数据的字符数
						=0，对面优雅断开了链接
						<0，recv函数出现错误
					*/
					
					// 先接收 Header
					int recvRes = recv(fdRead.fd_array[i], (char*)&header, sizeof(header), 0);
					if (recvRes <= 0) {
						std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
						closesocket(servSock);
						for (auto client : clientList) {
							closesocket(client);
						}
						WSACleanup();
						return 1;
					}
					// 然后接收剩余的，如果收得到，判断一下对方发过来的是什么cmd
					switch (header.cmd) {
					case CMD::LOGIN: {
						Login login;
						int recvRes2 = recv(fdRead.fd_array[i], (char*)&login + sizeof(Header), sizeof(Login) - sizeof(Header), 0);
						if (recvRes2 <= 0) {
							std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
							closesocket(servSock);
							for (auto client : clientList) {
								closesocket(client);
							}
							WSACleanup();
							return 1;
						}
						LoginResult loginResult(20);

						// 发送回给client 状态码10
						if (send(fdRead.fd_array[i], (char*)&loginResult, sizeof(loginResult), 0) < 0) {
							std::cout << "send func error, the error num is " << WSAGetLastError() << std::endl;
							closesocket(servSock);
							for (auto client : clientList) {
								closesocket(client);
							}
							WSACleanup();
							return 1;
						}
						break;
					}
					case CMD::LOGOUT:{
						Logout logout;
						int recvRes2 = recv(fdRead.fd_array[i], (char*)&logout + sizeof(Header), sizeof(Logout) - sizeof(Header), 0);
						if (recvRes2 <= 0) {
							std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
							closesocket(servSock);
							for (auto client : clientList) {
								closesocket(client);
							}
							WSACleanup();
							return 1;
						}
						LogoutResult logoutResult(10);
						if (send(fdRead.fd_array[i], (char*)&logoutResult, sizeof(logoutResult), 0) < 0) {
							std::cout << "send func error, the error num is " << WSAGetLastError() << std::endl;
							closesocket(servSock);
							for (auto client : clientList) {
								closesocket(client);
							}
							WSACleanup();
							return 1;
						}
						break;
					}
					default:
						break;
					}
				}
			}
		}
		else if (selectRes == 0) {
			std::cout << "do something else" << std::endl;
			continue;
		}
		else {
			std::cout << "select func error, error num : " << WSAGetLastError() << std::endl;
			break;
		}
	}
	closesocket(servSock);
	for (auto client : clientList) {
		closesocket(client);
	}
	WSACleanup();
	system("pause");
	return 0;
}
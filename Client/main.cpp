#include<WinSock2.h>
#include<iostream>
#include<WS2tcpip.h>
#include"Message.hpp"
#pragma comment(lib, "ws2_32.lib")

// client¶Ë
// socket -> connect
int main() {
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup error, the error num is : " << WSAGetLastError() << std::endl;
		return 1;
	}
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, the error num is " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	sockaddr_in servAddr{};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(2345);
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr.S_un.S_addr);
	if (connect(servSock, (sockaddr*)&servAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		std::cout << "connect func error, the error num is : " << WSAGetLastError() << std::endl;
		closesocket(servSock);
		WSACleanup();
		return 1;
	}
	std::cout << "connection success" << std::endl;
	while (1) {
		Login login("aaa", "bbb");
		if (send(servSock, (char*)&login, sizeof(Login), 0) < 0) {
			std::cout << "send func error, the error num is : " << WSAGetLastError() << std::endl;
			closesocket(servSock);
			WSACleanup();
			break;
		}
		LoginResult loginResult;
		if (recv(servSock, (char*)&loginResult, sizeof(LoginResult), 0) <= 0) {
			std::cout << "recv func error, the error num is : " << WSAGetLastError() << std::endl;
			closesocket(servSock);
			WSACleanup();
			break;
		}
		std::cout << loginResult.loginResultNum << std::endl;
		Sleep(100);
	}
	
	system("pause");

	return 0;
}
#include "Client.h"
#include <iostream>
#include <WS2tcpip.h>
Client::Client():servSock(INVALID_SOCKET) {

}
Client::~Client() {
	close();
}
void Client::close() {
	if (servSock != INVALID_SOCKET) {
		closesocket(servSock);
		WSACleanup();
	}
}
bool Client::init(const char* ip, unsigned short port) {
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup error, the error num is : " << WSAGetLastError() << std::endl;
		return false;
	}
	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, the error num is " << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}
	sockaddr_in servAddr{};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servAddr.sin_addr.S_un.S_addr);
	if (connect(servSock, (sockaddr*)&servAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		std::cout << "connect func error, the error num is : " << WSAGetLastError() << std::endl;
		closesocket(servSock);
		WSACleanup();
		return false;
	}
	std::cout << "connection success" << std::endl;
	return true;
}
bool Client::start(const char* username, const char* pwd) {
	while (1) {
		//Login login("aaa", "bbb");
		Login login(username, pwd);
		if (send(servSock, (char*)&login, sizeof(Login), 0) < 0) {
			std::cout << "send func error, the error num is : " << WSAGetLastError() << std::endl;
			closesocket(servSock);
			WSACleanup();
			return false;
		}
		LoginResult loginResult;
		if (recv(servSock, (char*)&loginResult, sizeof(LoginResult), 0) <= 0) {
			std::cout << "recv func error, the error num is : " << WSAGetLastError() << std::endl;
			closesocket(servSock);
			WSACleanup();
			return false;
		}
		std::cout << loginResult.loginResultNum << std::endl;
		Sleep(100);
	}
	return true;
}
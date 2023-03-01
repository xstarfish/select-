#include "Server.h"
#include <WS2tcpip.h>
#include <assert.h>

Server* Server::instance = new Server;
Server::Server():servSock(INVALID_SOCKET) {

}
Server::~Server() {
	close();
}
void Server::close() {
	//����Ҫ���Լ��ģ� ��Ҫ�ر����ӵ�clientSocket
	if (servSock != INVALID_SOCKET) {
		closesocket(servSock);
		for (auto client : clientList) {
			closesocket(client);
		}
		WSACleanup();
	}
}
// ��ʼ�� socket -> bind -> listen
bool Server::init(const char* ip, unsigned short port) {
	// Ӧ�ó����DLLֻ����һ�γɹ���WSAStartup()����֮����ܵ��ý�һ����Windows Sockets API����
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup func error, error num is" << WSAGetLastError() << std::endl;
		return 1;
	}
	// Socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)����tcpЭ��ջ
	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// �����ɹ�����Э��ջ������ļ�����������ʧ�ܷ���-1��INVALID_SOCKET��
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, error num is : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}

	sockaddr_in servAddr{};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servAddr.sin_addr.S_un.S_addr);

	// bind û�д��󷵻�0���д��󣬷���-1��SOCKET_ERROR��
	if (bind(servSock, (sockaddr*)&servAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		std::cout << "bind func error, error num is : " << WSAGetLastError() << std::endl;
		// ��������˾͹ر�sock
		closesocket(servSock);
		WSACleanup();
		return false;
	}

	// listen û�д��󷵻�0���д��󣬷���-1��SOCKET_ERROR��
	if (listen(servSock, 128) == SOCKET_ERROR) {
		std::cout << "listen func error, error num is : " << WSAGetLastError() << std::endl;
		// ��������˾͹ر�sock
		closesocket(servSock);
		WSACleanup();
		return false;
	}
	return true;
}
bool Server::start() {
	assert(servSock != INVALID_SOCKET);
	while (1) {
		/* int select(
			int nfds,                  //windows��������
			fd_set* fdRead,            //���ɶ���
			fd_set* fdWrite,           //����д��
			fd_set* fdExcept,          //����쳣
			const timeval* timeout     //�趨��ʱʱ��
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

		// ����ֵ��
		//> 0 : ��ʾ�пͻ������ӻ��пͻ��˷�����Ϣ
		//= 0 : ��ʾ��ʱ�޿ͻ������ӣ�Ҳû�пͻ��˷�����Ϣ
		//< 0 : ��ʾselect�������ִ���
		if (selectRes > 0) {
			//����Ϣ��������������������Щ����Ϣ����Ҫ����Щ�� ������ ���� ��Ϣ
			for (unsigned i = 0; i < fdRead.fd_count; i++) {
				//˵����һ��������
				if (fdRead.fd_array[i] == servSock) {
					SOCKET clientSock = accept(servSock, nullptr, nullptr);
					if (clientSock == INVALID_SOCKET) {
						std::cout << "accept func error, the error num is " << WSAGetLastError() << std::endl;
						break;
					}
					clientList.push_back(clientSock);
				}
				//�յ�����Ϣ
				else {
					if (!recvMsg(fdRead, fdRead.fd_array[i])) {
						return false;
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
			return false;
		}
	}
	return true;
}
bool Server::recvMsg(fd_set& fdRead, SOCKET clientSock) {
	Header header;
	/*	����ֵ��
		>0���������ݵ��ַ���
		=0���������ŶϿ�������
		<0��recv�������ִ���
	*/

	// �Ƚ��� Header
	int recvRes = recv(clientSock, (char*)&header, sizeof(header), 0);
	if (recvRes <= 0) {
		std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
		return false;
	}
	// Ȼ�����ʣ��ģ�����յõ����ж�һ�¶Է�����������ʲôcmd
	switch (header.cmd) {
	case CMD::LOGIN: {
		Login login;
		int recvRes2 = recv(clientSock, (char*)&login + sizeof(Header), sizeof(Login) - sizeof(Header), 0);
		if (recvRes2 <= 0) {
			std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
			return false;
		}
		LoginResult loginResult(20);

		// ���ͻظ�client ״̬��10
		if (send(clientSock, (char*)&loginResult, sizeof(loginResult), 0) < 0) {
			std::cout << "send func error, the error num is " << WSAGetLastError() << std::endl;
			return false;
		}
		break;
	}
	case CMD::LOGOUT: {
		Logout logout;
		int recvRes2 = recv(clientSock, (char*)&logout + sizeof(Header), sizeof(Logout) - sizeof(Header), 0);
		if (recvRes2 <= 0) {
			std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
			return false;
		}
		LogoutResult logoutResult(10);
		if (send(clientSock, (char*)&logoutResult, sizeof(logoutResult), 0) < 0) {
			std::cout << "send func error, the error num is " << WSAGetLastError() << std::endl;
			return false;
		}
		break;
	}
	default:
		break;
	}
	return true;
}
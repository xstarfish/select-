#include<WinSock2.h>
#include<iostream>
#include<WS2tcpip.h>
#include<list>
#include"Message.hpp"
#pragma comment(lib, "ws2_32.lib")
int main() {
	// Ӧ�ó����DLLֻ����һ�γɹ���WSAStartup()����֮����ܵ��ý�һ����Windows Sockets API����
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2,2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup func error, error num is" << WSAGetLastError() << std::endl;
		return 1;
	}
	
	// socket -> bind -> listen -> accept
	
	// Socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)����tcpЭ��ջ
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// �����ɹ�����Э��ջ������ļ�����������ʧ�ܷ���-1��INVALID_SOCKET��
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, error num is : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in servAddr{};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(2345);
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr.S_un.S_addr);

	// bind û�д��󷵻�0���д��󣬷���-1��SOCKET_ERROR��
	if (bind(servSock, (sockaddr*)&servAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		std::cout << "bind func error, error num is : " << WSAGetLastError() << std::endl;
		// ��������˾͹ر�sock
		closesocket(servSock);
		WSACleanup();
		return 1;
	}

	// listen û�д��󷵻�0���д��󣬷���-1��SOCKET_ERROR��
	if (listen(servSock, 128) == SOCKET_ERROR) {
		std::cout << "listen func error, error num is : " << WSAGetLastError() << std::endl;
		// ��������˾͹ر�sock
		closesocket(servSock);
		WSACleanup();
		return 1;
	}

	//��select��·���ü�����accpet��
	std::list<SOCKET> clientList;
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
					Header header;
					/*	����ֵ��
						>0���������ݵ��ַ���
						=0���������ŶϿ�������
						<0��recv�������ִ���
					*/
					
					// �Ƚ��� Header
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
					// Ȼ�����ʣ��ģ�����յõ����ж�һ�¶Է�����������ʲôcmd
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

						// ���ͻظ�client ״̬��10
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
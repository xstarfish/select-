#pragma once
#include <WinSock2.h>
#include "Message.hpp"
#include <iostream>
#include <list>
class Server{
public:
	static Server* getInstance() {
		return instance;
	}
	~Server();
	void close();
	bool init(const char* ip, unsigned short port);
	bool start();
	bool recvMsg(fd_set& fdRead, SOCKET clientSock);
private:
	Server();
	static Server* instance;
	SOCKET servSock;
	std::list<SOCKET>clientList;
};


#pragma once
#include <WinSock2.h>
#include "Message.hpp"
class Client {
public:
	Client();
	~Client();
	void close();
	bool init(const char* ip, unsigned short port);
	bool start(const char* username, const char* pwd);
private:
	SOCKET servSock;
};
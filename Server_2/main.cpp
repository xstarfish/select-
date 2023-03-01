#include"Server.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	// 使用单例模式，一个Server对象代表一个服务
	Server* server = Server::getInstance();

	/*Server* server2 = Server::getInstance();
	if (server == server2) {
		std::cout << "yes" << std::endl;
	}
	*/

	if (!server->init("127.0.0.1", 2345)) {
		return -1;
	}
	if (!server->start()) {
		return -1;
	}
	system("pause");
	return 0;
}
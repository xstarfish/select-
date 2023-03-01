#include"Client.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	Client *client = new Client();
	if (!client->init("127.0.0.1", 2345)) {
		return -1;
	}
	if (!client->start("cxy", "329")) {
		return -1;
	}
	system("pause");

	return 0;
}
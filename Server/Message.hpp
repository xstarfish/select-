#pragma once
#include<cstring>
enum class CMD {
	LOGIN,
	LOGOUT,
	LOGIN_RESULT,
	LOGOUT_RESULT,
	NO_CMD
};

struct Header {
	CMD cmd;
	unsigned dataLength;
	Header() :cmd(CMD::NO_CMD), dataLength(0) {

	}
	Header(CMD _cmd, unsigned _dataLength) :cmd(_cmd), dataLength(_dataLength) {

	}
};

struct Login :public Header {
	char userName[32];
	char passwd[32];
	Login(const char* _userName, const char* _passwd) : Header(CMD::LOGIN, sizeof(Login)) {
		strcpy_s(userName, 32, _userName);
		strcpy_s(passwd, 32, _passwd);
	}
	Login() : Header(CMD::LOGIN, sizeof(Login)), userName{ 0 }, passwd{ 0 } {

	}
};

struct Logout :public Header {
	unsigned logoutNum;
	Logout(unsigned _logoutNum) : Header(CMD::LOGOUT, sizeof(Logout)), logoutNum(_logoutNum) {

	}
	Logout() : Header(CMD::LOGOUT, sizeof(Logout)), logoutNum(0) {

	}
};

struct LoginResult : public Header {
	unsigned loginResultNum;
	LoginResult(unsigned _loginResultNum) :Header(CMD::LOGIN_RESULT, sizeof(LoginResult)), loginResultNum(_loginResultNum) {

	}
	LoginResult() :Header(CMD::LOGIN_RESULT, sizeof(LoginResult)), loginResultNum(0) {

	}
};

struct LogoutResult : public Header {
	unsigned logoutResultNum;
	LogoutResult(unsigned _logoutResultNum) : Header(CMD::LOGOUT_RESULT, sizeof(LogoutResult)), logoutResultNum(_logoutResultNum) {

	}
	LogoutResult() : Header(CMD::LOGIN_RESULT, sizeof(LogoutResult)), logoutResultNum(0) {

	}
};
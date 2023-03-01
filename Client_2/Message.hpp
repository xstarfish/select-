#pragma once
#include<cstring>
enum class CMD
{
	LOGIN,
	LOGOUT,
	LOGIN_RESULT,
	LOGOUT_RESULT,
	NO_CMD
};

struct Header
{
	CMD cmd;
	unsigned dataLength;
	Header() :cmd(CMD::NO_CMD), dataLength(0) {}
	Header(CMD cmd_, unsigned dataLength_) : cmd(cmd_), dataLength(dataLength_) {}
};

struct Login : public Header
{
	char usrName[32];
	char passwd[32];
	Login(const char* usrName_, const char* passwd_) :Header(CMD::LOGIN, sizeof(Login))
	{
		strcpy_s(usrName, 32, usrName_);
		strcpy_s(passwd, 32, passwd_);
	}
	Login() :Header(CMD::LOGIN, sizeof(Login)), usrName{ 0 }, passwd{ 0 } {}
};

struct Logout : public Header
{
	unsigned logoutNum;
	Logout(unsigned logoutNum_) :Header(CMD::LOGOUT, sizeof(Logout)), logoutNum(logoutNum_) {}
	Logout() :Header(CMD::LOGOUT, sizeof(Logout)), logoutNum(0) {}
};

struct LoginResult : public Header
{
	unsigned loginResultNum;
	LoginResult(unsigned loginResultNum_) : Header(CMD::LOGIN_RESULT, sizeof(LoginResult)), loginResultNum(loginResultNum_) {}
	LoginResult() : Header(CMD::LOGIN_RESULT, sizeof(LoginResult)), loginResultNum(0) {}
};
struct LogoutResult : public Header
{
	unsigned logoutResultNum;
	LogoutResult(unsigned logoutResultNum_) :Header(CMD::LOGOUT_RESULT, sizeof(LogoutResult)), logoutResultNum(logoutResultNum_) {}
	LogoutResult() :Header(CMD::LOGOUT_RESULT, sizeof(LogoutResult)), logoutResultNum(0) {}
};
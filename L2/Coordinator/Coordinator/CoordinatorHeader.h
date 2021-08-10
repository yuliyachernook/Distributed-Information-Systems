#pragma once
#include <string>
#include <WinSock2.h>
#include <thread>
#include <list>


#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable:4996)

std::string GetErrorMsgText(int code)
{
	std::string msgText;

	switch (code) {
	case WSAEINTR:
		msgText = "WSAEINTR";
		break;
	case WSAEACCES:
		msgText = "WSAEACCES";
		break;

	case WSASYSCALLFAILURE:
		msgText = "WSASYSCALLFAILURE";
		break;
	default:
		msgText = "***ERROR***";
		break;
	};

	return msgText;
}

std::string SetErrorMsgText(std::string msgText, int code) {
	return  msgText + GetErrorMsgText(code);
}

const int ENTERCA = 1001;
const int LEAVCA = 1000;

const int OKGOONCA = 2001;
const int WAITASECONDCA = 2002;

bool CRITICAL_SECTION_FLAG = false;

std::list<SOCKADDR_IN> clientQueue;
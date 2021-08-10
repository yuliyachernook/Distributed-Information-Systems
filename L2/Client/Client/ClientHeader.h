#pragma once
#include <iostream>
#include <WinSock2.h>
#include <fstream>

#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable:4996)

std::string GetErrorMsgText(int code)
{
	std::string msgText;

	switch (code)
	{
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

std::string SetErrorMsgText(std::string msgText, int code) { return  msgText + GetErrorMsgText(code); }


const int ENTERCA = 1001;
const int LEAVCA = 1000;

const int OKGOONCA = 2001;
int size = sizeof(SOCKADDR_IN);

struct CA {
	char* ipaddr;
	char* resource;
	enum STATUS {
		NOINIT,
		INIT,
		ENTER,
		LEAVE,
		WAIT
	} status;

	SOCKADDR_IN server;

	CA(){
		status = NOINIT;
		std::cout << "CA is not initialized" << std::endl;
	}
};

CA InitCA(char ipaddr[], char resource[]) {
	CA object;
	object.status = object.INIT;
	std::cout << "CA is initialized" << std::endl;
	std::cout << "Coordinator IP: " << ipaddr << std::endl;
	std::cout << "Client wants to get access to: " << resource << std::endl;
	
	object.ipaddr = ipaddr;
	object.resource = resource;

	object.server.sin_family = AF_INET;
	object.server.sin_addr.s_addr = inet_addr("127.0.0.1");
	object.server.sin_port = htons(2000);

	return object;
}

bool EnterCA(CA& ca, SOCKET sC) {
	std::cout << "Client tries to enter critical section" << std::endl;
	int out;

	sendto(sC, (char*)&ENTERCA, sizeof(int), NULL, (sockaddr*)&(ca.server), size);
	recvfrom(sC, (char*)&out, sizeof(int), NULL, (SOCKADDR*)&(ca.server), &size);

	if (out == OKGOONCA)
	{
		std::cout << "Client entered critical section" << std::endl;
		ca.status = ca.WAIT;
		return true;
	}
	std::cout << "Client couldn't enter critical section" << std::endl;
	std::cout << "Critical section is waiting..." << std::endl;
	ca.status = ca.ENTER;
	return false;
}

bool LeaveCA(CA& ca, SOCKET sC) {
	std::cout << "Client is leaving critical section" << std::endl;
	try
	{
		if (sendto(sC, (char*)&LEAVCA, sizeof(int), NULL, (sockaddr*)&(ca.server), size) == SOCKET_ERROR)
			throw  SetErrorMsgText("sendto:", WSAGetLastError());
	}
	catch (std::string errorMsgText)
	{
		std::cout << "WSAGetLastError: " << errorMsgText << std::endl;
		std::cout << "Client couldn't leave critical section" <<std::endl;
		ca.status = ca.WAIT;
		return false;
	}

	std::cout << "Client leaved critical section" <<std::endl;
	ca.status = ca.LEAVE;
	return true;
}

bool CloseCA(CA& ca) {
	std::cout << "Critical section is closed" << std::endl;
	return true;
}
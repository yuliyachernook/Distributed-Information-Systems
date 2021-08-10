
#include "iostream"
#include "CoordinatorHeader.h"

void main() {
	setlocale(LC_ALL, "Russian");

	std::cout << "Server is ready..." << std::endl;

	SOCKET sS;
	WSADATA wsaData;

	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("Socket:", WSAGetLastError());

		SOCKADDR_IN server;

		server.sin_family = AF_INET;
		server.sin_port = htons(2000);
		server.sin_addr.s_addr = inet_addr("127.0.0.1");

		if (bind(sS, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR)
			throw  SetErrorMsgText("Bind:", WSAGetLastError());

		SOCKADDR_IN client;
		memset(&client, 0, sizeof(client));
		int size = sizeof(client);

		int in;

		while (true)
		{
			if (recvfrom(sS, (char*)&in, sizeof(int), NULL, (sockaddr*)&client, &size) == SOCKET_ERROR)
				throw  SetErrorMsgText("recvfrom:", WSAGetLastError());


			if (in == ENTERCA && CRITICAL_SECTION_FLAG == false)
			{
				std::cout << "[" << inet_ntoa(client.sin_addr) << ":" << htons(client.sin_port) << "]" << " Client enters critical section" << std::endl;

				if (sendto(sS, (char*)&OKGOONCA, sizeof(int), NULL, (sockaddr*)&client, size) == SOCKET_ERROR)
					throw  SetErrorMsgText("sendto:", WSAGetLastError());

				CRITICAL_SECTION_FLAG = true;
			}
			else if (in == LEAVCA) {
				std::cout << "[" << inet_ntoa(client.sin_addr) << ":" << htons(client.sin_port) << "]" << " Client leaves critical section" << std::endl;

				CRITICAL_SECTION_FLAG = false;
			}
			else
			{
				std::cout << "[" << inet_ntoa(client.sin_addr) << ":" << htons(client.sin_port) << "]" << " Client wants to enter critical section" << std::endl;
				std::cout << "[" << inet_ntoa(client.sin_addr) << ":" << htons(client.sin_port) << "]" << " Critical section is busy" << std::endl;

				if (sendto(sS, (char*)&WAITASECONDCA, sizeof(int), NULL, (sockaddr*)&client, size) == SOCKET_ERROR)
					throw  SetErrorMsgText("sendto:", WSAGetLastError());

				clientQueue.push_back(client);
			}
		}

		if (closesocket(sS) == SOCKET_ERROR)
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText) {
		std::cout << std::endl << "WSAGetLastError: " << errorMsgText;
	}
}
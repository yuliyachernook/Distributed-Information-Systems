// Client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
#include <iostream>
#include "ClientHeader.h"
#include <time.h>
#include <string>

using namespace std;


void main() {
	int s = 0;
	setlocale(LC_ALL, "Russian");
	int currentCount = 0;
	cout << "Client is ready" << endl;
	cin >> s;
	char ipaddr[] = "127.0.0.1";
	char resource[] = "F:\\3k2sem\\RIS\\L2\\file.txt";

	CA* mainObject;
	mainObject = &InitCA(ipaddr, resource);


	SOCKET sC;
	WSADATA wsaData;

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((sC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("Socket:", WSAGetLastError());
		

		while (!EnterCA(*mainObject, sC)) {
			Sleep(10000);
		}

		cout << "Client starts writing in file" << endl;

		ofstream out;
		out.open(resource, ios::app);
		time_t temp;

		out << "Client text: "  << endl;
		out << s << endl;
		for (size_t i = 0; i < 10; i++)
		{
			temp = time(0);
			if (out.is_open())
			{
				out << ctime(&temp);
			}
			Sleep(1000);
		}

		cout << "Client starts reading file" << endl << endl;
		ifstream in("F:\\3k2sem\\RIS\\L2\\file.txt");
		string line;

		for (size_t i = 0; i < 10; i++)
		{
			if (in.is_open())
			{
				getline(in, line);
				std::cout << line << "\n";
			}
		}
		in.close();

		cout << endl;

		while (!LeaveCA(*mainObject, sC)) {
			Sleep(10000);
		}

		if (closesocket(sC) == SOCKET_ERROR)
			throw  SetErrorMsgText("Closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText)
	{
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}
	while (!CloseCA(*mainObject)) {
		Sleep(10000);
	}
}

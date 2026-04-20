#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include <miniaudio.h>

#include "GrowlListener.h"

#define BUFFER_SIZE 65536

int main()
{
	WSADATA wsaData;

	int errStat = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errStat != 0) {
		std::cout << "WSAStartup failed: " << WSAGetLastError() << "\n";
		return 1;
	}
	else {
		std::cout << "WinSock initializated!\n";
	}

	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, NULL);
	if (clientSock == INVALID_SOCKET) {
		std::cout << "Error create socket: " << WSAGetLastError() << "\n";
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Socket initializated!\n";
	}

	sockaddr_in servInfo;
	servInfo.sin_family = AF_INET;
	servInfo.sin_port = htons(8000);
	
	in_addr ipToNum;
	errStat = inet_pton(AF_INET, "26.70.26.159", &ipToNum);
	if (errStat <= 0) {
		std::cout << "Error in IP translation in num format\n";
		return 1;
	}

	servInfo.sin_addr = ipToNum;

	errStat = connect(clientSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (errStat != 0) {
		std::cout << "Connection to server is failed: " << WSAGetLastError() << "\n";
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Connection successfull.\n";
	}

	//Test prikol
	std::string testRequest =
		"GET /test.mp3 HTTP/1.1\r\n"
		"Host: 26.70.26.159:8000\r\n"
		"Icy-Metadata: 1\r\n"
		"\r\n";

	int bytesSend = send(clientSock, testRequest.c_str(), testRequest.size(), NULL);
	if (bytesSend == SOCKET_ERROR) {
		std::cout << "Request error!\n";
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}
	std::cout << "Sent " << bytesSend << " bytes to server\n";

	std::vector<char> serverBuffer(BUFFER_SIZE);
	std::string headers;

	bool endOfHeaders = false;
	int bytesInBuffer = 0;

	while (!endOfHeaders) {
		short packetSize;
		packetSize = recv(clientSock, serverBuffer.data() + bytesInBuffer, serverBuffer.size(), NULL);
		std::cout << "Received " << packetSize << " bytes from server\n";
		if (packetSize <= 0) { break; }
		bytesInBuffer += packetSize;

		std::string data(serverBuffer.data(), bytesInBuffer);

		size_t pos = data.find("\r\n\r\n");
		if (pos != std::string::npos) {
			headers = data.substr(0, pos);
			endOfHeaders = true;
		}
	}

	std::cout << "Final message: " << headers << "\n";

	closesocket(clientSock);
	WSACleanup();

	return 0;
}

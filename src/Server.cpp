#include "Server.h"

Server::Server()
{
}

void Server::Init()
{
	initWindow();
	initWinsok();
}

void Server::Launch(const std::string& ipAddress, int port)
{
	struct sockaddr_in server, si_other;

	if ((m_serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		std::cout << "Could not create socket : " + WSAGetLastError() << std::endl;
		return;
	}
	std::cout << "Socket created." << std::endl;;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	server.sin_port = htons(port);

	if (bind(m_serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		std::cout << "Bind failed with error code : " + WSAGetLastError() << std::endl;
		return;
	}
	std::cout << "Lauching done" << std::endl;;
}

void Server::Run()
{
	while (1)
	{
		readMessage();
	}
}

void Server::Close()
{
	closesocket(m_serverSocket);
	WSACleanup();
}

void Server::readMessage()
{
	struct sockaddr_in client;
	int len = sizeof(client);
	std::string recvbuf(512, '\0');

	std::cout << "Waiting for data...\n";
	int recv_len = recvfrom(m_serverSocket, &recvbuf[0], 512, 0, (struct sockaddr*)&client, &len);

	if (recv_len == SOCKET_ERROR) {
		std::cerr << "ReadMessage failed: " << WSAGetLastError() << std::endl;
		return;
	}
	recvbuf.resize(recv_len);
	std::cout << "Data: " << recvbuf << std::endl;
	/*to rmv */ sendMessage("Well received", client);
}

void Server::sendMessage(const std::string& message, struct sockaddr_in & client)
{
	if (sendto(m_serverSocket, message.c_str(), message.size(), 0, (struct sockaddr*)&client, sizeof(client)) == SOCKET_ERROR)
		std::cout << "sendto() failed with error code : " + WSAGetLastError() << std::endl;
}

void Server::initWinsok()
{
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
		std::cout << "WSAStartup failed: " + std::to_string(iResult) << std::endl;
}

void Server::initWindow()
{
}


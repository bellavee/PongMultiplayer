
#include<stdio.h>
#include <iostream>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	std::cout << "Initialising Winsock..." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed. Error Code : " +  WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Initialised." << std::endl ;

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Could not create socket : " +  WSAGetLastError() << std::endl;
	}
	std::cout << "Socket created." << std::endl;;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code : " + WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Bind done" << std::endl;;

	//keep listening for data
	while (1)
	{
		std::cout << "Waiting for data..." << std::endl;
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR)
		{
			std::cout << "recvfrom() failed with error code : " + WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		std::cout << "Received packet from " << inet_ntoa(si_other.sin_addr) + ntohs(si_other.sin_port) << std::endl;
		std::cout << "Data: " << buf << std::endl;

		//now reply the client with the same data
		if (sendto(s, "Well received", strlen("Well received"), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			std::cout << "sendto() failed with error code : " + WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
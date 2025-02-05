#pragma one
#include <string>
#include<winsock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

class Server
{
public:
	Server();
	void Init();
	void Launch(const std::string& ipAddress, int port);
	void Run();
	void Close();
private:
	void readMessage();
	void sendMessage(const std::string& message, struct sockaddr_in& client);
	void initWinsok();
	void initWindow();
	SOCKET m_serverSocket;
};
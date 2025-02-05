#ifndef SERVER_H
#define SERVER_H

#include <string>
#include<winsock2.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "UI_ServerMenu.h"
#include "UI_ServerRunning.h"

#pragma comment(lib,"ws2_32.lib")

enum class ServerState {
	NOT_RUNNING,
	RUNNING,
	CLOSED
};

class Server
{
public:
	Server();
	void Init();
	void Launch(const std::string& ipAddress, int port);
	void Run();
	void Close();
	void Connect();
private:
	void readMessage();
	void sendMessage(const std::string& message, struct sockaddr_in& client);
	void initWinsok();
	void initWindow();
	void processEvents();
	void render();

	ServerState _state;
	SOCKET m_serverSocket;
	bool m_isRunning;
	std::unique_ptr<sf::RenderWindow> _window;
	std::unique_ptr<UI_ServerMenu> _serverMenu;
	std::unique_ptr<UI_ServerRunning> _serverRunning;
};

#endif
#ifndef SERVER_H
#define SERVER_H
#include <nlohmann/json.hpp>
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
	void sendMessage(const std::string& message, const std::string& clientId);
	void initWinsok();
	void initWindow();
	void processEvents();
	void render();
	void decodeClientMessages(const std::string& clientName, nlohmann::json messageContent);
	 
	ServerState _state;
	SOCKET m_serverSocket;
	bool m_isRunning;
	std::unique_ptr<sf::RenderWindow> _window;
	std::unique_ptr<UI_ServerMenu> _serverMenu;
	std::unique_ptr<UI_ServerRunning> _serverRunning;
	std::unordered_map<std::string, sockaddr_in> _clientsMap;
	std::unordered_map<int, sockaddr_in> _players;
	std::vector<sockaddr_in> _clientsList;
	std::unordered_map<std::string, std::string> _messageBuffer;
};

#endif
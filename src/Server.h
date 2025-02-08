#ifndef SERVER_H
#define SERVER_H
#include <nlohmann/json.hpp>
#include <string>
#include<winsock2.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "UI_ServerMenu.h"
#include "UI_ServerRunning.h"
#include "Paddle.h"
#include "Ball.h"

#pragma comment(lib,"ws2_32.lib")

enum class ServerState {
	NOT_RUNNING,
	RUNNING,
	GAME_STARTED,
	GAME_ENDED,
	CLOSED,
	PAUSED
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
	void setPauseState(bool paused);
	void newClientConnected(const std::string& clientName, nlohmann::json messageContent);
	void clientIsMoving(const std::string& clientName, nlohmann::json messageContent);
	void updatePlayersPosition();
	void startGame();
	void sendMessageToAll(const std::string& mess);
	void updateGameState();
	void update(float deltatime);
	void handleEndGame();

	void checkPaddleCollision(Ball &ball, const Paddle &paddle, bool isLeftPaddle);

	void checkScore();

	ServerState _state;
	SOCKET m_serverSocket;
	bool m_isRunning;
	std::unique_ptr<sf::RenderWindow> _window;
	std::unique_ptr<UI_ServerMenu> _serverMenu;
	std::unique_ptr<UI_ServerRunning> _serverRunning;
	std::unordered_map<std::string, sockaddr_in> _clientsMap;
	std::unordered_map<int, std::string> _players;
	std::vector<std::string> _clientsList;
	std::unordered_map<std::string, std::string> _clientsNamesList;
	std::unordered_map<std::string, std::string> _messageBuffer;

	std::unique_ptr<Paddle> _playerOnePaddle;
	std::unique_ptr<Paddle> _playerTwoPaddle;
	std::unique_ptr<Ball> _ball;
	int _playerOneScore;
	int _playerTwoScore;
	float _playerSpeed;
	int _playerOneDir;
	int _playerTwoDir;
};

#endif
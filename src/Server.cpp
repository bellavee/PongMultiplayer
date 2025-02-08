#include "Server.h"
#include "resources.h"

using json = nlohmann::json;

Server::Server() 
	: _window(std::make_unique<sf::RenderWindow>(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Server"))
	, _serverMenu(nullptr)
	, _serverRunning(nullptr)
	, m_isRunning(false)
	, _state(ServerState::NOT_RUNNING)
	, _playerOnePaddle(std::make_unique<Paddle>(30.0f, WINDOW_HEIGHT / 2))
	, _playerTwoPaddle(std::make_unique<Paddle>(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT / 2))
	, _ball(std::make_unique<Ball>(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2))
	, _playerOneScore(0)
	, _playerTwoScore(0)
	, _playerSpeed(15)
	, _playerOneDir(0)
	, _playerTwoDir(0)
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

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	server.sin_port = htons(port);

	if (bind(m_serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		std::cout << "Bind failed with error code : " + WSAGetLastError() << std::endl;
		return;
	}
	u_long mode = 1;
	ioctlsocket(m_serverSocket, FIONBIO, &mode);

	m_isRunning = true;
	_serverRunning->setContent("Server running at " + ipAddress + " at port " + std::to_string(port));
	_state = ServerState::RUNNING;
}

void Server::Run()
{
	sf::Clock clock;
	while (_window->isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		processEvents();
		render();
		switch (_state)
		{
		case ServerState::NOT_RUNNING:
			break;
		case ServerState::RUNNING:
			readMessage();
			break;
		case ServerState::GAME_STARTED:
			readMessage();
			update(deltaTime);
			break;
		case ServerState::GAME_ENDED:
			readMessage();
			break;
		case ServerState::CLOSED:
			Close();
			break;
		default:
			break;
		}
	}
}

void Server::Close()
{
	_window->close();
	if (!m_isRunning)
		return;
	closesocket(m_serverSocket);
	WSACleanup();
}

void Server::Connect()
{
	Launch(_serverMenu->getIP(), std::stoi(_serverMenu->getPort()));
}

void Server::readMessage()
{
	struct sockaddr_in client;
	int len = sizeof(client);
	char recvbuf[512];

	int recv_len = recvfrom(m_serverSocket, recvbuf, 512, 0, (struct sockaddr*)&client, &len);
	
	if (recv_len == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
			return;
		return;
	}
	std::string clientKey = std::to_string(client.sin_addr.s_addr) + ":" + std::to_string(client.sin_port);
	if (_messageBuffer.find(clientKey) == _messageBuffer.end()) {
		_messageBuffer[clientKey] = std::string(recvbuf, recv_len);
		_clientsMap[clientKey] = client;
		_clientsList.push_back(clientKey);
	} else
		_messageBuffer[clientKey] += std::string(recvbuf, recv_len);

	if (!_messageBuffer[clientKey].empty() && _messageBuffer[clientKey].back() == '\0') {
		json recvbufJson;
		try {
			recvbufJson = json::parse(_messageBuffer[clientKey]);
		} catch (const json::parse_error& e) {
			std::cerr << "JSON Parse Error: " << e.what() << std::endl;
			_messageBuffer[clientKey].clear();
			return;
		}
		decodeClientMessages(clientKey, recvbufJson);
		_messageBuffer[clientKey].clear();
	}
}

void Server::sendMessage(const std::string& message, const std::string& clientId)
{
	std::string cpy = message;
	cpy.push_back('\0');
	if (sendto(m_serverSocket, cpy.c_str(), cpy.size(), 0, (struct sockaddr*)&_clientsMap[clientId], sizeof(_clientsMap[clientId])) == SOCKET_ERROR)
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
	_window->setFramerateLimit(60);
	_serverMenu = std::make_unique<UI_ServerMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { this->Connect(); }, [this]() { this->Close(); });
	_serverRunning = std::make_unique<UI_ServerRunning>(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Server::processEvents()
{
	while (const std::optional event = _window->pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			_state = ServerState::CLOSED;
			Close();
		}
		switch (_state)
		{
		case ServerState::NOT_RUNNING:
			_serverMenu->handleEvent(*event);
			break;
		case ServerState::RUNNING:
			_serverRunning->handleEvent(*event);
			break;
		case ServerState::GAME_STARTED:
			_serverRunning->handleEvent(*event);
			break;
		case ServerState::GAME_ENDED:
			_serverRunning->handleEvent(*event);
			break;
		case ServerState::CLOSED:
			break;
		default:
			break;
		}
	}
}

void Server::render()
{
	_window->clear(sf::Color::Black);
	switch (_state)
	{
	case ServerState::NOT_RUNNING:
		_window->draw(*_serverMenu);
		break;
	case ServerState::RUNNING:
		_window->draw(*_serverRunning);
		break;
	case ServerState::GAME_STARTED:
		_window->draw(*_serverRunning);
		break;
	case ServerState::GAME_ENDED:
		_window->draw(*_serverRunning);
		break;
	case ServerState::CLOSED:
		break;
	default:
		break;
	}
	_window->display();
}

void Server::decodeClientMessages(const std::string& clientName, nlohmann::json messageContent)
{
	if (messageContent["type"] == "connect")
		newClientConnected(clientName, messageContent);
	else if (messageContent["type"] == "input" && _state != ServerState::GAME_ENDED)
		clientIsMoving(clientName, messageContent);
	else if (messageContent["type"] == "disconnect")
		playerDisconnect(clientName, messageContent);
	else if (messageContent["type"] == "restart")
		clientRestart(clientName, messageContent);
}

void Server::playerDisconnect(const std::string& clientId, nlohmann::json messageContent) {
	std::cout << "Player disconnect: " << messageContent << std::endl;
	int disconnectedPlayerId = messageContent["content"]["player_id"];
	if (disconnectedPlayerId == 0) return;
	_players.erase(disconnectedPlayerId);

	auto it = std::find(_clientsList.begin(), _clientsList.end(), clientId);
	if (it != _clientsList.end()) {
		_clientsList.erase(it);
	}
	_clientsMap.erase(clientId);

	std::string disconnectedPlayerName = _clientsNamesList[clientId];
	_clientsNamesList.erase(clientId);

	json disconnectMessage = {
		{"type", "player_disconnected"},
		{"content", {
	            {"player_id", disconnectedPlayerId},
				{"player_name", disconnectedPlayerName}
		}}
	};
	sendMessageToAll(disconnectMessage.dump());

	_serverRunning->addNewClient(disconnectedPlayerName + " has disconnected");
}

void Server::newClientConnected(const std::string& clientId, nlohmann::json messageContent)
{
	int id = (_players.size() < 2) ? _players.size() + 1 : 3;
	_clientsNamesList[clientId] = messageContent["content"]["player_name"];
	if (_players.size() < 2) {
		_players[id] = clientId;
		_serverRunning->addNewClient("Player " + _clientsNamesList[clientId] + " is now connected");
	} else
		_serverRunning->addNewClient("Watcher " + _clientsNamesList[clientId] + " is now connected");
	json messJson = {
		{"type", "connected"},
		{"content" , {{"player_id", id}}} };
	
	sendMessage(messJson.dump(), clientId);
	if (_players.size() == 2 && _state != ServerState::GAME_ENDED)
		startGame();
	/*if (_state == ServerState::GAME_ENDED)
		handleEndGame();*/
}

bool Server::isPlayer(const std::string& valueToFind) {
	for (const auto& pair : _players) {
		if (pair.second == valueToFind) {
			return true;
		}
	}
	return false;
}

void Server::clientRestart(const std::string& clientId, nlohmann::json messageContent)
{
	if (_state != ServerState::RUNNING || isPlayer(clientId))
		return;
	int id = (_players.size() < 2) ? _players.size() + 1 : 3;
	_clientsNamesList[clientId] = messageContent["content"]["player_name"];
	if (_players.size() < 2) {
		_players[id] = clientId;
		_serverRunning->addNewClient("Player " + _clientsNamesList[clientId] + " is trying to restart");
	}
	json messJson = {
		{"type", "connected"},
		{"content" , {{"player_id", id}}} };

	sendMessage(messJson.dump(), clientId);
	if (_players.size() == 2 && _state != ServerState::GAME_ENDED)
		startGame();
}

void Server::clientIsMoving(const std::string& clientName, nlohmann::json messageContent)
{
	int dir = messageContent["content"]["direction"];
	if (_players[1] == clientName)
		_playerOneDir = dir;
	else if (_players[2] == clientName)
		_playerTwoDir = dir;
}

void Server::updatePlayersPosition()
{
	if (_playerOneDir != 0) {
		sf::Vector2f newPos = _playerOnePaddle->getPosition() + (sf::Vector2f{ 0, 1 } *(_playerSpeed * _playerOneDir));
		if (newPos.y >= 0 && newPos.y <= (WINDOW_HEIGHT  - PADDLE_HEIGHT))
			_playerOnePaddle->setPosition(newPos);
	} else if (_playerTwoDir != 0) {
		sf::Vector2f newPos = _playerTwoPaddle->getPosition() + (sf::Vector2f{ 0, 1 } *(_playerSpeed * _playerTwoDir));
		if (newPos.y >= 0 && newPos.y <= (WINDOW_HEIGHT - PADDLE_HEIGHT))
			_playerTwoPaddle->setPosition(newPos);
	}
}

void Server::startGame()
{
	_state = ServerState::GAME_STARTED;
	_playerOneScore = 0;
	_playerTwoScore = 0;
	_ball->reset(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
	json messJson = {
		{"type", "start"},
		{"content", {
			{"player1_name", _clientsNamesList[_players[1]]},
			{"player2_name", _clientsNamesList[_players[2]]}
		}}
	};
	sendMessageToAll(messJson.dump());
	updateGameState();
}

void Server::sendMessageToAll(const std::string& mess)
{
	for (int i = 0; i < _clientsList.size(); i++)
		sendMessage(mess, _clientsList[i]);
}

void Server::updateGameState()
{
	json messJson = {
	{"type", "update"},
	{"content", {
		{"ball", {
			{"x", _ball->getPosition().x},
			{"y", _ball->getPosition().y}
		}},
		{"paddles", {
			{ "1", {{"x", _playerOnePaddle->getPosition().x}, {"y", _playerOnePaddle->getPosition().y}}},
			{"2", {{"x", _playerTwoPaddle->getPosition().x}, {"y", _playerTwoPaddle->getPosition().y}}}
		}},
		{"score", {
			{"1", _playerOneScore},
			{"2", _playerTwoScore}
		}}
	}}
	};
	sendMessageToAll(messJson.dump());
}

void Server::update(float deltatime)
{
	if (_state != ServerState::GAME_STARTED)
		return;
	_ball->update(BALL_SPEED * deltatime);
	checkScore();
	updatePlayersPosition();
	updateGameState();
	checkPaddleCollision(*_ball, *_playerOnePaddle, true);
	checkPaddleCollision(*_ball, *_playerTwoPaddle, false);
	if (_state == ServerState::GAME_ENDED)
		handleEndGame();
}

void Server::handleEndGame()
{
	if (_state != ServerState::GAME_ENDED)
		return;
	json gameOverMessage = {
				{"type", "game_over"},
				{"content", {
						{"winner", (_playerOneScore >= WINNING_SCORE) ? 1 : 2}
				}}
	};
	sendMessageToAll(gameOverMessage.dump());
	resetGame();
	_state = ServerState::RUNNING;
}

void Server::resetGame()
{
	_players.clear();
	_playerOneScore = 0;
	_playerTwoScore = 0;
	_playerOneDir = 0;
	_playerTwoDir = 0;
	_playerOnePaddle->reset(30.0f, WINDOW_HEIGHT / 2);
	_playerTwoPaddle->reset(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT / 2);
	_ball->reset(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}

void Server::checkPaddleCollision(Ball& ball, const Paddle& paddle, bool isLeftPaddle) {
	auto ballPos = ball.getPosition();
	auto paddlePos = paddle.getPosition();

	float ballCenterX = ballPos.x + ball.getRadius();
	float ballCenterY = ballPos.y + ball.getRadius();

	if (isLeftPaddle) {
		if (ballCenterX - ball.getRadius() <= paddlePos.x + PADDLE_WIDTH/2 &&
			ballCenterX >= paddlePos.x - PADDLE_WIDTH/2) {
			if (ballCenterY >= paddlePos.y - PADDLE_HEIGHT/2 && ballCenterY <= paddlePos.y + PADDLE_HEIGHT/2) {
				ball.setPosition({paddlePos.x + PADDLE_WIDTH/2 + ball.getRadius() - (ballCenterX - ballPos.x), ballPos.y});
				ball.reverseXVelocity();
				float relativeIntersectY = (ballCenterY - paddlePos.y) / (PADDLE_HEIGHT/2);
				ball.setVelocityY(relativeIntersectY * BALL_SPEED);
			}
		}
	} else {
		if (ballCenterX + ball.getRadius() >= paddlePos.x - PADDLE_WIDTH/2 &&
			ballCenterX <= paddlePos.x + PADDLE_WIDTH/2) {
			if (ballCenterY >= paddlePos.y - PADDLE_HEIGHT/2 && ballCenterY <= paddlePos.y + PADDLE_HEIGHT/2) {
				ball.setPosition({paddlePos.x - PADDLE_WIDTH/2 - ball.getRadius() - (ballCenterX - ballPos.x), ballPos.y});
				ball.reverseXVelocity();
				float relativeIntersectY = (ballCenterY - paddlePos.y) / (PADDLE_HEIGHT/2);
				ball.setVelocityY(relativeIntersectY * BALL_SPEED);
			}
		}
	}
}

void Server::checkScore() {
	auto ballPos = _ball->getPosition();
	bool scoreChanged = false;
	int scoringPlayer = 0;

	if (ballPos.x <= 0) {
		_playerTwoScore++;
		scoringPlayer = 2;
		_ball->reset(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		scoreChanged = true;
	}
	else if (ballPos.x + BALL_RADIUS * 2 >= WINDOW_WIDTH) {
		_playerOneScore++;
		scoringPlayer = 1;
		_ball->reset(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		scoreChanged = true;
	}

	if (scoreChanged) {
		if (_playerOneScore >= WINNING_SCORE || _playerTwoScore >= WINNING_SCORE) {
			_state = ServerState::GAME_ENDED;
			_serverRunning->addNewClient(_clientsNamesList[_players[scoringPlayer]] + " is the winner");
		} else {
			json scoreMessage = {
				{"type", "score"},
				{"content", {
	                    {"new_score", {
	                        {"1", _playerOneScore},
							{"2", _playerTwoScore}
	                    }}
				}}
			};
			sendMessageToAll(scoreMessage.dump());
		}
	}
}

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
	std::cout << "Socket created." << std::endl;

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
			update(deltaTime);
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
	else if (messageContent["type"] == "input") {
		handlePaddleInput(clientName, messageContent["content"]["direction"]);
	}
}

void Server::newClientConnected(const std::string& clientId, nlohmann::json messageContent)
{
	int id = (_players.size() < 2) ? _players.size() + 1 : 3;
	_clientsNamesList[clientId] = messageContent["content"]["player_name"];
	std::cout << "Client name: " + _clientsNamesList[clientId] + "connected" << std::endl;
	if (_players.size() < 2) {
		_players[id] = clientId; // _clientsMap[clientId];
		std::cout << "player: " << id << " connected" << std::endl;
	}
	json messJson = {
		{"type", "connected"},
		{"content" , {{"player_id", id}}} };
	sendMessage(messJson.dump(), clientId);
	if (_players.size() == 2)
		startGame();
}

void Server::handlePaddleInput(const std::string& clientId, int direction) {
	std::cout << "[Server] Input from: " << clientId << ", direction: " << direction << std::endl;

	int playerId = 0;
	for (const auto& [id, key] : _players) {
		if (key == clientId) {
			playerId = id;
			break;
		}
	}

	std::cout << "------ [Server] Processing input for Player " << playerId << "-------\n";

	float newPos;
	if (playerId == 1) {
		newPos = _playerOnePaddle->getPosition().y + direction * PADDLE_SPEED * (1.0f/60.0f);
		newPos = std::max(PADDLE_HEIGHT/2.0f, std::min(newPos, WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f));
		_playerOnePaddle->setPosition({0, newPos});
		std::cout << "[Server] Processing input for Player " << playerId << "\n";
	}
	else if (playerId == 2) {
		newPos = _playerTwoPaddle->getPosition().y + direction * PADDLE_SPEED * (1.0f/60.0f);
		newPos = std::max(PADDLE_HEIGHT/2.0f, std::min(newPos, WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f));
		_playerTwoPaddle->setPosition({0, newPos});
		std::cout << "[Server] Processing input for Player " << playerId << "\n";
	}
}

void Server::startGame()
{
	_state = ServerState::GAME_STARTED;
	json messJson = {
		{"type", "start"},
		{"content" , {{ {{"player1_name", _clientsNamesList[_players[1]]}},
		{{"player2_name", _clientsNamesList[_players[2]]}}}} } };
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
			{"1", {{"y", _playerOnePaddle->getPosition().y}}},
			{"2", {{"y", _playerTwoPaddle->getPosition().y}}}
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
	_ball->update(BALL_SPEED * deltatime);
	updateGameState();

	checkPaddleCollision(*_ball, *_playerOnePaddle, true);
	checkPaddleCollision(*_ball, *_playerTwoPaddle, false);
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

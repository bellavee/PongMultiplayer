#include "Server.h"
#include "resources.h"

using json = nlohmann::json;

Server::Server() 
	: _window(std::make_unique<sf::RenderWindow>(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Server"))
	, _serverMenu(nullptr)
	, _serverRunning(nullptr)
	, m_isRunning(false)
	, _state(ServerState::NOT_RUNNING)
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
	m_isRunning = true;
	_serverRunning->setContent("Server running at " + ipAddress + " at port " + std::to_string(port));
	_state = ServerState::RUNNING;
}

void Server::Run()
{
	std::cout << "Fran " << std::endl;
	while (_window->isOpen()) {
		processEvents();
		switch (_state)
		{
		case ServerState::NOT_RUNNING:
			break;
		case ServerState::RUNNING:
			readMessage();
			break;
		case ServerState::CLOSED:
			Close();
			break;
		default:
			break;
		}
		render();

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
	std::string recvbuf(512, '\0');

	std::cout << "Waiting for data...\n";
	int recv_len = recvfrom(m_serverSocket, &recvbuf[0], 512, 0, (struct sockaddr*)&client, &len);

	if (recv_len == SOCKET_ERROR) {
		std::cerr << "ReadMessage failed: " << WSAGetLastError() << std::endl;
		return;
	}
	recvbuf.resize(recv_len);
	json recvbufJson = json::parse(recvbuf);
	std::cout << "Data: " << recvbufJson.dump(4) << std::endl;
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
	case ServerState::CLOSED:
		break;
	default:
		break;
	}
	_window->display();
}


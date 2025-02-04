
#include "Game.h"

#include <iostream>
#include <sstream>

Game::Game() {
    _window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), WINDOW_TITLE
    );
    _window->setFramerateLimit(60);

    _playerPaddle = std::make_unique<Paddle>(30.0f, WINDOW_HEIGHT / 2);
    _opponentPaddle = std::make_unique<Paddle>(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT / 2);
    _ball = std::make_unique<Ball>(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    _playerScore = std::make_unique<Score>(WINDOW_WIDTH / 4, 50.0f);
    _opponentScore = std::make_unique<Score>(3 * WINDOW_WIDTH / 4, 50.0f);

	_mainMenu = std::make_unique<UI_MainMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { join(); }, [this]() { quit(); });
	_pauseMenu = std::make_unique<UI_PauseMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { resumeGame(); }, [this]() { backToMenu(); });
    _lostConnectionPopup = std::make_unique<UI_LostConnection>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { backToMenu(); });

    _playerPaddle->setColor(sf::Color(66, 135, 245));
    _opponentPaddle->setColor(sf::Color(245, 66, 66));
    _ball->setColor(sf::Color(245, 197, 66));

    if (!_playerScore->loadFont(BASE_FONT_PATH) ||
       !_opponentScore->loadFont(BASE_FONT_PATH)) {
        throw std::runtime_error("Failed to load font file");
    }

	_winsockClient = std::make_unique<WinsockClient>();
}

Game::~Game() {
	if (_winsockClient && _winsockClient->isConnected()) {
		_winsockClient->disconnect();
	}
}

void Game::run() {
    sf::Clock clock;

    while (_window->isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        processEvents();

		if (_state == GameState::Playing) {
			update(deltaTime);
		}

    	if (_state == GameState::Waiting) {
    		checkForPlayers();
    	}

        render();
    }
}

void Game::join()
{
	if (!_winsockClient->initialize()) return;

	if (!_winsockClient->connectToServer("127.0.0.1", "2222")) return;

	_winsockClient->sendData("CONNECT");
	waitingGame();
}

void Game::backToMenu()
{
    _state = GameState::MainMenu;

	_playerScore->reset();
	_opponentScore->reset();
	resetBall();
}

void Game::quit()
{
    _window->close();
}

void Game::processEvents() {
    while (const std::optional event = _window->pollEvent()) {
        if (event->is<sf::Event::Closed>())
            _window->close();

		if (_state == GameState::MainMenu) {
			if (event->is<sf::Event::MouseButtonPressed>())
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
				_mainMenu->handleInput(mousePos);
			}
		}

		if (_state == GameState::Paused) {
			if (event->is<sf::Event::MouseButtonPressed>())
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
				_pauseMenu->handleInput(mousePos);
			}
		}
		
        if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
			if (_state == GameState::Playing)
				_state = GameState::Paused;
			else if (_state == GameState::Paused)
				_state = GameState::Playing;
        }
        
    }
}

void Game::update(float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        _playerPaddle->move(-400.0f * deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        _playerPaddle->move(400.0f * deltaTime);

    // _ball->update(deltaTime);
    // handleCollisions();

    _playerPaddle->keepInBounds(0.0f, WINDOW_HEIGHT);
    // _opponentPaddle->keepInBounds(0.0f, WINDOW_HEIGHT);

	sendPlayerData();
	processServerMessages();
}

void Game::handleCollisions() {
    if (_ball->getPosition().y <= 0 ||
        _ball->getPosition().y + _ball->getRadius() >= WINDOW_HEIGHT) {
        _ball->reverseYVelocity();
    }

    if (_ball->getGlobalBounds().findIntersection(_playerPaddle->getGlobalBounds()) ||
        _ball->getGlobalBounds().findIntersection(_opponentPaddle->getGlobalBounds())) {
        _ball->reverseXVelocity();
    }

    if (_ball->getPosition().x <= 0) {
        _opponentScore->increment();
        resetBall();
    }
    else if (_ball->getPosition().x + _ball->getRadius() >= WINDOW_WIDTH) {
        _playerScore->increment();
        resetBall();
    }
}

void Game::resetBall() {
    _ball->reset(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}

void Game::checkForPlayers() {
	std::string response = _winsockClient->receiveData();
	if (response.substr(0, 9) == "CONNECTED:") {
		std::cout << response << std::endl;
		int playerCount = std::stoi(response.substr(9));
		std::cout << "Connected. Current players: " << playerCount << std::endl;
	}

	else if (response == "PLAYERS_READY") {
		std::cout << response << std::endl;
		std::cout << "All players ready! Starting game..." << std::endl;
	}
}

void Game::processServerMessages() {
	if (!_winsockClient || !_winsockClient->isConnected()) return;

	std::string message = _winsockClient->receiveData();
	if (message.empty()) return;

	if (message.substr(0, 6) == "STATE:") {
		// Format: "STATE:ballX-0,ballY-1,player1PosY-2,player2PosY-3, player1Score-4,player2Score-5"
		try {
			std::string data = message.substr(6);

			std::stringstream ss(data);
			std::vector<float> values;
			std::string token;

			while (std::getline(ss, token, ',')) {
				values.push_back(std::stof(token));
			}

			if (values.size() >= 6) {
				_ball->setPosition({values[0], values[1]});
				_playerPaddle->setPosition({30.0f, values[2]});
				_opponentPaddle->setPosition({WINDOW_WIDTH - 30.0f, values[3]});
				_playerScore->setValue(static_cast<int>(values[4]));
				_opponentScore->setValue(static_cast<int>(values[5]));
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Error parsing state: " << e.what() << std::endl;
		}
	}
}

void Game::sendPlayerData() {
	if (_winsockClient && _winsockClient->isConnected()) {
		std::string paddleMsg = "PADDLE:" + std::to_string(_playerPaddle->getPosition().y);
		_winsockClient->sendData(paddleMsg);
	}
}

void Game::render() {
    _window->clear(sf::Color::Black);

    if (_state == GameState::MainMenu) {
        _window->draw(*_mainMenu);
    }
	if (_state == GameState::Playing || _state == GameState::Paused)
	{
		auto centerLine = std::make_unique<sf::RectangleShape>(sf::Vector2f(2.0f, WINDOW_HEIGHT));
		centerLine->setPosition({ static_cast<float>(WINDOW_WIDTH / 2), 0 });
		centerLine->setFillColor(sf::Color::White);
		_window->draw(*centerLine);

		_window->draw(*_playerPaddle);
		_window->draw(*_opponentPaddle);
		_window->draw(*_ball);
		_window->draw(*_playerScore);
		_window->draw(*_opponentScore);
	}
	if (_state == GameState::Paused) {
		_window->draw(*_pauseMenu);
	}
	if (_state == GameState::Waiting) {
		auto centerLine = std::make_unique<sf::RectangleShape>(sf::Vector2f(2.0f, WINDOW_HEIGHT));
		centerLine->setPosition({ static_cast<float>(WINDOW_WIDTH / 2), 0 });
		centerLine->setFillColor(sf::Color::White);
		_window->draw(*centerLine);
		_window->draw(*_playerPaddle);
		_window->draw(*_opponentPaddle);
	}

    _window->display();
}

#include "Game.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

Game::Game() {
    _window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), WINDOW_TITLE
    );
    _window->setFramerateLimit(60);

    _player1Paddle = std::make_unique<Paddle>(0.0f, WINDOW_HEIGHT / 2);
    _player2Paddle = std::make_unique<Paddle>(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT / 2);
    _ball = std::make_unique<Ball>(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    _player1Score = std::make_unique<Score>(WINDOW_WIDTH / 5, 50.0f);
    _player2Score = std::make_unique<Score>(3 * WINDOW_WIDTH / 5, 50.0f);

	_mainMenu = std::make_unique<UI_MainMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { join(); }, [this]() { quit(); });
	_pauseMenu = std::make_unique<UI_PauseMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { resumeGame(); }, [this]() { backToMenu(); });
    _lostConnectionPopup = std::make_unique<UI_LostConnection>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { backToMenu(); });
	_waitingPlayerPopup = std::make_unique<UI_WaitingPlayer>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { backToMenu(); });
	_playMenu = std::make_unique<UI_PlayMenu>();
	_lostConnectionPopup = std::make_unique<UI_LostConnection>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { backToMenu(); });

    _player1Paddle->setColor(sf::Color(66, 135, 245)); // blue
    _player2Paddle->setColor(sf::Color(245, 66, 66)); // red
    _ball->setColor(sf::Color(245, 197, 66)); // yellow

	_player1Score->setPlayerName("Player1");
	_player2Score->setPlayerName("Player2");

    if (!_player1Score->loadFont(BASE_FONT_PATH) ||
       !_player2Score->loadFont(BASE_FONT_PATH)) {
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

    	if (_state == GameState::Waiting) {
    		checkForPlayers();
    	}

		if (_state == GameState::Playing || _state == GameState::Paused) {
			update(deltaTime);
		}

        render();
    }
}

void Game::join()
{	
	if (!_winsockClient->initialize()) return;

	if (!_winsockClient->connectToServer(_mainMenu->getIP(), _mainMenu->getPort())) return;

	json message = {
		{"type", "connect"},
		{"content", {
	            {"player_name", _mainMenu->getClientName()} // placeholder
		}}
	};

	_winsockClient->sendData(message.dump());
	waitingGame();
}

void Game::resumeGame() {
	if (!_winsockClient) {
		return;
	}

	if (!_winsockClient->isConnected()) {
		return;
	}

	json message = { {"type", "resume"} };
	_winsockClient->sendData(message.dump());
}




void Game::backToMenu()
{
    _state = GameState::MainMenu;

	_player1Score->reset();
	_player2Score->reset();
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
        
		switch (_state) {
		case GameState::MainMenu:
			_mainMenu->handleEvent(*event);
			break;
		case GameState::Paused:
			_pauseMenu->handleEvent(*event);
			break;
		case GameState::LostConnection:
			_lostConnectionPopup->handleEvent(*event);
			break;
		case GameState::Waiting:
			_waitingPlayerPopup->handleEvent(*event);
			break;
		}
		if (_state == GameState::Playing) {
			if (event->is<sf::Event::KeyPressed>()) {
				auto keyEvent = event->getIf<sf::Event::KeyPressed>();
				if (keyEvent->code == sf::Keyboard::Key::Up || keyEvent->code == sf::Keyboard::Key::Z)
					sendPlayerData(-1);
				else if (keyEvent->code == sf::Keyboard::Key::Down || keyEvent->code == sf::Keyboard::Key::S)
					sendPlayerData(1);
			}
			else if (event->is<sf::Event::KeyReleased>()) {
				auto keyEvent = event->getIf<sf::Event::KeyReleased>();
				if (keyEvent->code == sf::Keyboard::Key::Up || keyEvent->code == sf::Keyboard::Key::Z ||
					keyEvent->code == sf::Keyboard::Key::Down || keyEvent->code == sf::Keyboard::Key::S)
					sendPlayerData(0);
			}
		}
		if (_state == GameState::Playing) {
			if (event->is<sf::Event::KeyPressed>()) {
				auto keyEvent = event->getIf<sf::Event::KeyPressed>();
				if (keyEvent->code == sf::Keyboard::Key::Escape) {
					sendPauseRequest(); // Demande de pause au serveur
				}
			}
		}
		if (_state == GameState::Paused) {
			if (event->is<sf::Event::KeyPressed>()) {
				auto keyEvent = event->getIf<sf::Event::KeyPressed>();
				if (keyEvent->code == sf::Keyboard::Key::Escape) {
					resumeGame(); // Demande de pause au serveur
				}
			}
		}


    }
}

void Game::sendPauseRequest() {
	if (_winsockClient && _winsockClient->isConnected()) {
		json message = {
			{"type", "pause"}  // Envoie au serveur
		};
		_winsockClient->sendData(message.dump());
	}
}


void Game::update(float deltaTime) {
	//if (_state != GameState::Playing) return;
	processServerMessages();
}

void Game::handleCollisions() {
}

void Game::resetBall() {
    _ball->reset(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}

void Game::checkForPlayers() {
	if (!_winsockClient || !_winsockClient->isConnected()) return;

	std::string messageStr = _winsockClient->receiveData();
	if (messageStr.empty()) return;
	//std::cout << "[checkForPlayers] " << messageStr << std::endl;

	try {
		json message = json::parse(messageStr);
		std::string type = message["type"];
		if (type == "connected") {
			_playerId = message["content"]["player_id"];
			switch (_playerId)
			{
			case 1:
				_player1Score->setPlayerName(_mainMenu->getClientName());
				break;
			default:
				break;
			}
			std::cout << "Connected as player " << _playerId << std::endl;
		} else if (type == "start") {
			_player1Score->setPlayerName(message["content"]["player1_name"]);
			_player2Score->setPlayerName(message["content"]["player2_name"]);
			startGame();
			std::cout << "Start game " << std::endl;
		}

	} catch (const json::exception& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error processing message: " << e.what() << std::endl;
	}
}

void Game::processServerMessages() {
	if (!_winsockClient || !_winsockClient->isConnected()) return;

	std::string messageStr = _winsockClient->receiveData();
	if (messageStr.empty()) return;

	// auto [command, data] = parseCommand(messageStr);
	//std::cout << messageStr << std::endl;

	try {
		json message = json::parse(messageStr);
		std::string type = message["type"];

		if (type == "update") {
			// Ball
            _ball->setPosition({message["content"]["ball"]["x"], message["content"]["ball"]["y"]});

			// Paddle
            auto& paddles = message["content"]["paddles"];
            for (const auto& [playerId, paddleData] : paddles.items()) {
				float paddleY = paddleData["y"];
				float paddleX = paddleData["x"];

				switch (std::stoi(playerId))
				{
				case 1:
					_player1Paddle->setPosition(sf::Vector2f(paddleX, paddleY));
					break;
				case 2:
					_player2Paddle->setPosition(sf::Vector2f(paddleX, paddleY));
					break;
				default:
					break;
				}
            }

			// Score
            auto& scores = message["content"]["score"];
            for (const auto& [playerId, score] : scores.items()) {
				if (std::stoi(playerId) == 1) {
					_player1Score->update(score);
				}
				else if (std::stoi(playerId) == 2) {
					_player2Score->update(score);
				}
            }
        }
        else if (type == "score") {
            auto& newScores = message["content"]["new_score"];
            for (const auto& [playerId, score] : newScores.items()) {
                if (std::stoi(playerId) == 1) {
                    _player1Score->update(score);
                } else if (std::stoi(playerId) == 2) {
                    _player2Score->update(score);
                }
            }
        }
        else if (type == "game_over") {
            int winner = message["content"]["winner"];
			_playMenu->setContent(((winner == 1) ? _player1Score->getPlayerName() : _player2Score->getPlayerName()) + " is the winner !");
            //backToMenu();
        }
		else if (type == "pause") {
			_state = GameState::Paused;
		}
		else if (type == "resume") {
			_state = GameState::Playing;
		}


	} catch (const json::exception& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error processing message: " << e.what() << std::endl;
	}
}

void Game::sendPlayerData(int direction) {
	if (_winsockClient && _winsockClient->isConnected()) {
		json message = {
			{"type", "input"},
			{"content", {
	                {"direction", direction}
			}}
		};
		_winsockClient->sendData(message.dump());
	}
}

void Game::render() {
    _window->clear(sf::Color::Black);
        
	if (_state != GameState::MainMenu)
	{
		auto centerLine = std::make_unique<sf::RectangleShape>(sf::Vector2f(2.0f, WINDOW_HEIGHT));
		centerLine->setPosition({ static_cast<float>(WINDOW_WIDTH / 2), 0 });
		centerLine->setFillColor(sf::Color::White);
		_window->draw(*centerLine);

		_window->draw(*_player1Paddle);
		_window->draw(*_player2Paddle);
		_window->draw(*_ball);
		_window->draw(*_player1Score);
		_window->draw(*_player2Score);
	}

    switch (_state)
    {
    case GameState::MainMenu:
		_window->draw(*_mainMenu);
        break;
    case GameState::Paused:
		_window->draw(*_pauseMenu);
        break;
    case GameState::LostConnection:
		_window->draw(*_lostConnectionPopup);
        break;
	case GameState::Playing:
		_window->draw(*_playMenu);
		break;

	 case GameState::Waiting:
	  {
		  _window->draw(*_waitingPlayerPopup);
		  break;
	  }
    default:
        break;
    }

    _window->display();
}

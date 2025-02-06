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

    _player1Paddle = std::make_unique<Paddle>(30.0f, WINDOW_HEIGHT / 2);
    _player2Paddle = std::make_unique<Paddle>(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT / 2);
    _ball = std::make_unique<Ball>(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    _player1Score = std::make_unique<Score>(WINDOW_WIDTH / 5, 50.0f);
    _player2Score = std::make_unique<Score>(3 * WINDOW_WIDTH / 5, 50.0f);

	_mainMenu = std::make_unique<UI_MainMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { join(); }, [this]() { quit(); });
	_pauseMenu = std::make_unique<UI_PauseMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { resumeGame(); }, [this]() { backToMenu(); });
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

		if (_state == GameState::Playing) {
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
		}
		
       /* if (event->is<sf::Event::KeyPressed>() && isKeyPressed(sf::Keyboard::Key::Escape))
        {
			if (_state == GameState::Playing)
				_state = GameState::Paused;
			else if (_state == GameState::Paused)
				_state = GameState::Playing;
        }*/

		if (event->is<sf::Event::KeyPressed>())
		{
			if ((isKeyPressed(sf::Keyboard::Key::Up) || isKeyPressed(sf::Keyboard::Key::Z)) && _state == GameState::Playing) {
				sendPlayerData(-1);
			} else if ((isKeyPressed(sf::Keyboard::Key::Down) || isKeyPressed(sf::Keyboard::Key::S)) && _state == GameState::Playing) {
				sendPlayerData(1);
			}
			//simulate Lost Connection with L Key
			//if (isKeyPressed(sf::Keyboard::Key::L))
			//	if (_state == GameState::Playing)
			//		_state = GameState::LostConnection;

			////simulate reconnection with R Key
   //         if (isKeyPressed(sf::Keyboard::Key::R))
			//    if (_state == GameState::LostConnection)
			//	    _state = GameState::Playing;
		}
        
    }
}

void Game::update(float deltaTime) {
	if (_state != GameState::Playing) return;

	//int direction = 0;
	//if (isKeyPressed(sf::Keyboard::Key::W) || isKeyPressed(sf::Keyboard::Key::Z)) {
	//	direction = -1;
	//	_predictedPaddleY -= _paddleSpeed * deltaTime;
	//} else if (isKeyPressed(sf::Keyboard::Key::S)) {
	//	direction = 1;
	//	_predictedPaddleY += _paddleSpeed * deltaTime;
	//}

	//// for debug
	//_predictedPaddleY = std::max(PADDLE_HEIGHT/2.0f, std::min(_predictedPaddleY, static_cast<float>(WINDOW_HEIGHT) - PADDLE_HEIGHT/2.0f));

	///*if (_playerId == 1) {
	//	_playerPaddle->setPosition({30.0f, _predictedPaddleY});
	//} else {
	//	_playerPaddle->setPosition({WINDOW_WIDTH - 30.0f, _predictedPaddleY});
	//}*/

	//if (direction != _lastDirection) {
	//	sendPlayerData(direction);
	//	_lastDirection = direction;
	//}
	processServerMessages();
}

void Game::handleCollisions() { // not use here
    //if (_ball->getPosition().y <= 0 ||
    //    _ball->getPosition().y + _ball->getRadius() >= WINDOW_HEIGHT) {
    //    _ball->reverseYVelocity();
    //}

    //if (_ball->getGlobalBounds().findIntersection(_playerPaddle->getGlobalBounds()) ||
    //    _ball->getGlobalBounds().findIntersection(_opponentPaddle->getGlobalBounds())) {
    //    _ball->reverseXVelocity();
    //}

    //if (_ball->getPosition().x <= 0) {
    //    _opponentScore->increment();
    //    resetBall();
    //}
    //else if (_ball->getPosition().x + _ball->getRadius() >= WINDOW_WIDTH) {
    //    _playerScore->increment();
    //    resetBall();
    //}
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
			std::cout << "Connected as player " << _playerId << std::endl;
		} else if (type == "start") {
			_player1Score->setPlayerName(message["content"]["player1_name"]);
			_player2Score->setPlayerName(message["content"]["player2_name"]);
			startGame();
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
				/*if (std::stoi(playerId) == 1) {
                    if (_playerId == 1) {
                        float currentY = _playerPaddle->getPosition().y;
                        if (std::abs(paddleY - currentY) > 10.0f) {
                            _playerPaddle->setPosition({30.0f, paddleY});
                        }
                    } else {
                        _opponentPaddle->setPosition({30.0f, paddleY});
                    }
                }
                else if (std::stoi(playerId) == 2) {
                    if (_playerId == 2) {
                        float currentY = _playerPaddle->getPosition().y;
                        if (std::abs(paddleY - currentY) > 10.0f) {
                            _playerPaddle->setPosition({WINDOW_WIDTH - 30.0f, paddleY});
                        }
                    } else {
                        _opponentPaddle->setPosition({WINDOW_WIDTH - 30.0f, paddleY});
                    }
                } */// to do handle id = 3 case
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
            backToMenu();
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

	  case GameState::Waiting:
	  {
		  // black window
		  break;
	  }
    default:
        break;
    }

    _window->display();
}

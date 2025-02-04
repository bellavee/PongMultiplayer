
#include "Game.h"

#include <iostream>

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
}

void Game::run() {
    sf::Clock clock;

    while (_window->isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        processEvents();
		if (_state == GameState::Playing) {
			update(deltaTime);
		}
        render();
    }
}

void Game::join()
{
	if (_mainMenu->getIP() == "localhost") { // TODO: Connexion a l'ip
		startGame();
	}
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
		
        if (event->is<sf::Event::KeyPressed>() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
			if (_state == GameState::Playing)
				_state = GameState::Paused;
			else if (_state == GameState::Paused)
				_state = GameState::Playing;
        }

		if (event->is<sf::Event::KeyPressed>())
		{
			//simulate Lost Connection with L Key
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
				if (_state == GameState::Playing)
					_state = GameState::LostConnection;

			//simulate reconnection with R Key
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
			    if (_state == GameState::LostConnection)
				    _state = GameState::Playing;
		}
        
    }
}

void Game::update(float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        _playerPaddle->move(-400.0f * deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        _playerPaddle->move(400.0f * deltaTime);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        _opponentPaddle->move(-400.0f * deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        _opponentPaddle->move(400.0f * deltaTime);

    _ball->update(deltaTime);

    handleCollisions();

    _playerPaddle->keepInBounds(0.0f, WINDOW_HEIGHT);
    _opponentPaddle->keepInBounds(0.0f, WINDOW_HEIGHT);
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

void Game::render() {
    _window->clear(sf::Color::Black);
        
	if (_state != GameState::MainMenu)
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
    default:
        break;
    }

    _window->display();
}
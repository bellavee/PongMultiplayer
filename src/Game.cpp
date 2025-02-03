
#include "Game.h"

#include <iostream>

Game::Game() {
    _window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Pong"
    );
    _window->setFramerateLimit(60);

    _playerPaddle = std::make_unique<Paddle>(30.0f, WINDOW_HEIGHT / 2);
    _opponentPaddle = std::make_unique<Paddle>(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT / 2);
    _ball = std::make_unique<Ball>(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    _playerScore = std::make_unique<Score>(WINDOW_WIDTH / 4, 50.0f);
    _opponentScore = std::make_unique<Score>(3 * WINDOW_WIDTH / 4, 50.0f);

	_mainMenu = std::make_unique<UI_MainMenu>(WINDOW_WIDTH, WINDOW_HEIGHT, [this]() { join(); }, [this]() { quit(); });

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
        update(deltaTime);
        render();
    }
}

void Game::join()
{
	std::cout << "Joining game" << std::endl;
}

void Game::quit()
{
    _window->close();
}

void Game::processEvents() {
    while (const std::optional event = _window->pollEvent()) {
        if (event->is<sf::Event::Closed>())
            _window->close();

        if (event->is<sf::Event::MouseButtonPressed>())
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
            _mainMenu->handleInput(mousePos);
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

    auto centerLine = std::make_unique<sf::RectangleShape>(sf::Vector2f(2.0f, WINDOW_HEIGHT));
    centerLine->setPosition({static_cast<float>(WINDOW_WIDTH / 2), 0});
    centerLine->setFillColor(sf::Color::White);
    _window->draw(*centerLine);

    _window->draw(*_playerPaddle);
    _window->draw(*_opponentPaddle);
    _window->draw(*_ball);
    _window->draw(*_playerScore);
    _window->draw(*_opponentScore);
    _window->draw(*_mainMenu);

    _window->display();
}
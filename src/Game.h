﻿//
// Created by Bella on 2/2/2025.
//

#ifndef GAME_H
#define GAME_H

#include "resources.h"

#include <SFML/Graphics.hpp>
#include <memory>
#include "Paddle.h"
#include "Ball.h"
#include "Score.h"
#include "UI_MainMenu.h"
#include "UI_PauseMenu.h"
#include "UI_LostConnection.h"
#include "WinsockClient.h"

enum class GameState {
	MainMenu,
	Playing,
	Paused,
    Waiting
};

class Game {
public:
    Game();
    ~Game();
    void run();

    void join();
    void startGame() { _state = GameState::Playing; }
    void resumeGame() { _state = GameState::Playing; }
    void waitingGame() { _state = GameState::Waiting; }
    void backToMenu();
    void quit();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void handleCollisions();
    void resetBall();
    void checkForPlayers();
    void processServerMessages();

    void sendPlayerData();

private:
    std::unique_ptr<sf::RenderWindow> _window;
    std::unique_ptr<Paddle> _playerPaddle;
    std::unique_ptr<Paddle> _opponentPaddle;
    std::unique_ptr<Ball> _ball;
    std::unique_ptr<Score> _playerScore;
    std::unique_ptr<Score> _opponentScore;
    std::unique_ptr<UI_MainMenu> _mainMenu;
    std::unique_ptr<UI_PauseMenu> _pauseMenu;
    std::unique_ptr<UI_LostConnection> _lostConnectionPopup;
    std::unique_ptr<WinsockClient> _winsockClient;

    GameState _state = GameState::MainMenu;
};


#endif //GAME_H

//
// Created by Bella on 2/2/2025.
//

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Paddle.h"
#include "Ball.h"
#include "Score.h"

class Game {
public:
    Game();
    ~Game() = default;
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void handleCollisions();
    void resetBall();

private:
    std::unique_ptr<sf::RenderWindow> _window;
    std::unique_ptr<Paddle> _playerPaddle;
    std::unique_ptr<Paddle> _opponentPaddle;
    std::unique_ptr<Ball> _ball;
    std::unique_ptr<Score> _playerScore;
    std::unique_ptr<Score> _opponentScore;

    unsigned WINDOW_WIDTH = 1200.0f;
    unsigned WINDOW_HEIGHT = 700.0f;

};


#endif //GAME_H

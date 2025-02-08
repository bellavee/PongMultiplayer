#include "Paddle.h"

Paddle::Paddle(float x, float y) {
    _rectangleShape = new sf::RectangleShape(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    _shape = _rectangleShape;
    setPosition({x - PADDLE_WIDTH/2, y - PADDLE_HEIGHT/2});
    _rectangleShape->setFillColor(sf::Color::White);
}

Paddle::~Paddle() {
    delete _rectangleShape;
}

void Paddle::move(float offset) {
    Entity::move({0, offset});
}

void Paddle::keepInBounds(float minY, float maxY) {
    sf::Vector2f pos = getPosition();
    if (pos.y < minY)
        setPosition({pos.x, minY});
    else if (pos.y + PADDLE_WIDTH > maxY)
        setPosition({pos.x, maxY - PADDLE_HEIGHT});
}

float Paddle::getHeight() const {
    return PADDLE_HEIGHT;
}

void Paddle::reset(float x, float y)
{
    setPosition({ x - PADDLE_WIDTH / 2, y - PADDLE_HEIGHT / 2 });
}

#include "Paddle.h"

Paddle::Paddle(float x, float y) {
    _rectangleShape = new sf::RectangleShape(sf::Vector2f(WIDTH, HEIGHT));
    _shape = _rectangleShape;
    setPosition({x - WIDTH/2, y - HEIGHT/2});
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
    else if (pos.y + HEIGHT > maxY)
        setPosition({pos.x, maxY - HEIGHT});
}

float Paddle::getHeight() const {
    return HEIGHT;
}
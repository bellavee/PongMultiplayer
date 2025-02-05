#include "Ball.h"
#include <random>

Ball::Ball(float x, float y) {
    _circleShape = new sf::CircleShape(BALL_RADIUS);
    _shape = _circleShape;
    reset(x, y);
    _circleShape->setFillColor(sf::Color::White);
}

Ball::~Ball() {
    delete _circleShape;
}

void Ball::update(float deltaTime) {
    move(_velocity * deltaTime);
}

void Ball::reset(float x, float y) {
    setPosition({x - BALL_RADIUS/2, y - BALL_RADIUS/2});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(-45.0f, 45.0f);
    float angle = angleDist(gen);

    float radians = angle * 3.14159f / 180.0f;
    _velocity.x = BALL_RADIUS * std::cos(radians);
    _velocity.y = BALL_RADIUS * std::sin(radians);

    if (std::uniform_int_distribution<>(0, 1)(gen) == 0)
        _velocity.x = -_velocity.x;
}

void Ball::reverseXVelocity() {
    _velocity.x = -_velocity.x;
}

void Ball::reverseYVelocity() {
    _velocity.y = -_velocity.y;
}

float Ball::getRadius() const {
    return _circleShape->getRadius();
}
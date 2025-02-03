
#include "Entity.h"

sf::FloatRect Entity::getGlobalBounds() const {
    return _shape->getGlobalBounds();
}

sf::Vector2f Entity::getPosition() const {
    return _shape->getPosition();
}

void Entity::move(const sf::Vector2f& offset) {
    _shape->move(offset);
}

void Entity::setPosition(const sf::Vector2f& position) {
    _shape->setPosition(position);
}

void Entity::setColor(const sf::Color& color) {
    if (_shape) {
        _shape->setFillColor(color);
    }
}

sf::Color Entity::getColor() const {
    return _shape ? _shape->getFillColor() : sf::Color::White;
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (_shape) {
        target.draw(*_shape, states);
    }
}
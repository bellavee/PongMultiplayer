
#include "InterfaceElement.h"

sf::Vector2f InterfaceElement::getPosition() const {
    return _shape->getPosition();
}

void InterfaceElement::setPosition(const sf::Vector2f& position) {
    _shape->setPosition(position);
}

void InterfaceElement::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (_shape) {
        target.draw(*_shape, states);
    }
}
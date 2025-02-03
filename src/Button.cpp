#include "Button.h"

Button::Button(const std::string& text, sf::Vector2f pos, std::function<void()> onClick) : _callback(onClick), _text(_font)
{
	_rectangleShape = new sf::RectangleShape({ 200, 50 });
	_shape = _rectangleShape;


	if (!_font.openFromFile("assets/Inter-Bold.ttf")) {
		throw std::runtime_error("Failed to load font");
	}

	_text.setFont(_font);
	_text.setString(text);
	_text.setCharacterSize(24);
	_text.setFillColor(sf::Color::White);
	_text.setPosition(sf::Vector2f(pos.x + 20, pos.y + 1));


	_shape->setFillColor(sf::Color::Blue);
	_shape->setPosition(sf::Vector2f(pos.x + 20, pos.y + 1));
}

void Button::checkClick(sf::Vector2f mousePos)
{
	if (_shape->getGlobalBounds().contains(mousePos)) {
		_callback();
	}	
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_text, states);
}


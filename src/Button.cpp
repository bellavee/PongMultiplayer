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

	_shape->setFillColor(sf::Color::Blue);
	_shape->setPosition(sf::Vector2f(pos.x + 20, pos.y + 1));
}


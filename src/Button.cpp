#include "Button.h"

#include "resources.h"

Button::Button(const std::string& text, sf::Vector2f pos, std::function<void()> onClick) : _callback(onClick), _text(_font)
{
	_rectangleShape = new sf::RectangleShape({ 200, 50 });
	_shape = _rectangleShape;

	_shape->setFillColor(sf::Color::White);
	_shape->setOrigin(_shape->getGlobalBounds().getCenter());
	_shape->setPosition(pos);


	if (!_font.openFromFile(BASE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}
	_text.setFont(_font);
	_text.setString(text);
	_text.setCharacterSize(24);
	_text.setFillColor(sf::Color::Black);
	_text.setOrigin(_text.getGlobalBounds().getCenter());
	_text.setPosition(_shape->getPosition());
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


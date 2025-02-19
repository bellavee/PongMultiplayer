#include "Button.h"

#include "resources.h"

Button::Button(const std::string& text, sf::Vector2f pos, std::function<void()> onClick) : _callback(onClick), _text(_font)
{
	_shape = new sf::RectangleShape({ 200, 50 });

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

void Button::handleEvent(const sf::Event& event)
{
	auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();

	if (!mouseEvent)
		return;

	sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));

	if (_shape->getGlobalBounds().contains(mousePos)) {
		_callback();
	}
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_text, states);
}


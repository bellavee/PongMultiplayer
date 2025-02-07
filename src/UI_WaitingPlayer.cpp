#include "UI_WaitingPlayer.h"

#include "resources.h"

UI_WaitingPlayer::UI_WaitingPlayer(float width, float height, std::function<void()> onBackToMenu)
	: _title(_font), _text(_font)
{
	_shape = new sf::RectangleShape({ width, height });
	_shape->setPosition({0,0});
	_shape->setFillColor(sf::Color(0,0,0,180));

	_popupShape = new sf::RectangleShape({ width/2, height/2 });
	_popupShape->setOrigin(_popupShape->getGlobalBounds().getCenter());
	_popupShape->setPosition({ width / 2, height / 2 });
	_popupShape->setFillColor(sf::Color::Black);
	_popupShape->setOutlineColor(sf::Color::White);
	_popupShape->setOutlineThickness(8);


	if (!_font.openFromFile(BASE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}

	_title.setFont(_font);
	_title.setString("No opponent found");
	_title.setCharacterSize(48);
	_title.setFillColor(sf::Color::White);
	_title.setOrigin(_title.getGlobalBounds().getCenter());
	_title.setPosition(sf::Vector2f(_popupShape->getGlobalBounds().getCenter().x, 3 * _popupShape->getGlobalBounds().getCenter().y / 4));

	_text.setFont(_font);
	_text.setString("Waiting for a player to connect...");
	_text.setCharacterSize(18);
	_text.setFillColor(sf::Color::White);
	_text.setOrigin(_text.getGlobalBounds().getCenter());
	_text.setPosition(sf::Vector2f(_popupShape->getGlobalBounds().getCenter().x, _popupShape->getGlobalBounds().getCenter().y));

	_buttons.emplace_back(std::make_unique<Button>("Back to Menu", sf::Vector2f(_popupShape->getGlobalBounds().getCenter().x, 5 * _popupShape->getGlobalBounds().getCenter().y / 4), onBackToMenu));
}

void UI_WaitingPlayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(*_popupShape, states);
	target.draw(_title, states);
	target.draw(_text, states);
	for (const auto& button : _buttons) {
		target.draw(*button, states);
	}
}

void UI_WaitingPlayer::handleEvent(const sf::Event& event)
{
	for (const auto& button : _buttons) {
		button->handleEvent(event);
	}
}

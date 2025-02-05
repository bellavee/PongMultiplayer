#include "UI_ServerMenu.h"
#include "resources.h"

UI_ServerMenu::UI_ServerMenu(float width, float height, std::function<void()> onJoin, std::function<void()> onQuit)
	: _title(_font)
{
	_shape = new sf::RectangleShape({ width, height });
	_shape->setPosition({ 0,0 });
	_shape->setFillColor(sf::Color::Black);

	if (!_font.openFromFile(TITLE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}

	_title.setFont(_font);
	_title.setString("Config the server port and address");
	_title.setCharacterSize(68);
	_title.setFillColor(sf::Color::White);
	_title.setOrigin(_title.getGlobalBounds().getCenter());
	_title.setPosition(sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4));

	_buttons.emplace_back(std::make_unique<Button>("Launch", sf::Vector2f(WINDOW_WIDTH / 2, 3 * WINDOW_HEIGHT / 5), onJoin));
	_buttons.emplace_back(std::make_unique<Button>("Quit", sf::Vector2f(WINDOW_WIDTH / 2, 5 * WINDOW_HEIGHT / 7), onQuit));

	_textFields.emplace_back(std::make_unique<TextInputField>(sf::Vector2f(4 * WINDOW_WIDTH / 9, WINDOW_HEIGHT / 2), sf::Vector2f(400, 30), "Server Address", AllowedCharacters::Alphanumeric));
	_textFields.emplace_back(std::make_unique<TextInputField>(sf::Vector2f(4 * WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2), sf::Vector2f(100, 30), "Port", AllowedCharacters::Numbers));

}

void UI_ServerMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_title, states);
	for (const auto& textField : _textFields) {
		target.draw(*textField, states);
	}

	for (const auto& button : _buttons) {
		target.draw(*button, states);
	}
}

void UI_ServerMenu::handleEvent(const sf::Event& event)
{
	for (const auto& textField : _textFields) {
		textField->handleEvent(event);
	}


	for (const auto& button : _buttons) {
		button->handleEvent(event);
	}
}

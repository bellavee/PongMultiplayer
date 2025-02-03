#include "UI_MainMenu.h"

#include "resources.h"

UI_MainMenu::UI_MainMenu(float width, float height, std::function<void()> onJoin, std::function<void()> onQuit)
	: onJoinGame(std::move(onJoin)), onQuitGame(std::move(onQuit)), _title(_font)
{
	if (!_font.openFromFile(BASE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}

	_title.setFont(_font);
	_title.setString("PONG Multiplayer");
	_title.setCharacterSize(68);
	_title.setFillColor(sf::Color::White);
	_title.setOrigin(_title.getGlobalBounds().getCenter());
	_title.setPosition(sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4));

	_buttons.emplace_back(std::make_unique < Button>("Join", sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), onJoinGame));
	_buttons.emplace_back(std::make_unique < Button>("Quit", sf::Vector2f(WINDOW_WIDTH / 2, 2 * WINDOW_HEIGHT / 3), onQuitGame));
}

void UI_MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_title, states);
	for (const auto& button : _buttons) {
		target.draw(*button, states);
	}
}

void UI_MainMenu::handleInput(sf::Vector2i mousePos)
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) 
		return;

	for (const auto& button : _buttons) {
		button->checkClick({ sf::Vector2f(mousePos.x, mousePos.y) });
	}
}

#include "UI_MainMenu.h"

UI_MainMenu::UI_MainMenu(float width, float height, std::function<void()> onJoin, std::function<void()> onQuit)
	: onJoinGame(std::move(onJoin)), onQuit(std::move(onQuit)), _title(_font)
{
	if (!_font.openFromFile("assets/Inter-Bold.ttf")) {
		throw std::runtime_error("Failed to load font");
	}

	_title.setFont(_font);
	_title.setString("PONG Multi");
	_title.setCharacterSize(48);
	_title.setFillColor(sf::Color::White);
	_title.setPosition(sf::Vector2f(250, 50));

	_buttons.emplace_back(std::make_unique < Button>("Join", sf::Vector2f(520, 220), onJoinGame));
	_buttons.emplace_back(std::make_unique < Button>("Quit", sf::Vector2f(520, 290), onQuit));
}

void UI_MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(_title, states);
	for (const auto& button : _buttons) {
		target.draw(*button, states);
	}
}

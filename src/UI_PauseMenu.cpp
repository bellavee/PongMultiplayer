#include "UI_PauseMenu.h"

#include "resources.h"

UI_PauseMenu::UI_PauseMenu(float width, float height, std::function<void()> onResume, std::function<void()> onBackToMenu)
	: _title(_font)
{
	_shape = new sf::RectangleShape({ width, height });
	_shape->setPosition({0,0});
	_shape->setFillColor(sf::Color(0,0,0,200));

	if (!_font.openFromFile(BASE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}

	_title.setFont(_font);
	_title.setString("Paused");
	_title.setCharacterSize(68);
	_title.setFillColor(sf::Color::White);
	_title.setOrigin(_title.getGlobalBounds().getCenter());
	_title.setPosition(sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4));

	_buttons.emplace_back(std::make_unique<Button>("Resume", sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), onResume));
	_buttons.emplace_back(std::make_unique<Button>("Back to Menu", sf::Vector2f(WINDOW_WIDTH / 2, 2 * WINDOW_HEIGHT / 3), onBackToMenu));
}

void UI_PauseMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_title, states);
	for (const auto& button : _buttons) {
		target.draw(*button, states);
	}
}

void UI_PauseMenu::handleEvent(const sf::Event& event)
{
	for (const auto& button : _buttons) {
		button->handleEvent(event);
	}
}

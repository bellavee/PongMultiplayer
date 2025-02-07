#include "UI_PlayMenu.h"

#include "resources.h"

UI_PlayMenu::UI_PlayMenu()
	: _title(_font)
{

	if (!_font.openFromFile(TITLE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}
	
	_title.setFont(_font);
	_title.setString("");
	_title.setCharacterSize(45);
	_title.setFillColor(sf::Color::White);
	_title.setOrigin(_title.getGlobalBounds().getCenter());
	_title.setPosition(sf::Vector2f(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4));
}

void UI_PlayMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_title, states);
}

void UI_PlayMenu::handleEvent(const sf::Event& event)
{
}

void UI_PlayMenu::setContent(const std::string& content)
{
	_title.setString(content);
}

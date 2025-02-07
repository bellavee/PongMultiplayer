#include "UI_ServerRunning.h"
#include "resources.h"

UI_ServerRunning::UI_ServerRunning(float width, float height)
	: _title(_font)
	, _content(_contentFont)
{
	_shape = new sf::RectangleShape({ width, height });
	_shape->setPosition({ 0,0 });
	_shape->setFillColor(sf::Color::Black);

	if (!_font.openFromFile(TITLE_FONT_PATH) || !_contentFont.openFromFile(TITLE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}

	_title.setFont(_font);
	_title.setString("Server running");
	_title.setCharacterSize(40);
	_title.setFillColor(sf::Color::White);
	_title.setOrigin(_title.getGlobalBounds().getCenter());
	_title.setPosition(sf::Vector2f(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 3));
	_content.setFont(_contentFont);
	_content.setString("");
	_content.setCharacterSize(30);
	_content.setFillColor(sf::Color::White);
	_content.setOrigin(_content.getGlobalBounds().getCenter());
	_content.setPosition(sf::Vector2f(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2));
}

void UI_ServerRunning::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_title, states);
	target.draw(_content, states);
}

void UI_ServerRunning::setContent(const std::string& content)
{
	_title.setString(content);
}

void UI_ServerRunning::addNewClient(const std::string& content)
{
	_content.setString(_content.getString() + content + "\n");
}

void UI_ServerRunning::handleEvent(const sf::Event& event)
{
}

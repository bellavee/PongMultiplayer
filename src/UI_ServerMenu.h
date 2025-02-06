#pragma once
#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

#include "Button.h"
#include "TextInputField.h"

class UI_ServerMenu : public InterfaceElement
{
public:
	UI_ServerMenu(float width, float height, std::function<void()> onLaunch, std::function<void()> onQuit);

	virtual void handleEvent(const sf::Event& event) override;

	std::string getIP() const { return _textFields[0]->getText(); }
	std::string getPort() const { return _textFields[1]->getText(); }


private:
	sf::Text _title;
	sf::Font _font;
	std::vector<std::unique_ptr<Button>> _buttons;
	std::vector<std::unique_ptr<TextInputField>> _textFields;


protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
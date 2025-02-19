#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

#include "Button.h"
#include "TextInputField.h"

class UI_MainMenu : public InterfaceElement
{
public:
	UI_MainMenu(float width, float height, std::function<void()> onJoin, std::function<void()> onQuit);
	~UI_MainMenu() {};

	virtual void handleEvent(const sf::Event& event) override;

	std::string getIP() const { return _textFields[0]->getText(); }
	std::string getPort() const { return _textFields[1]->getText(); }
	std::string getClientName() const { return _textFields[2]->getText(); }

private:
	sf::Text _title;
	sf::Font _font;
	std::vector<std::unique_ptr<Button>> _buttons;
	std::vector<std::unique_ptr<TextInputField>> _textFields;


protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


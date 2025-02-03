#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

#include "Button.h"

class UI_MainMenu : public InterfaceElement
{
public:
	UI_MainMenu(float width, float height, std::function<void()> onJoin, std::function<void()> onQuit);
	~UI_MainMenu() {};

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void handleInput(sf::Vector2i mousePos);


private:
	sf::Text _title;
	sf::Font _font;
	std::vector<std::unique_ptr<Button>> _buttons;

	std::function<void()> onJoinGame;
	std::function<void()> onQuitGame;
};


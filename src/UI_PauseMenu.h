#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

#include "Button.h"

class UI_PauseMenu : public InterfaceElement
{
public:
	UI_PauseMenu(float width, float height, std::function<void()> onJoin, std::function<void()> onQuit);
	~UI_PauseMenu() {};

	virtual void handleEvent(const sf::Event& event) override;


private:
	sf::Text _title;
	sf::Font _font;
	std::vector<std::unique_ptr<Button>> _buttons;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};


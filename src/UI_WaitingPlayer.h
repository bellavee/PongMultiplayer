#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

#include "Button.h"

class UI_WaitingPlayer : public InterfaceElement
{
public:
	UI_WaitingPlayer(float width, float height, std::function<void()> onBackToMenu);
	~UI_WaitingPlayer() {};

	virtual void handleEvent(const sf::Event& event) override;

private:
	sf::Text _title;
	sf::Text _text;
	sf::Font _font;
	std::vector<std::unique_ptr<Button>> _buttons;
	sf::RectangleShape* _popupShape;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


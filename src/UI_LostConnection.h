#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

#include "Button.h"

class UI_LostConnection : public InterfaceElement
{
public:
	UI_LostConnection(float width, float height, std::function<void()> onBackToMenu);
	~UI_LostConnection() {};


	void handleInput(sf::Vector2i mousePos);


private:
	sf::Text _title;
	sf::Text _text;
	sf::Font _font;
	std::vector<std::unique_ptr<Button>> _buttons;
	sf::RectangleShape* _popupShape;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


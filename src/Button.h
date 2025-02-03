#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>


class Button : public InterfaceElement {
public:
	Button(const std::string& text, sf::Vector2f pos, std::function<void()> onClick);
	~Button() {};

private:
	sf::Font _font;
	sf::Text _text;
	std::function<void()> _callback;

protected:
	sf::RectangleShape* _rectangleShape;

};
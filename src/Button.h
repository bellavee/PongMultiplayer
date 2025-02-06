#pragma once

#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>


class Button : public InterfaceElement {
public:
	Button(const std::string& text, sf::Vector2f pos, std::function<void()> onClick);
	~Button() {};

	virtual void handleEvent(const sf::Event& event) override;

	std::function<void()> _callback;

private:
	sf::Font _font;
	sf::Text _text;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
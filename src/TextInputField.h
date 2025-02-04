#pragma once

#include "InterfaceElement.h"
#include <SFML/Graphics.hpp>
#include <string>
#include "SFML/Window/Event.hpp"

class TextInputField : public InterfaceElement {
public:
	TextInputField(sf::Vector2f pos, sf::Vector2f size);
	~TextInputField() {};

	std::string getText() const { return _userText.getString(); }
	
	virtual void handleEvent(const sf::Event& event) override;

	

private:
	sf::RectangleShape _shape;
	sf::Font _font;
	sf::Text _text;
	sf::Text _userText;
	bool _isActive;

	sf::Vector2f _position;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

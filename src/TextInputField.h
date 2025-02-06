#pragma once

#include "InterfaceElement.h"
#include <SFML/Graphics.hpp>
#include <string>
#include "SFML/Window/Event.hpp"

enum class AllowedCharacters {
	All,
	Numbers,
	Letters,
	Alphanumeric
};

class TextInputField : public InterfaceElement {
public:
	TextInputField(sf::Vector2f pos, sf::Vector2f size, const std::string& displayText = std::string("SampleText"), AllowedCharacters allowedchars = AllowedCharacters::All, int maxChars = 15);

	~TextInputField() {};

	std::string getText() const { return _userText.getString(); }
	
	virtual void handleEvent(const sf::Event& event) override;

	void setActive(bool active) { _isActive = active; }
	bool isActive() const { return _isActive; }

private:
	bool isCharacterAllowed(char32_t unicode);
	bool _isActive;

	sf::RectangleShape _shape;
	sf::Font _font;
	sf::Text _text;
	sf::Text _userText;

	sf::Vector2f _position;
	AllowedCharacters _allowedChars;
	int _maxChars;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

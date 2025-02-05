#pragma once
#include "InterfaceElement.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

#include "Button.h"
#include "TextInputField.h"

class UI_ServerRunning : public InterfaceElement
{
public:
	UI_ServerRunning(float width, float height);

	void setContent(const std::string& content);
	virtual void handleEvent(const sf::Event& event) override;

private:
	sf::Text _title;
	sf::Font _font;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

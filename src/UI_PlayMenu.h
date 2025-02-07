#pragma once

#include "InterfaceElement.h"
#include <SFML/Graphics.hpp>
#include <vector>

class UI_PlayMenu : public InterfaceElement
{
public:
	UI_PlayMenu();
	~UI_PlayMenu() {};

	virtual void handleEvent(const sf::Event& event) override;
	void setContent(const std::string& content);

private:
	sf::Text _title;
	sf::Font _font;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};

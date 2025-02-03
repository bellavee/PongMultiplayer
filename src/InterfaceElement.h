#pragma once

#include <SFML/Graphics.hpp>

class InterfaceElement : public sf::Drawable {
public:
    InterfaceElement() = default;
    virtual ~InterfaceElement() = default;

    sf::Vector2f getPosition() const;

    void setPosition(const sf::Vector2f& position);


protected:
    sf::Shape* _shape = nullptr;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
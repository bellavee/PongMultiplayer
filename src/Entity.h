#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class Entity : public sf::Drawable {
public:
    Entity() = default;
    virtual ~Entity() = default;

    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getPosition() const;

    virtual void move(const sf::Vector2f& offset);
    void setPosition(const sf::Vector2f& position);

    void setColor(const sf::Color& color);
    sf::Color getColor() const;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

protected:
    sf::Shape* _shape = nullptr;

};


#endif //ENTITY_H

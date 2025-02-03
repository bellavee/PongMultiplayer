
#ifndef BALL_H
#define BALL_H

#include "Entity.h"
#include <SFML/Graphics.hpp>

class Ball : public Entity {
public:
    Ball(float x, float y);
    ~Ball() override;

    void update(float deltaTime);
    void reset(float x, float y);
    void reverseXVelocity();
    void reverseYVelocity();
    float getRadius() const;

private:
    sf::CircleShape* _circleShape;  // Concrete shape type
    sf::Vector2f _velocity;
    const float SIZE = 20.0f;
    const float SPEED = 400.0f;
};



#endif //BALL_H

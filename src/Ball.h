
#ifndef BALL_H
#define BALL_H

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include "resources.h"

class Ball : public Entity {
public:
    Ball(float x, float y);
    ~Ball() override;

    void update(float deltaTime);
    void reset(float x, float y);
    void reverseXVelocity();
    void reverseYVelocity();
    float getRadius() const;
    void setVelocityY(float vy) { _velocity.y = vy; }

private:
    sf::CircleShape* _circleShape;
    sf::Vector2f _velocity;
};



#endif //BALL_H

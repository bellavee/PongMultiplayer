
#ifndef PADDLE_H
#define PADDLE_H

#include "Entity.h"
#include <SFML/Graphics.hpp>

class Paddle : public Entity {
public:
    Paddle(float x, float y);
    ~Paddle() override;

    void move(float offset);  // Specialized move for Paddle
    void keepInBounds(float minY, float maxY);
    float getHeight() const;

private:
    sf::RectangleShape* _rectangleShape;  // Concrete shape type
    const float WIDTH = 30.0f;
    const float HEIGHT = 150.0f;
};


#endif //PADDLE_H

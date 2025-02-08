
#ifndef PADDLE_H
#define PADDLE_H

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include "resources.h"

class Paddle : public Entity {
public:
    Paddle(float x, float y);
    ~Paddle() override;

    void move(float offset);
    void keepInBounds(float minY, float maxY);
    float getHeight() const;
    void reset(float x, float y);

private:
    sf::RectangleShape* _rectangleShape;
};


#endif //PADDLE_H

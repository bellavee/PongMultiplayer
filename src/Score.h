
#ifndef SCORE_H
#define SCORE_H

#include <SFML/Graphics.hpp>

class Score : public sf::Drawable {
public:
    Score(float x, float y);

    void increment();
    bool loadFont(const std::string &fontPath);
    void reset();
    void update(int value);
    void setPlayerName(const std::string& name) { _playerName = name; update(_value); }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Text _text;
    sf::Font _font;
    int _value;
    std::string _playerName;
};



#endif //SCORE_H

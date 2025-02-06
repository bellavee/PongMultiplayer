
#include "Score.h"

Score::Score(float x, float y) : _text(_font), _value(0), _playerName("") {
    _text.setCharacterSize(45);
    _text.setFillColor(sf::Color::White);
    _text.setPosition({x, y});
    _text.setString(_playerName + ": " + "0");
}

void Score::increment() {
    _value++;
    _text.setString(std::to_string(_value));
}

void Score::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_text, states);
}

bool Score::loadFont(const std::string& fontPath) {
    if (!_font.openFromFile(fontPath)) {
        return false;
    }
    _text.setFont(_font);
    return true;
}

void Score::reset()
{
    _value = 0;
	_text.setString(_playerName + ": " + std::to_string(_value));
}

void Score::update(int value) {
    _value = value;
    _text.setString(_playerName + ": " + std::to_string(_value));
}

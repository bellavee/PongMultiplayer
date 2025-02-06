#include "TextInputField.h"
#include "resources.h"

#include <SFML/Window/Export.hpp>

TextInputField::TextInputField(sf::Vector2f pos, sf::Vector2f size, const std::string& displayText, AllowedCharacters allowedchars, int maxChars) 
	: _text(_font), _userText(_font), _position(pos), _isActive(false), _allowedChars(allowedchars), _maxChars(maxChars)
{
	_shape.setSize(size);
	_shape.setFillColor(sf::Color::Black);
	_shape.setOutlineColor(sf::Color::White);
	_shape.setOutlineThickness(2);
	_shape.setOrigin(_shape.getLocalBounds().getCenter());
	_shape.setPosition(pos);

	if (!_font.openFromFile(BASE_FONT_PATH)) {
		throw std::runtime_error("Failed to load font");
	}

	_text.setFont(_font);
	_text.setCharacterSize(18);
	_text.setFillColor(sf::Color(255, 255, 255, 150));
	_text.setString(displayText);

	sf::FloatRect textBounds = _text.getLocalBounds();
	_text.setOrigin({ textBounds.position + textBounds.size / 2.f });
	_text.setPosition(_shape.getPosition());


	_userText.setFont(_font);
	_userText.setCharacterSize(18);
	_userText.setFillColor(sf::Color::White);
	_userText.setString("qdA!");
	sf::FloatRect userTextBounds = _userText.getLocalBounds();
	_userText.setString("");
	_userText.setOrigin({ userTextBounds.position + userTextBounds.size / 2.f });
	_userText.setPosition(_shape.getPosition());
}

void TextInputField::handleEvent(const sf::Event& event)
{
	if (event.is<sf::Event::MouseButtonPressed>())
	{
		auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();

		if (mouseEvent->button == sf::Mouse::Button::Left)
		{
			sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));
			_isActive = _shape.getGlobalBounds().contains(mousePos);			

		}
		return;
	}

	if (!_isActive)
		return;

	
	if (const auto* textEvent = event.getIf<sf::Event::TextEntered>())
	{
		if (textEvent->unicode == 8 && !_userText.getString().isEmpty()) // Backspace
		{
			sf::String str = _userText.getString();
			str.erase(str.getSize() - 1);
			_userText.setString(str);
		}
		else if (textEvent->unicode >= 32 && textEvent->unicode <= 126)
		{
			// Limite le nombre de caractères à la taille de la forme
			if (_userText.getLocalBounds().size.x >= _shape.getSize().x - 20)
				return;

			if (_userText.getString().getSize() >= _maxChars)
				return;

			if (!isCharacterAllowed(textEvent->unicode))
				return;

			_userText.setString(_userText.getString() + static_cast<char>(textEvent->unicode));
		}
	}	
	else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		// Utilisation de Ctrl + V pour coller une chaîne de caractères nettoyée
		if (keyPressed->control && keyPressed->code == sf::Keyboard::Key::V)
		{
			sf::String clipboardText = sf::Clipboard::getString();
			sf::String filteredText = _userText.getString();

			for (auto unicode : clipboardText)
			{
				if (isCharacterAllowed(unicode))
				{
					if (filteredText.getSize() >= _maxChars)
						break;
					filteredText += static_cast<char>(unicode);
				}
			}
			_userText.setString(filteredText);
		}
	}

	sf::FloatRect userTextBounds = _userText.getLocalBounds();
	_userText.setOrigin({ userTextBounds.position + userTextBounds.size / 2.f });
	_userText.setPosition(_shape.getPosition());
}


bool TextInputField::isCharacterAllowed(char32_t unicode)
{
	switch (_allowedChars)
	{
	case AllowedCharacters::Numbers:
		return (unicode >= 48 && unicode <= 57) || unicode == 46;
	case AllowedCharacters::Letters:
		return (unicode >= 65 && unicode <= 90) || (unicode >= 97 && unicode <= 122);
	case AllowedCharacters::Alphanumeric:
		return (unicode >= 48 && unicode <= 57) || (unicode >= 65 && unicode <= 90) || (unicode >= 97 && unicode <= 122) || unicode == 46;
	default:
		return false;
	}
}


void TextInputField::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	InterfaceElement::draw(target, states);
	target.draw(_shape, states);
	if (_userText.getString().isEmpty() && !_isActive)
		target.draw(_text, states);
	else
		target.draw(_userText, states);
}


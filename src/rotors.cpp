#include "rotors.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Rotor::Rotor()
{

}

Rotor::Rotor(std::array<int, 26> conversions, int turnoverPosition,
	sf::RectangleShape rotorWindow, sf::Texture* rotorWindowTexture,
	sf::RectangleShape rotorWheel, sf::Texture* rotorWheelTexture, int sizeScale, sf::Text rotorNumber)
{
	this->conversions = conversions;
	this->turnoverPosition = turnoverPosition;
	this->rotorWindow = rotorWindow;
	this->rotorWheel = rotorWheel;
	this->rotorNumber = rotorNumber;

	rotation = 1;

	rotorWindowAnimation = Animation(rotorWindowTexture, sf::Vector2i(10, 26), 0.0125f);
	rotorWheelAnimation  = Animation(rotorWheelTexture, sf::Vector2i(10, 1), 0.0125f);
	rotorWheelBeingDragged = false;
	currentDragCoordinates = sf::Vector2u(0, 0);
}

int Rotor::inConvert(int in)
{
	int index = in + rotation - 1;
	if (index > 25)
	{
		index -= 26;
	}

	return conversions[index];
}

int Rotor::outConvert(int out)
{
	int index = -1;

	for (int i = 0; i < 26; i++)
	{
		if (conversions[i] == out)
		{
			index = i - rotation + 1;
		}
	}

	if (index < 0)
	{
		index += 26;
	}

	return index;
}

void Rotor::rotateUp()
{
	rotorWindowAnimation.startAnimation(rotation - 1);
	rotorWheelAnimation.startAnimation(0);

	rotation++;

	if (rotation > 26)
	{
		rotation -= 26;
	}
}

void Rotor::updateAnimation(float deltaTime)
{
	rotorWindowAnimation.update(deltaTime);
	rotorWheelAnimation.update(deltaTime);
	rotorWindow.setTextureRect(rotorWindowAnimation.uvRect);
	rotorWheel.setTextureRect(rotorWheelAnimation.uvRect);
}

bool Rotor::rotorWheelClicked(int x, int y)
{
	return rotorWheel.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
}

Reflector::Reflector()
{

}

Reflector::Reflector(int* conversions)
{
	this->conversions = conversions;
}

int Reflector::inConvert(int in)
{
	return conversions[in];
}

Plugboard::Plugboard()
{
	conversions.reserve(13);
}

void Plugboard::insertPair(PlugboardButton* button1, PlugboardButton* button2)
{
	std::pair<PlugboardButton*, PlugboardButton*> pair(button1, button2);
	conversions.push_back(pair);
}

int Plugboard::convert(int in)
{
	for (std::pair<PlugboardButton*, PlugboardButton*> pair : conversions)
	{
		if (pair.first->charVal == in && pair.second->charVal >= 0 && pair.second->charVal < 26)
		{
			return pair.second->charVal;
		}

		if (pair.second->charVal == in && in && pair.first->charVal >= 0 && pair.first->charVal < 26)
		{
			return pair.first->charVal;
		}
	}

	return in;
}



Button::Button()
{

}

Button::Button(sf::RectangleShape shape)
{
	this->shape = shape;
}

bool Button::isClicked(int x, int y)
{
	return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
}

PlugboardButton::PlugboardButton(sf::RectangleShape shape, sf::RectangleShape letter, int charVal)
{
	this->shape = shape;
	this->letter = letter;
	this->charVal = charVal;
}

bool PlugboardButton::isClicked(int x, int y)
{
	return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
}

void PlugboardButton::setLetter(int charVal)
{
	letter.setTextureRect(sf::IntRect(letter.getTexture()->getSize().x / 27 * (charVal + 1), 0, 10, 10));
	this->charVal = charVal;
}



int getConversion(int in, Plugboard plugboard, Rotor rotor1, Rotor rotor2, Rotor rotor3, Reflector reflector)
{
	return plugboard.convert(
				rotor3.outConvert(
					rotor2.outConvert(
						rotor1.outConvert(
							reflector.inConvert(
								rotor1.inConvert(
									rotor2.inConvert(
										rotor3.inConvert(
											plugboard.convert(in)))))))));
}

int getCharIndex(char key)
{
	return key - 65;
}

char getKeyFromIndex(int index)
{
	return index + 65;
}

char getKeyFromEvent(sf::Event* event)
{
	char pressedKey = ' ';

	switch (event->key.code)
	{
	case sf::Keyboard::A:
		pressedKey = 'A';
		break;

	case sf::Keyboard::B:
		pressedKey = 'B';
		break;

	case sf::Keyboard::C:
		pressedKey = 'C';
		break;

	case sf::Keyboard::D:
		pressedKey = 'D';
		break;

	case sf::Keyboard::E:
		pressedKey = 'E';
		break;

	case sf::Keyboard::F:
		pressedKey = 'F';
		break;

	case sf::Keyboard::G:
		pressedKey = 'G';
		break;

	case sf::Keyboard::H:
		pressedKey = 'H';
		break;

	case sf::Keyboard::I:
		pressedKey = 'I';
		break;

	case sf::Keyboard::J:
		pressedKey = 'J';
		break;

	case sf::Keyboard::K:
		pressedKey = 'K';
		break;

	case sf::Keyboard::L:
		pressedKey = 'L';
		break;

	case sf::Keyboard::M:
		pressedKey = 'M';
		break;

	case sf::Keyboard::N:
		pressedKey = 'N';
		break;

	case sf::Keyboard::O:
		pressedKey = 'O';
		break;

	case sf::Keyboard::P:
		pressedKey = 'P';
		break;

	case sf::Keyboard::Q:
		pressedKey = 'Q';
		break;

	case sf::Keyboard::R:
		pressedKey = 'R';
		break;

	case sf::Keyboard::S:
		pressedKey = 'S';
		break;

	case sf::Keyboard::T:
		pressedKey = 'T';
		break;

	case sf::Keyboard::U:
		pressedKey = 'U';
		break;

	case sf::Keyboard::V:
		pressedKey = 'V';
		break;

	case sf::Keyboard::W:
		pressedKey = 'W';
		break;

	case sf::Keyboard::X:
		pressedKey = 'X';
		break;

	case sf::Keyboard::Y:
		pressedKey = 'Y';
		break;

	case sf::Keyboard::Z:
		pressedKey = 'Z';
		break;

	default:
		pressedKey = ' ';
		break;
	}

	return pressedKey;
}
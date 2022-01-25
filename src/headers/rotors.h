#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include "animation.h"

class Rotor;
class Reflector;
class Plugboard;
class Button;
class PlugboardButton;

class Rotor
{
public:
	Rotor();
	Rotor(std::array<int, 26> conversions, int turnoverPosition,
		sf::RectangleShape rotorWindow, sf::Texture* rotorWindowTexture,
		  sf::RectangleShape rotorWheel, sf::Texture* rotorWheelTexture, int sizeScale, sf::Text rotorNumber);
	int inConvert(int in);
	int outConvert(int out);
	void rotateUp();
	void updateAnimation(float deltaTime);
	bool rotorWheelClicked(int x, int y);

public:
	int rotation;
	int turnoverPosition;
	sf::RectangleShape rotorWindow;
	sf::RectangleShape rotorWheel;
	Animation rotorWindowAnimation;
	Animation rotorWheelAnimation;
	bool rotorWheelBeingDragged;
	sf::Vector2u currentDragCoordinates;
	sf::Text rotorNumber;

private:
	std::array<int, 26> conversions;
};

class Reflector
{
public:
	Reflector();
	Reflector(int* conversions);
	int inConvert(int in);

private:
	int* conversions;
};

class Plugboard
{
public:
	Plugboard();
	void insertPair(PlugboardButton* button1, PlugboardButton* button2);
	int convert(int in);

private:
	std::vector<std::pair<PlugboardButton*, PlugboardButton*>> conversions;
};



class Button
{
public:
	Button();
	Button(sf::RectangleShape shape);
	bool isClicked(int x, int y);

public:
	sf::RectangleShape shape;

private:
};

class PlugboardButton
{
public:
	PlugboardButton(sf::RectangleShape shape, sf::RectangleShape letter, int charVal);
	bool isClicked(int x, int y);
	void setLetter(int charVal);

public:
	sf::RectangleShape shape;
	sf::RectangleShape letter;
	int charVal;

private:
};


int getConversion(int in, Plugboard plugboard, Rotor rotor1, Rotor rotor2, Rotor rotor3, Reflector reflector);
int getCharIndex(char key);
char getKeyFromIndex(int index);
char getKeyFromEvent(sf::Event* event);
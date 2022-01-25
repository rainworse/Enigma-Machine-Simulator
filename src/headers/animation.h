#pragma once
#include <SFML\Graphics.hpp>

class Animation
{
public:
	Animation();
	Animation(sf::Texture* texture, sf::Vector2i imageCount, float switchTime);

	void startAnimation(int row);
	void finishAnimation(int inc);
	void update(float deltaTime);
	int incrementAnimation(int inc);

public:
	sf::IntRect uvRect;	// Coordinates of rotor image in tux
	int currentRow;

private:
	sf::Vector2i imageCount;
	sf::Vector2i currentImage;

	float totalTime;
	float switchTime;
	int runCycles;
	int increment;	// Which direction to increment animation in
};
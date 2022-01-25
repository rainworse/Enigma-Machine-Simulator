#include "animation.h"

Animation::Animation(sf::Texture* texture, sf::Vector2i imageCount, float switchTime)
{
	this->imageCount = imageCount;
	this->switchTime = switchTime;
	totalTime = 0.0f;
	currentImage.x = 0;
	runCycles = 0;
	currentRow = 0;

	uvRect.width  = texture->getSize().x / imageCount.x;
	uvRect.height = texture->getSize().y / imageCount.y;
}

Animation::Animation()
{

}

void Animation::startAnimation(int row)
{
	runCycles++;
	currentRow = row;
	currentImage.x = 0;
	increment = 1;
}

void Animation::finishAnimation(int inc)
{
	runCycles = 1;
	increment = inc;
}

void Animation::update(float deltaTime)
{
	if (runCycles > 0)
	{
		currentImage.y = currentRow;
		totalTime += deltaTime;

		if (totalTime >= switchTime)
		{
			totalTime -= switchTime;
			currentImage.x += increment;

			if (currentImage.x >= imageCount.x - 1)
			{
				currentImage.x = imageCount.x - 1;
				totalTime = 0.0f;
				runCycles--;
			}

			if (currentImage.x <= 0)
			{
				currentImage.x = 0;
				totalTime = 0.0f;
				runCycles--;
			}
		}

		uvRect.left = currentImage.x * uvRect.width;
		uvRect.top = currentImage.y * uvRect.height;
	}
}

int Animation::incrementAnimation(int inc)
{
	if (inc > 0)
		currentImage.x++;
	else if (inc < 0)
		currentImage.x--;

	currentImage.y = currentRow;

	if (currentImage.x >= imageCount.x - 1)
	{
		currentImage.x = 0;
		currentImage.y++;
		currentRow++;

		if (currentImage.y >= imageCount.y)
		{
			currentImage.y = 0;
			currentRow = 0;
		}
	}

	else if (currentImage.x < 0)
	{
		currentImage.x = imageCount.x - 1;
		currentImage.y--;
		currentRow--;

		if (currentImage.y < 0)
		{
			currentImage.y = imageCount.y - 1;
			currentRow = imageCount.y - 1;
		}
	}

	uvRect.left = currentImage.x * uvRect.width;
	uvRect.top = currentImage.y * uvRect.height;

	// Return the current rotor setting the animation is closest to
	if (currentImage.x > 5)
	{
		if (currentRow + 1 >= imageCount.y)
		{
			return 1;
		}
		return currentRow + 2;
	}

	return currentRow + 1;
}
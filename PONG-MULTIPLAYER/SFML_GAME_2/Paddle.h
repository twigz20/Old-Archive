#pragma once
#include "Entity.h"

class Paddle :
	public Entity
{
public:
	Paddle();
	~Paddle();

	void Update(float elapsedTime);
	void Draw(sf::RenderWindow& window);

	void moveDown(float elapsedTime);
	void moveUp(float elapsedTime);

private:
	float speed;
};


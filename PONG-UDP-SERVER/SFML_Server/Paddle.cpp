#include "Game.h"
#include "Paddle.h"


Paddle::Paddle() : speed(400.0f)
{
}


Paddle::~Paddle()
{
}

void Paddle::Draw(sf::RenderWindow& window)
{
	Entity::Draw(window);
}

void Paddle::Update(float elapsedTime)
{
	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		moveDown(elapsedTime);

		if (GetPositionY() + GetHeight() >= SCREEN_HEIGHT)
			moveUp(elapsedTime);
	} 
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		moveUp(elapsedTime);

		if (Entity::GetPositionY() <= 0)
			moveDown(elapsedTime);
	}*/
}

void Paddle::moveDown(float elapsedTime)
{
	float posY = sprite.getPosition().y;
	posY += 1 * speed * elapsedTime;

	sprite.setPosition(sprite.getPosition().x, posY);
}

void Paddle::moveUp(float elapsedTime)
{
	float posY = sprite.getPosition().y;
	posY += -1 * speed * elapsedTime;

	sprite.setPosition(sprite.getPosition().x, posY);
}

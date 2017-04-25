#include "Ball.h"


Ball::Ball() : velocity(1,1), speed(500), speedModifier(1.025)
{
}


Ball::~Ball()
{
}

void Ball::Draw(sf::RenderWindow& window)
{
	Entity::Draw(window);
}

void Ball::Update(float elapsedTime)
{
	//Updates Ball Position

	float posX = sprite.getPosition().x;
	posX += velocity.x * elapsedTime;

	float posY = sprite.getPosition().y;
	posY += velocity.y * elapsedTime;

	sprite.setPosition(posX, posY);
}

void Ball::setBallDirection(float mVelX, float mVelY)
{
	//Normalizing Ball Vector
	float length = sqrt((mVelX * mVelX) + (mVelY * mVelY));
	velocity.x = speed * (mVelX / length);
	velocity.y = speed * (mVelY / length);

}

void Ball::changeBallSpeed(float newBallSpeed)
{
	if (newBallSpeed == 0 || newBallSpeed >= 300)
		speed = newBallSpeed;
}

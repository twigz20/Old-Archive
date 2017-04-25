#pragma once
#include "Entity.h"
class Ball :
	public Entity
{
public:
	Ball();
	~Ball();

	void Update(float elapsedTime);
	void Draw(sf::RenderWindow& window);

	void setBallDirection(float mVelX, float mVelY);
	float getBallSpeed() { return speed; }
	void changeBallSpeed(float newBallSpeed);
	float getSpeedMod() { return speedModifier; };

	void SetVelocity(sf::Vector2f& vel) { velocity = vel; }
	sf::Vector2f& GetVelocity() { return velocity; }
private:
	float speed;
	float speedModifier;

	sf::Vector2f velocity;
};


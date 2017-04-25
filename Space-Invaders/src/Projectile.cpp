#include "Projectile.h"

Projectile::Projectile()
{
    posX = 20;
	posY = 20;

	width = 1;
	height = 1;

	mVelX = 0.0f;
	mVelY = 0.0f;

	speed = 0.0f;
	damage = 0;

	dead = false;
}

Projectile::~Projectile()
{
    //dtor
}

void Projectile::update(const float deltaTime)
{

}

void Projectile::render()
{

}

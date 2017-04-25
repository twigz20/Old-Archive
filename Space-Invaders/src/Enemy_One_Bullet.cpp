#include "Enemy_One_Bullet.h"
#include "Operations.h"
#include "picojson.h"
#include <sstream>

Enemy_One_Bullet::Enemy_One_Bullet()
{

	dead = false;

	std::stringstream ss;
	ss << "enemy-bullet";

	std::string fileStr = readFile("resources/files/bullet.json");
	picojson::value v;
	std::string err;
	picojson::parse(v, fileStr.begin(), fileStr.end(), &err);

	picojson::object& obj = v.get<picojson::object>();
	auto& plane = obj[ss.str()].get<picojson::object>();

	damage = plane["damage"].get<double>();
	speed = plane["speed"].get<double>();
	width = plane["width"].get<double>();
	height = plane["height"].get<double>();
	mVelX = plane["velX"].get<double>();
	mVelY = plane["velY"].get<double>();

	bullet = rm.getTexture("resources/images/Invaders2.png");
    bullet->setColor(0,191,255);

}

Enemy_One_Bullet::~Enemy_One_Bullet()
{
    bullet = nullptr;
}

void Enemy_One_Bullet::handleEvents(SDL_Event* event)
{

}

void Enemy_One_Bullet::update(const float deltaTime)
{
    posY += mVelY * speed * deltaTime;
}

void Enemy_One_Bullet::render()
{
    SDL_Rect src_rect;
    src_rect.x = 48;
	src_rect.y = 0;
	src_rect.w = width;
	src_rect.h = height;

    SDL_Rect dest_rect;
    dest_rect.x = posX;
	dest_rect.y = posY;
	dest_rect.w = width;
	dest_rect.h = height;

    if(!dead)
        bullet->render(&src_rect,&dest_rect);
}


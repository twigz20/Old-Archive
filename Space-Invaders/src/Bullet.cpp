#include "Bullet.h"
#include "Operations.h"
#include "picojson.h"
#include <sstream>

std::string bulletType(BulletType type)
{
    std::string typ;
	switch (type) {
		case PLAYER:
			typ = "player-bullet";
			break;
		case ENEMY:
			typ = "enemy-bullet";
			break;
		default:
			break;
	}
	return typ;
}

BulletType bulletType(std::string type)
{
	BulletType typ;
	if(type == "player-bullet")
		typ = PLAYER;
	else if(type == "enemy-bullet")
		typ = ENEMY;
	return typ;
};

Bullet::Bullet(BulletType type) :
    type_(type)
{
    dead = false;

	std::stringstream ss;
	ss << bulletType(type_);

	std::string fileStr = readFile("resources/files/bullet.json");
	picojson::value v;
	std::string err;
	picojson::parse(v, fileStr.begin(), fileStr.end(), &err);

	picojson::object& obj = v.get<picojson::object>();
	auto& plane = obj[ss.str()].get<picojson::object>();

    image_ = plane["image"].get<std::string>();
    name_ = plane["name"].get<std::string>();
	damage = plane["damage"].get<double>();
	speed = plane["speed"].get<double>();
	width = plane["width"].get<double>();
	height = plane["height"].get<double>();
	mVelX = plane["velX"].get<double>();
	mVelY = plane["velY"].get<double>();

	bullet = rm.getTexture(image_);
	bullet->width_ = width;
	bullet->height_ = height;

	if(ss.str() == "enemy-bullet")
	    bullet->setColor(124,252,0);
    else
        bullet->setColor(255,140,0);
}

Bullet::~Bullet()
{
    bullet = nullptr;
}

void Bullet::handleEvents(SDL_Event* event)
{
}

void Bullet::update(const float deltaTime)
{
    posY += mVelY * speed * deltaTime;
}

void Bullet::render()
{
    SDL_Rect dest_rect;
    dest_rect.x = posX;
	dest_rect.y = posY;
	dest_rect.w = width;
	dest_rect.h = height;

    if(!dead)
       bullet->render(NULL,&dest_rect); // SDL_RenderCopy(game->getRenderer(), bullet, NULL, &dest_rect);
}

std::string Bullet::name() const
{
	return name_;
}

#include "CreepInfo.h"
#include "Operations.h"

#include "picojson.h"

#include <sstream>
#include <fstream>

std::string creepColor(CreepColor color)
{
	std::string cor;
	switch (color) {
		case BLUE:
			cor = "blue";
			break;
		case PURPLE:
			cor = "purple";
			break;
		case RED:
			cor = "red";
			break;
		default:
			break;
	}
	return cor;
};

std::string creepLevel(CreepLevel level)
{
	std::string lev;
	switch (level) {
		case ONE:
			lev = "one";
			break;
		case TWO:
			lev = "two";
			break;
		case THREE:
			lev = "three";
			break;
		default:
			break;
	}
	return lev;
};

CreepColor creepColor(std::string color)
{
	CreepColor cor = BLUE;
	if(color == "purple")
		cor = PURPLE;
	else if(color == "red")
		cor = RED;
	return cor;
};

CreepLevel creepLevel(std::string level)
{
	CreepLevel lev = ONE;
	if(level == "two")
		lev = TWO;
	else if(level == "three")
		lev = THREE;
	return lev;
};

CreepInfo::CreepInfo(CreepColor color, CreepLevel level) :
		color_(color),
		level_(level)

{
    shotUpperBound = 170;
	width = 48;
	height = 45;

	mVelX = 0.0f;
	mVelY = 0.0f;

	dead = false;

	setClips(color);

    sheet = rm.getTexture("resources/images/Invaders2.png");

    frame = 0;
    explosionFrame = 0;
    explosionEnd = false;

	std::stringstream ss;
	ss << "creep-" << level_ << "-" << creepColor(color);

	std::string fileStr = readFile("resources/files/creeps.json");
	picojson::value v;
	std::string err;
	picojson::parse(v, fileStr.begin(), fileStr.end(), &err);

	picojson::object& obj = v.get<picojson::object>();
	auto& creep = obj[ss.str()].get<picojson::object>();

	health_ = creep["health"].get<double>();
	speed_ = creep["speed"].get<double>();
	gold_ = creep["gold"].get<double>();
}

CreepInfo::~CreepInfo()
{
    sheet = nullptr;
}

void CreepInfo::setClips(CreepColor color)
{
    int x = 0;
    int y = 0;

    if(color == BLUE) {
        x = 0;
        if(level_ == ONE)
            y = 0;
        else if(level_ == TWO)
            y = 49;
        else
            y = 96;
    } else if(color == PURPLE) {
        x = 0;
        if(level_ == ONE)
            y = 15;
        else if(level_ == TWO)
            y = 65;
        else
            y = 112;
    } else if(color == RED) {
        x = 0;
        if(level_ == ONE)
            y = 32;
        else if(level_ == TWO)
            y = 80;
        else
        {
            x = 63;
            y = 160;
        }
    }

    //Set sprite clips
    enemy[ 0 ].x = x;
    enemy[ 0 ].y = y;
    enemy[ 0 ].w = width/3;
    enemy[ 0 ].h = height/3;

    enemy[ 1 ].x = x+16;
    enemy[ 1 ].y = y;
    enemy[ 1 ].w = width/3;
    enemy[ 1 ].h = height/3;

    enemy[ 2 ].x = x+32;
    enemy[ 2 ].y = y;
    enemy[ 2 ].w = width/3;
    enemy[ 2 ].h = height/3;

    enemyExplosion[ 0 ].x = 0;
    enemyExplosion[ 0 ].y = 496;
    enemyExplosion[ 0 ].w = width/3-1;
    enemyExplosion[ 0 ].h = height/3;

    enemyExplosion[ 1 ].x = 16;
    enemyExplosion[ 1 ].y = 496;
    enemyExplosion[ 1 ].w = width/3-1;
    enemyExplosion[ 1 ].h = height/3;

    enemyExplosion[ 2 ].x = 32;
    enemyExplosion[ 2 ].y = 496;
    enemyExplosion[ 2 ].w = width/3-1;
    enemyExplosion[ 2 ].h = height/3;

    enemyExplosion[ 3 ].x = 49;
    enemyExplosion[ 3 ].y = 494;
    enemyExplosion[ 3 ].w = width/3-1;
    enemyExplosion[ 3 ].h = height/3;

    enemyExplosion[ 4 ].x = 65;
    enemyExplosion[ 4 ].y = 494;
    enemyExplosion[ 4 ].w = width/3-1;
    enemyExplosion[ 4 ].h = height/3;

    enemyExplosion[ 5 ].x = 81;
    enemyExplosion[ 5 ].y = 494;
    enemyExplosion[ 5 ].w = width/3-1;
    enemyExplosion[ 5 ].h = height/3;
}

void CreepInfo::handleEvents(SDL_Event* event)
{
}

void CreepInfo::update(float deltaTime)
{
    if(!dead)
    {
        animate();
    }
    else
        explosion();
}

void CreepInfo::animate()
{
    // patternTwo(deltaTime);
    ++frame;

    //Cycle animation
    if( frame / 12 >= ENEMY_FRAMES )
    {
        frame = 0;
    }
}

void CreepInfo::explosion()
{
    // patternTwo(deltaTime);
    ++explosionFrame;

    //Cycle animation
    if( explosionFrame / 8 >= ENEMY_EXPLOSION_FRAMES )
    {
        explosionFrame = 0;
        explosionEnd = true;
    }
}

void CreepInfo::moveLeft(float deltaTime)
{
    posX += -1 * speed_ * deltaTime;
    posY += mVelY * speed_ * deltaTime;
}

void CreepInfo::moveRight(float deltaTime)
{
    posX += 1 * speed_ * deltaTime;
    posY += mVelY * speed_ * deltaTime;
}

void CreepInfo::moveDown(float deltaTime)
{
    posX += mVelX * speed_ * deltaTime;
    posY += 15 * speed_ * deltaTime;
}

void CreepInfo::render()
{
    SDL_Rect* currentClip = nullptr;
    SDL_Rect dest_rect;

    if(!dead)
    {
        //Render current frame
        currentClip = &enemy[ frame / 12 ];

        dest_rect.x = posX;
        dest_rect.y = posY;
        dest_rect.w = width;
        dest_rect.h = height;

        sheet->render(currentClip,&dest_rect);

    } else if( dead && !explosionEnd )
    {
        //Render current frame
        currentClip = &enemyExplosion[ explosionFrame / 8 ];

        dest_rect.x = posX;
        dest_rect.y = posY;
        dest_rect.w = width;
        dest_rect.h = height;

        sheet->render(currentClip,&dest_rect);
    }
}

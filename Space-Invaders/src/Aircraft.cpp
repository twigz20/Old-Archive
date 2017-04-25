#include "Aircraft.h"
#include "Operations.h"
#include "picojson.h"
#include <sstream>

Aircraft::Aircraft()
{
    setClips();
    explosionFrame = 0;
    explosionEnd = false;
	dead = false;

	std::stringstream ss;
	ss << "aircraft-raptor";

	std::string fileStr = readFile("resources/files/aircraft.json");
	picojson::value v;
	std::string err;
	picojson::parse(v, fileStr.begin(), fileStr.end(), &err);

	picojson::object& obj = v.get<picojson::object>();
	auto& plane = obj[ss.str()].get<picojson::object>();

	speed = plane["speed"].get<double>();
	posX = plane["x"].get<double>();
	posY = plane["y"].get<double>();
	width = plane["width"].get<double>();
	height = plane["height"].get<double>();
	mVelX = plane["velX"].get<double>();
	mVelY = plane["velY"].get<double>();
	shotDelay = plane["delay"].get<double>();

    aircraft = rm.getTexture("resources/images/Raptor.png");
    aircraft->width_ = width;
    aircraft->height_ = height;
	explosionTexture = rm.getTexture("resources/images/Invaders2.png");
}

Aircraft::~Aircraft()
{
    aircraft = nullptr;
    explosionTexture = nullptr;
}

void Aircraft::setClips()
{
    explosion[ 0 ].x = 0;
    explosion[ 0 ].y = 496;
    explosion[ 0 ].w = 48/3-1;
    explosion[ 0 ].h = 45/3;

    explosion[ 1 ].x = 16;
    explosion[ 1 ].y = 496;
    explosion[ 1 ].w = 48/3-1;
    explosion[ 1 ].h = 45/3;

    explosion[ 2 ].x = 32;
    explosion[ 2 ].y = 496;
    explosion[ 2 ].w = 48/3-1;
    explosion[ 2 ].h = 45/3;

    explosion[ 3 ].x = 49;
    explosion[ 3 ].y = 494;
    explosion[ 3 ].w = 48/3-1;
    explosion[ 3 ].h = 45/3;

    explosion[ 4 ].x = 65;
    explosion[ 4 ].y = 494;
    explosion[ 4 ].w = 48/3-1;
    explosion[ 4 ].h = 45/3;

    explosion[ 5 ].x = 81;
    explosion[ 5 ].y = 494;
    explosion[ 5 ].w = 48/3-1;
    explosion[ 5 ].h = 45/3;

}

void Aircraft::handleEvents(SDL_Event* event)
{

}

void Aircraft::update(const float deltaTime)
{
    if(!dead)
    {
        const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

        if( currentKeyStates[ SDL_SCANCODE_LEFT ] )
        {
            moveLeft(deltaTime);

            // Left Boundary
            if (posX <= 0)
                moveRight(deltaTime);
        }
        if( currentKeyStates[ SDL_SCANCODE_RIGHT ] )
        {
            moveRight(deltaTime);

            // Right Boundary
            if (posX + width >= game.screen_width)
                moveLeft(deltaTime);
        }
    }
    else
    {
        ++explosionFrame;

        //Cycle animation
        if( explosionFrame / 8 >= EXPLOSION_FRAMES )
        {
            explosionFrame = 0;
            explosionEnd = true;
        }
    }
}

void Aircraft::render()
{
    if(!dead)
    {
        aircraft->render(posX,posY);
    }
    else if( dead && !explosionEnd )
    {
        //Render current frame
        SDL_Rect* currentClip = &explosion[ explosionFrame / 8 ];

        SDL_Rect dest_rect;
        dest_rect.x = posX;
        dest_rect.y = posY;
        dest_rect.w = width;
        dest_rect.h = height;

        explosionTexture->render(currentClip,&dest_rect);
    }
}

void Aircraft::moveLeft(const float deltaTime)
{
    posX += -mVelX * speed * deltaTime;
    posY += mVelY * speed * deltaTime;
}

void Aircraft::moveRight(const float deltaTime)
{
    posX += mVelX * speed * deltaTime;
    posY += mVelY * speed * deltaTime;
}

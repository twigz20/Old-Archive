#include "Aircraft.h"
#include <sstream>

Aircraft::Aircraft()
{
    aircraft = new Texture("resources/images/Raptor.png");
	explosionTexture = new Texture("resources/images/explosion.png");

    setClips();

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
}

Aircraft::~Aircraft()
{
    if(aircraft != nullptr)
    {
        delete aircraft;
        aircraft = nullptr;
    }

    if(explosionTexture != nullptr)
    {
        delete explosionTexture;
        explosionTexture = nullptr;
    }
}

void Aircraft::setClips()
{
    explosion[ 0 ].x = 0;
    explosion[ 0 ].y = 0;
    explosion[ 0 ].w = 46;
    explosion[ 0 ].h = 46;

    explosion[ 1 ].x = 46;
    explosion[ 1 ].y = 0;
    explosion[ 1 ].w = 46;
    explosion[ 1 ].h = 46;

    explosion[ 2 ].x = 92;
    explosion[ 2 ].y = 0;
    explosion[ 2 ].w = 46;
    explosion[ 2 ].h = 46;

    explosion[ 3 ].x = 132;
    explosion[ 3 ].y = 0;
    explosion[ 3 ].w = 46;
    explosion[ 3 ].h = 46;

    explosion[ 4 ].x = 184;
    explosion[ 4 ].y = 0;
    explosion[ 4 ].w = 46;
    explosion[ 4 ].h = 46;

    explosion[ 5 ].x = 230;
    explosion[ 5 ].y = 0;
    explosion[ 5 ].w = 46;
    explosion[ 5 ].h = 46;

    explosion[ 6 ].x = 276;
    explosion[ 6 ].y = 0;
    explosion[ 6 ].w = 46;
    explosion[ 6 ].h = 46;

    explosion[ 7 ].x = 322;
    explosion[ 7 ].y = 0;
    explosion[ 7 ].w = 46;
    explosion[ 7 ].h = 46;

    explosion[ 8 ].x = 368;
    explosion[ 8 ].y = 0;
    explosion[ 8 ].w = 46;
    explosion[ 8 ].h = 46;

}

void Aircraft::update(const float deltaTime)
{
    if(!dead)
    {
        const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

        if( currentKeyStates[ SDL_SCANCODE_LEFT ] )
        {
            moveLeft(deltaTime);
        }
        if( currentKeyStates[ SDL_SCANCODE_RIGHT ] )
        {
            moveRight(deltaTime);
        }
    }
    else
    {
        ++explosionFrame;

        //Cycle animation
        if( explosionFrame / 9 >= EXPLOSION_FRAMES )
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
        SDL_Rect* currentClip = &explosion[ explosionFrame / 5 ];

        SDL_Rect dest_rect;
        dest_rect.x = posX;
        dest_rect.y = posY;
        dest_rect.w = width*1.75;
        dest_rect.h = height*1.75;

        explosionTexture->render(currentClip,&dest_rect);
    }
}

void Aircraft::moveLeft(float deltaTime)
{
    posX += -mVelX * speed * deltaTime;
    posY += mVelY * speed * deltaTime;
}

void Aircraft::moveRight(float deltaTime)
{
    posX += mVelX * speed * deltaTime;
    posY += mVelY * speed * deltaTime;
}

#include "State_Credits.h"
#include <stdexcept>

State_Credits::State_Credits() :
    game(Game::getInstance()),
	rm(ResourceManager::getInstance())
{
    background = nullptr;
    music = nullptr;

    background = rm.getTexture("resources/images/Game Over.png");
    music = rm.getMusic("resources/sounds/Tango.wav");

	// start off opaque
	alpha = 255;

	 //Play the music
    Mix_PlayMusic( music, -1 );
}

State_Credits::~State_Credits()
{
    background = nullptr;

    if(music)
    {
        Mix_HaltMusic();
        Mix_FreeMusic( music );
        music = nullptr;
    }
}

void State_Credits::handleEvents(const float deltaTime)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            game.popState();
        else if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym){
                     case SDLK_ESCAPE:
                          game.popState();
                          break;
                 }
        }
    }
}

void State_Credits::update(const float deltaTime)
{
    background->setAlpha(alpha);
	alpha--;

	if (alpha < 0)
		alpha = 0;

}

void State_Credits::render(const float deltaTime)
{
    if(alpha != 0)
        // Loads BackGround
        background->render(0.1,0.1);
    else
        game.popState();
}

void State_Credits::restart()
{

}

std::string State_Credits::name() const
{
    return "State_Credits";
}

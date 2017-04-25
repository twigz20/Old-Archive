#include "State_Intermission.h"
#include "State_Gameplay.h"
#include "State_Help.h"
#include <stdexcept>

State_Intermission::State_Intermission() :
    game(Game::getInstance()),
    rm(ResourceManager::getInstance())
{
    //music = nullptr;
	background = nullptr;

	background = rm.getTexture("resources/images/Main Menu.png");
	//music = rm.getMusic("resources/sounds/Tango.wav");

    //Play the music
    //Mix_FadeInMusic( music, -1, 25000 );
}

State_Intermission::~State_Intermission()
{
	background = nullptr;

    //Mix_FreeMusic( music );
    //music = nullptr;
}

void State_Intermission::handleEvents(const float deltaTime)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
	{
        bool leftClick = false;
        if(event.type == SDL_QUIT)
            game.popState();
	}
}

void State_Intermission::update(const float deltaTime)
{
    snprintf(scoreS,20,"Score: %06d",2);
    snprintf(lifeS,20,"Life: %d",3);
}

void State_Intermission::render(const float deltaTime)
{
    SDL_RenderClear( Game::getInstance().renderer );
    background->render(0,0);
    // Display Score
    auto scoreInfo = Texture(scoreS,SDL_Color{ 255, 255, 255, 128 },rm.getFont("resources/fonts/Exo-MediumItalic.otf", 20, false));
    scoreInfo.render(300,300);

    // Display Life
    auto lifeInfo = Texture(lifeS,SDL_Color{ 255, 255, 255, 128 },rm.getFont("resources/fonts/Exo-MediumItalic.otf", 20, false));
    lifeInfo.render(300,350);
}

void State_Intermission::restart()
{

}

std::string State_Intermission::name() const
{
	return "State_Intermission";
}


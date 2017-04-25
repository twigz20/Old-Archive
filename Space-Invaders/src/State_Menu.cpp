#include "State_Menu.h"
#include "State_Gameplay.h"
#include "State_HighScores.h"
#include <stdexcept>

State_Menu::State_Menu() :
    game(Game::getInstance()),
    rm(ResourceManager::getInstance())

{
    //music = nullptr;
	background = nullptr;
    play_button = nullptr;
    highScores_button = nullptr;
	quit_button = nullptr;

	background = rm.getTexture("resources/images/Main Menu.png");
	//music = rm.getMusic("resources/sounds/Tango.wav");
	play_button = new Button(300,390,200,100,rm.getTexture("resources/images/1 playerTransparent.png"));
	highScores_button = new Button(300,500,200,100,rm.getTexture("resources/images/High ScoresTransparent.png"));
	quit_button = new Button(300,610,200,100,rm.getTexture("resources/images/quitTransparent.png"));

    //Play the music
    //Mix_FadeInMusic( music, -1, 25000 );
}

State_Menu::~State_Menu()
{
    play_button = nullptr;
    highScores_button = nullptr;
	quit_button = nullptr;
	background = nullptr;

    //Mix_FreeMusic( music );
    //music = nullptr;
}

void State_Menu::handleEvents(const float deltaTime)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
	{
        bool leftClick = false;
        if(event.type == SDL_QUIT)
            game.popState();

        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                leftClick = true;
            }
            if(event.type == SDL_MOUSEBUTTONUP)
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    leftClick = false;
                }
            }

            const Uint8 Button = SDL_BUTTON_LEFT;

            if(play_button->CheckEvents(leftClick))
            {
                if( (event.button.button &Button) == Button && play_button->IsOnTop(play_button->getX(), play_button->getY(), play_button->getW(), play_button->getH(), event.button.x, event.button.y))
                {
                    // Popping Current State
                    game.toPop_ = name();

                    // Then Loading New State
                    game.pushState(std::make_shared<State_Gameplay>());
                }
            }

            if(highScores_button->CheckEvents(leftClick))
            {
                if( (event.button.button &Button) == Button && highScores_button->IsOnTop(highScores_button->getX(), highScores_button->getY(), highScores_button->getW(), highScores_button->getH(), event.button.x, event.button.y))
                {
                    game.pushState(std::make_shared<State_Help>());
                }
            }

            if(quit_button->CheckEvents(leftClick))
            {
                if( (event.button.button &Button) == Button && quit_button->IsOnTop(quit_button->getX(), quit_button->getY(), quit_button->getW(), quit_button->getH(), event.button.x, event.button.y))
                {
                    game.popState();
                }
            }
        }
	}
}

void State_Menu::update(const float deltaTime)
{

}

void State_Menu::render(const float deltaTime)
{
    SDL_RenderClear( Game::getInstance().renderer );
    background->render(0.1,0.1);
    play_button->DrawToScreen();
    highScores_button->DrawToScreen();
    quit_button->DrawToScreen();
}

void State_Menu::restart()
{

}

std::string State_Menu::name() const
{
	return "State_Menu";
}


#include "State_Pause.h"
#include "State_Gameplay.h"
#include "State_Help.h"
#include "State_Credits.h"
#include <stdexcept>

bool State_Pause::Init(Game* game)
{
	this->game = game;

	rm = new ResourceManager(game);

	try
	{
		rm->loadTexture("images/font_texture.png");
		rm->loadTexture("images/pause.png");
		rm->loadTexture("images/continue.png");
		rm->loadTexture("images/settings.png");
		rm->loadTexture("images/quit.png");
	}
	catch(std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	background = rm->getTexture("images/pause.png");
    continue_texture = rm->getTexture("images/continue.png");
	settings_texture = rm->getTexture("images/settings.png");
	quit_texture = rm->getTexture("images/quit.png");
	settings = new Button(230,520,150,50,settings_texture,game->getRenderer());
	quit = new Button(400,520,150,50,quit_texture,game->getRenderer());
	continue_button = new Button(300,460,150,50,continue_texture,game->getRenderer());

	blinking = 0.0f;

	return true;
}

void State_Pause::HandleEvents(SDL_Event* event)
{
    bool leftClick = false;
	if(event->type == SDL_QUIT)
		game->PopState();

    if(event->type == SDL_MOUSEBUTTONDOWN)
    {
        if(event->button.button == SDL_BUTTON_LEFT)
        {
            leftClick = true;
        }
        if(event->type == SDL_MOUSEBUTTONUP)
        {
            if(event->button.button == SDL_BUTTON_LEFT)
            {
                leftClick = false;
            }
        }

        const Uint8 Button = SDL_BUTTON_LEFT;

        if(continue_button->CheckEvents(leftClick))
        {
            if( (event->button.button &Button) == Button && continue_button->IsOnTop(continue_button->getX(), continue_button->getY(), continue_button->getW(), continue_button->getH(), event->button.x, event->button.y))
            {
                game->PopState();
            }
        }
        if(settings->CheckEvents(leftClick))
        {
            if( (event->button.button &Button) == Button && settings->IsOnTop(settings->getX(), settings->getY(), settings->getW(), settings->getH(), event->button.x, event->button.y))
            {

            }
        }
        if(quit->CheckEvents(leftClick))
        {
            if( (event->button.button &Button) == Button && quit->IsOnTop(quit->getX(), quit->getY(), quit->getW(), quit->getH(), event->button.x, event->button.y))
            {
                game->DeleteAllPreviousState(new State_Credits());
            }
        }
    }
}

void State_Pause::Update(float deltaTime)
{
	blinking += 2 * deltaTime;
	if(blinking >= 2.0f)
		blinking = 0.0f;
}

void State_Pause::Render()
{
    // Loads BackGround
    SDL_RenderCopy(game->getRenderer(), background, NULL, NULL);

    continue_button->DrawToScreen();
    settings->DrawToScreen();
    quit->DrawToScreen();
}

void State_Pause::Quit()
{
	delete rm;
	delete continue_button;
	delete settings;
	delete quit;

    SDL_DestroyTexture(background);
    background = NULL;
    SDL_DestroyTexture(continue_texture);
    continue_texture = NULL;
    SDL_DestroyTexture(settings_texture);
    settings_texture = NULL;
    SDL_DestroyTexture(quit_texture);
    quit_texture = NULL;
}

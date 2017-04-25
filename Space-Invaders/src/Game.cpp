#include "Game.h"
#include "GameState.h"

#include <algorithm>
#include <iostream>
#include <time.h>

Game::Game()
{
	renderer = nullptr;
    screen_width = 0;
	screen_height = 0;
	old_time = 0;
	quit_ = false;
	title = "";
}

Game::~Game()
{
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

bool Game::Init(const std::string& title, int screen_width, int screen_height, bool fullscreen)
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    else
    {
        std::cout << "Initialization Successul" << std::endl;
        this->title = title;
        this->screen_width = screen_width;
        this->screen_height = screen_height;

        Uint32 flags = SDL_WINDOW_SHOWN;

        if(fullscreen)
            flags = SDL_WINDOW_FULLSCREEN;

         //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
        }

        //Create window
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, flags);
		if( window == nullptr )
		{
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        else
        {
            std::cout << "Window Creation Successul" << std::endl;
            //Create renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( renderer == nullptr )
			{
                std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
                return false;
            }
            else
            {
                std::cout << "Renderer Creation Successul" << std::endl;
                //Clear screen and Set Default Color
                SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
                    return false;
                }

                std::cout << "Image Accessor Successul" << std::endl;
                //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
					return false;
				}

                std::cout << "Sound Accessor Successul" << std::endl;
				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
					return false;
				}
				std::cout << "Font Accessor Successul" << std::endl;
            }
        }
    }

    timer.start();
    return true;
}
void Game::Thread()
{
	while(!quit_)
	{
	    old_time = new_time;
        // Gives the number of miliseconds that have passed since SDL Initialized
		new_time = timer.getTicks();
		if(new_time > old_time)
        {
            // Seconds that have passed
			float deltaTime = (float)(new_time - old_time) / 1000.0f;

            handleEvents(deltaTime);
            update(deltaTime);
            render(deltaTime);

            if (toPop_ != "")
			{
				removeState(toPop_);
				toPop_ = "";
			}
        }
        countedFrames++;
        //Calculate and correct fps
        float avgFPS = countedFrames / ( timer.getTicks() / 1000.f );
        if( avgFPS > 2000000 )
        {
            avgFPS = 0;
        }

        // Render FPS to Window Title
        char buf[100];
        snprintf(buf, 100, "%s (fps: %u)",title.c_str(), (int)avgFPS);
        SDL_SetWindowTitle(window, buf);
	}
	quit();
}

void Game::handleEvents(const float deltaTime)
{
	if(states_.size() > 0)
	{
		states_.back()->handleEvents(deltaTime);
	}
}

void Game::update(const float deltaTime)
{
	if(states_.size() > 0)
	{
	    states_.back()->update(deltaTime);
	}
}

void Game::render(const float deltaTime)
{
    //Only draw when not minimized
    SDL_RenderClear(renderer);

    if(states_.size() > 0)
        states_.back()->render(deltaTime);

    SDL_RenderPresent(renderer);

    //Increase Frame Count
    countedFrames++;
}

void Game::quit()
{
	while(states_.size() > 0)
	{
		states_.pop_back();
	}
}

void Game::pushState(const std::shared_ptr<GameState>& state)
{
	std::cout << "pushing state " << state->name() << std::endl;
	states_.push_back(state);
//	if(state->init() == false)
//	{
//		quit_ = true;
//	}
}

void Game::popState()
{
	removeState(states_.back()->name());
}

void Game::removeState(const std::string& stateName)
{
	auto it = std::find_if(states_.begin(), states_.end(), [&](const std::shared_ptr<GameState>& state){ return state->name() == stateName; });
	if(it != states_.end())
	{
		std::cout << "removendo state " << stateName << std::endl;
		states_.erase(it);
		if(states_.size() == 0)
		{
			quit_ = true;
		}
	}
}

std::shared_ptr<GameState> Game::getState(const std::string& stateName)
{
	auto it = std::find_if(states_.begin(), states_.end(), [&](const std::shared_ptr<GameState>& state){ return state->name() == stateName; });
	if (it != states_.end())
	{
		return *it;
	}
	return std::shared_ptr < GameState >() ;
}

std::shared_ptr<GameState> Game::peekState()
{
	if(states_.size() > 0)
		return states_.back();

	return std::shared_ptr<GameState>();
}


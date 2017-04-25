#ifndef GAME_H
#define GAME_H

#include "GameState.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <vector>
#include <string>
#include <memory>
#include <random>
#include <cstdio>
#include "LTimer.h"

class Game
{
    private:
        SDL_Event event_;
        Uint32 new_time;
        Uint32 old_time;

        std::vector<std::shared_ptr<GameState>> states_;

        void update(const float deltaTime);
        void render(const float deltaTime);
        void handleEvents(const float deltaTime);

        void quit();

        //The FPS Timer
        LTimer timer;

        //Start counting frames per second
        int countedFrames = 0;
        std::string title;

    public:
        Game();
        virtual ~Game();

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event event;

        int screen_width;
        int screen_height;

        bool Init(const std::string& title, int screen_width, int screen_height, bool fullscreen);
        void Thread();

        //state management functions
        void pushState(const std::shared_ptr<GameState>& state);
        void popState();
        void removeState(const std::string& stateName);
        std::shared_ptr<GameState> peekState();
        std::shared_ptr<GameState> getState(const std::string& stateName);

        static Game& getInstance()
        {
            static Game game;
            return game;
        }
        bool quit_;
        std::mt19937 rng_;
        std::string toPop_;
};

#endif

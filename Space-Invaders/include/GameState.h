#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>

union SDL_Event;

class GameState
{
    public:
        virtual ~GameState() {};
        virtual void handleEvents(const float deltaTime) = 0;
        virtual void update(const float deltaTime) = 0;
        virtual void render(const float deltaTime) = 0;
        virtual void restart() = 0;
        virtual std::string name() const = 0;
};

#endif

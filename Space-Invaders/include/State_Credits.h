#ifndef STATE_CREDITS
#define STATE_CREDITS

#include "GameState.h"
#include "Resource Manager.h"

class State_Credits : public GameState
{
    private:
        std::shared_ptr<Texture> background;
        Mix_Music* music;

        int alpha;

    public:
        State_Credits();
        ~State_Credits();

        Game& game;
        ResourceManager& rm;

        void handleEvents(const float deltaTime);
        void update(const float deltaTime);
        void render(const float deltaTime);
        void restart();
        std::string name() const;
};

#endif

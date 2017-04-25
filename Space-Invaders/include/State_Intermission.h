#ifndef STATE_INTRO
#define STATE_INTRO

#include "GameState.h"
#include "Resource Manager.h"

class State_Intermission: public GameState
{
    public:
    Game& game;
    ResourceManager& rm;
    //Mix_Music* music;
	std::shared_ptr<Texture> background;

	State_Intermission();
    ~State_Intermission();

	void handleEvents(const float deltaTime);
    void update(const float deltaTime);
    void render(const float deltaTime);
    void restart();
    std::string name() const;

    char scoreS[20];
    char lifeS[20];
};

#endif

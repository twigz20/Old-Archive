#ifndef STATE_MENU
#define STATE_MENU

#include "GameState.h"
#include "Resource Manager.h"

#include "Button.h"

class State_Menu : public GameState
{
    public:
    Game& game;
    ResourceManager& rm;
    //Mix_Music* music;
	std::shared_ptr<Texture> background;

	State_Menu();
    ~State_Menu();

    Button* play_button;
    Button* highScores_button;
	Button* quit_button;

	void handleEvents(const float deltaTime);
    void update(const float deltaTime);
    void render(const float deltaTime);
    void restart();
    std::string name() const;
};

#endif

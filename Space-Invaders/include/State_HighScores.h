#ifndef STATE_HELP
#define STATE_HELP

#include "GameState.h"
#include "Resource Manager.h"

#include "Button.h"

class State_Help : public GameState
{
    public:
        Game& game;
        ResourceManager& rm;
        //Mix_Music* music;
        std::shared_ptr<Texture> background;
        Texture* highScoresText;
        Texture* allScores;

        State_Help();
        ~State_Help();

        Button* back_button;

        void handleEvents(const float deltaTime);
        void update(const float deltaTime);
        void render(const float deltaTime);
        void restart();
        std::string name() const;

        void fileRead();
        std::vector<std::string> scores;
        std::vector<Texture*> s;
};
#endif

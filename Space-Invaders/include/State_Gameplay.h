#ifndef STATE_GAMEPLAY
#define STATE_GAMEPLAY

#include "GameState.h"
#include "Resource Manager.h"

#include "Aircraft.h"
#include "Bullet.h"
#include "Button.h"
#include "CreepInfo.h"
#include "LTimer.h"

#include <sstream>

class State_Gameplay : public GameState
{
    public:
        Game& game;
        ResourceManager& rm;
        LTimer time;
        Mix_Music* gameSound;
        Mix_Chunk* explosion;
        std::shared_ptr<Texture> levelCleared;
        std::shared_ptr<Texture> win;

        State_Gameplay();
        ~State_Gameplay();

        Aircraft aircraft;
        std::vector<std::unique_ptr<CreepInfo>> creeps;
        std::vector<std::unique_ptr<Bullet>> bullets;

        void checkCollisions(float deltaTime);
        void handleEvents(const float deltaTime);
        void update(const float deltaTime);
        void render(const float deltaTime);
        void restart();
        std::string name() const;

        std::string gameLevel(int level);
        std::string levelID;
        int lev;
        const int maxLev = 3;
        bool gameOver;
        bool levelChange;
        bool wallTouch;
        bool fileWritten;

        int score;
        bool scoreChange;
        char scoreS[20];
        int life;
        bool lifeChange;
        char lifeS[20];

        int intermissionTimer;

        Button* quit_button;
        Button* menu_button;
        Button* restart_button;
        Button* highScores_button;
        void fileWrite();
};

#endif

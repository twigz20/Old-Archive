#ifndef STATE_PAUSE
#define STATE_PAUSE

#include "GameState.h"
#include "Resource Manager.h"
#include "Font.h"
#include "Button.h"

class State_Pause : public GameState
{
    private:
        ResourceManager* rm;
        SDL_Texture* background;
        SDL_Texture* continue_texture;
        SDL_Texture* settings_texture;
        SDL_Texture* quit_texture;

        Button* continue_button;
        Button* settings;
        Button* quit;

        /*This will be used to determine whether
            the blinking text is visible or not at
            certain moment */
        float blinking;
    public:
        bool Init(Game* game);
        void HandleEvents(SDL_Event* event);
        void Update(float deltaTime);
        void Render();
        void Quit();
};

#endif

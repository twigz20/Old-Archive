#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <sstream>

class Game;
class Window
{
    public:
        //Intializes internals
        Window();

        //Creates window
        bool init(Game* game);

        //Creates renderer from internal window
        SDL_Renderer* createRenderer();

        //Handles window events
        void handleEvent( SDL_Event& event );

        //Deallocates internals
        void free();

        //Window dimensions
        int getWidth();
        int getHeight();
        SDL_Window* getWindow() { return window; }

        //Window focii
        bool hasMouseFocus();
        bool hasKeyboardFocus();
        bool isMinimized();

    private:
        //Game
        Game* game;

        //Window data
        SDL_Window* window;

        //Window dimensions
        int mWidth;
        int mHeight;

        //Window focus
        bool mMouseFocus;
        bool mKeyboardFocus;
        bool mFullScreen;
        bool mMinimized;

        //Screen dimension constants
        static const int SCREEN_WIDTH = 800;
        static const int SCREEN_HEIGHT = 800;
};

#endif // WINDOW_H

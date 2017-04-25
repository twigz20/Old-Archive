#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <string>

#include "Texture.h"

class Button
{
    public:
        Button(int x,int y, int w, int h,std::shared_ptr<Texture> Image);
        void DrawToScreen();
        bool CheckEvents(bool leftMousePressed);
        bool IsOnTop(int objectX, int objectY, int objectW, int objectH, int mouseX, int mouseY );
        int getX() { return x; }
        int getY() { return y; }
        int getW() { return w; }
        int getH() { return h; }

    private:
        std::shared_ptr<Texture> ButtonImage;
        SDL_Rect ButtonRect;
        SDL_Event event;
        int MouseX,MouseY;

        int x,y,w,h;
};


#endif // BUTTON_H

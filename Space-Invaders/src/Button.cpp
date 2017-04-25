#include "Button.h"
#include <SDL.h>
#include <string>

using namespace std;

Button::Button(int x,int y, int w, int h,std::shared_ptr<Texture> Image):
    MouseX(0),
    MouseY(0)
{
    ButtonImage = Image;
    ButtonRect.x = x;
    ButtonRect.y = y;
    ButtonRect.w = w;
    ButtonRect.h = h;

    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

void Button::DrawToScreen()
{
    ButtonImage->render(NULL,&ButtonRect);
}

bool Button::CheckEvents(bool leftMousePressed)
{
    if(leftMousePressed)
    {
        if((MouseX > ButtonRect.x) && (MouseY > ButtonRect.y) && (MouseX < (ButtonRect.x + ButtonRect.w)) && (MouseY < (ButtonRect.y + ButtonRect.h))){
            return true;
        }
    }
    return true;
}

bool Button::IsOnTop(int objectX, int objectY, int objectW, int objectH, int mouseX, int mouseY )
{
	if( mouseX < objectX || mouseY < objectY )
	{
		return false;
	}
	if( mouseX > objectX+objectW || mouseY > objectY+objectH )
	{
		return false;
	}

	return true;
}

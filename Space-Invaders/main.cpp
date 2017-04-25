#include "Game.h"
#include "State_Menu.h"

#include <iostream>

int main(int argc, char** args)
{
    try
	{
		Game& game = Game::getInstance();
		if(game.Init("Twigzolupolus Inc", 800, 960, false))
        {
            game.pushState(std::make_shared<State_Menu>());
            game.Thread();
        }
	}
	catch (std::exception& e)
	{
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}
    return 0;
}

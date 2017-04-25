//main.cpp

#include "Game.h"

int main() {
	Game app;
	app.ChangeState(Game::gameStates::MAINMENU);

	app.Run();

	return 0;
}
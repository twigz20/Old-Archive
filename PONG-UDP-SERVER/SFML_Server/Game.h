#pragma once

#include <iostream>
#include <memory>

#include "Music.h"
#include "GameState.h"

//including the Gamestate-Headers
#include "MainMenuState.h"
#include "MultiplayerState.h"
#include "PlayState.h"
#include "SplashState.h"

//screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Game
{
public:
	Game();
	~Game();

	enum class gameStates{ SPLASHSTATE, MAINMENU, INTRO, PLAY, MULTIPLAYER, SETTINGS, GRAPHICSET, SOUNDSET, DIFFSET, HIGHSCORELIST };

	void Run();
	void ChangeState(gameStates newState);

	const bool isRunning() { return running; };

	void setRunning(bool running);

	sf::RenderWindow	window;
	sf::Time elapsedTime;

private:
	void Quit();
	void Update();
	void HandleEvents();
	void Render();

	void checkScreenSet();

	std::unique_ptr<GameState> currentState;
	MenuMusic	menMusic;

	int volume;
	bool running;
	
	sf::Clock clock;
};


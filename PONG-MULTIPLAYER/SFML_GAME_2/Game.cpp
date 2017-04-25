#include "Game.h"
#include <Windows.h>


Game::Game()
{
	running = true;

	// Check Setting for fullscreen
	checkScreenSet();

	// Window Settings
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	// Hide the console
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	//Sound and Music
	volume = 50;
	menMusic.LoadMusic(volume);
}


Game::~Game()
{
	//currentState = nullptr;
}

void Game::Run() {
	while (running) {
		elapsedTime = clock.getElapsedTime();
		//trap mouse curser in window
		//HWND hwnd;
		//hwnd = FindWindow(0, LPCWSTR("Twigzolupolus Inc"));
		RECT r;

		//top left coords
		r.left = window.getPosition().x;
		r.top = window.getPosition().y;

		//bottom right coords
		r.right = window.getPosition().x + window.getSize().x;
		r.bottom = window.getPosition().y + window.getSize().y;

		//clip mouse to window
		//GetWindowRect(hwnd, &r);
		//ClipCursor(&r);

		//do the game stuff
		Update();
		HandleEvents();
		Render();
		Quit();

		clock.restart();
	}
}

void Game::ChangeState(gameStates newState)
{
	switch (newState) {
	case gameStates::MAINMENU:
		currentState = std::move(std::unique_ptr<MainMenuState>(new MainMenuState));
		menMusic.PlayMusic("menu");
		//ingMusic.Pause();
		break;
	case gameStates::MULTIPLAYER:
		currentState = std::move(std::unique_ptr<MultiplayerState>(new MultiplayerState));
		menMusic.StopMusic();
		//menMusic.Pause("menu");
		menMusic.PlayMusic("intro");
		break; 
	case gameStates::PLAY:
		currentState = std::move(std::unique_ptr<PlayState>(new PlayState));
		menMusic.StopMusic();
		//menMusic.Pause("intro");
		//ingMusic.PlayMusic();
		break;
	/*case gameStates::HIGHSCORE:
		CurrentState = std::move(std::unique_ptr<Highscore>(new Highscore));
		break;
	case gameStates::SETTINGS:
		CurrentState = std::move(std::unique_ptr<Settings>(new Settings));
		checkScreenSet();
		break;
	case gameStates::DIFFSET:
		CurrentState = std::move(std::unique_ptr<DiffSet>(new DiffSet));
		break;
	case gameStates::GRAPHICSET:
		CurrentState = std::move(std::unique_ptr<GraphicSet>(new GraphicSet));
		break;
	case gameStates::SOUNDSET:
		CurrentState = std::move(std::unique_ptr<SoundSet>(new SoundSet));
		break;
	case gameStates::HIGHSCORELIST:
		CurrentState = std::move(std::unique_ptr<HighscoreList>(new HighscoreList));
		break;*/
	case gameStates::SPLASHSTATE:
		currentState = std::move(std::unique_ptr<SplashState>(new SplashState));
	}
}

void Game::setRunning(bool running)
{
	this->running = running;
}

void Game::Quit()
{
	if (!running)
		window.close();
}

void Game::Update()
{
	currentState->Update(*this);
}

void Game::HandleEvents()
{
	currentState->HandleEvents(*this);
}

void Game::Render()
{
	window.clear();
	currentState->Render(*this);
	window.display();
}

void Game::checkScreenSet()
{
	window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Twigzolupolus");
}

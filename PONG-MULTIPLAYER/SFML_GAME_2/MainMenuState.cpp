#include "MainMenuState.h"



MainMenuState::MainMenuState()
{
	// sound & music
	menuSound.LoadSoundBuffer();
	menuVolume = 50;
	menuSound.SetBuffer(menuVolume);

	menuSelection = 0;

	menuElapsedTime = 0.0f;

	// Initialize buttons
	initButtons();

	//initialize Fading
	initFading();
}


MainMenuState::~MainMenuState()
{
}

void MainMenuState::HandleEvents(Game &game)
{
	sf::Event event;

	while (game.window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game.setRunning(false);

		//Only accept Input when not fading out
		if (!menuStartFading) {
			//Keyboard selection
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Up:
					if (menuSelection > 0) {
						menuSelection -= 1;
						menuSound.PlaySound("select");
					}
					else
						menuSelection = 0;
					break;

				case sf::Keyboard::Down:
					if (menuSelection < 4) {
						menuSelection += 1;
						menuSound.PlaySound("select");
					}
					else
						menuSelection = 4;
					break;

				case sf::Keyboard::Return:
						menuStartFading = true;
					break;

				case sf::Keyboard::Escape:
					game.setRunning(false);
					break;

				default:
					break;
				}
			}
		}
		//mouse selection
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mouseIsIntersecting()) {
			menuStartFading = true;
		}
	}//End if !fadeOut
}

void MainMenuState::Update(Game &game)
{
	// Update Mouse Rectangle
	menuMouseRect = sf::Rect<float>((float)sf::Mouse::getPosition(game.window).x,
		(float)sf::Mouse::getPosition(game.window).y + 1.0f, 1.0f, 1.0f);

	// Check for Mouse Selection if not fading out
	if (!menuStartFading) {
		updateMouseSelection();
	}

	// Do color shit
	updateButtonColor();

	//get the elapsed time
	menuElapsedTime = (float)menuClock.restart().asMilliseconds();

	// See if we should change the ScreenState
	if (menuStartFading) {
		fadeOut();
		ChangeState(game);
	}
}

void MainMenuState::Render(Game &game)
{
	// draw Background
	

	// Draw Buttons
	settings.Render(game.window);
	close.Render(game.window);
	highscore.Render(game.window);
	multiplayer.Render(game.window);
	play.Render(game.window);
}

// Init Fading
void MainMenuState::initFading()
{
	if (menuFadingTex.loadFromFile("Resources/Images/settings.png")) {
		perror("could not load fading graphic\n");
	}

	menuFadingTex.setRepeated(true);

	menuFadingSprite.setTexture(menuFadingTex);
	menuFadingSprite.setPosition(0.0f, 0.0f);

	menuStartFading = false;
	menuFadingAlpha = 0;
	menuFadingSprite.setColor(sf::Color(128, 128, 128, menuFadingAlpha));
}

// Fadeout by increasing the alpha-value
void MainMenuState::fadeOut()
{
	// increase as long as value is not 255
	if (menuFadingAlpha <= 255) {
		menuFadingAlpha += (int)menuElapsedTime / 4;
		// set alpha
		menuFadingSprite.setColor(sf::Color(255, 255, 255, menuFadingAlpha));
	}
}

//change the state (after fadeOut is done)
void MainMenuState::ChangeState(Game& game) {
	//if the fadeOut is done we are ready to change the screen
	if (menuFadingAlpha >= 255) {

		//Go to the game
		if (menuSelection == 0) {
			//if (!game.getIntroPlayed())
				//game.ChangeState(Game::gameStates::INTRO);

			//else {
				//game.setIntroPlayed(true);
				game.ChangeState(Game::gameStates::PLAY);
			//}
		}

		//Go to the Multiplayer
		else if (menuSelection == 1) {
			game.ChangeState(Game::gameStates::MULTIPLAYER);
		}

		//Go to the HighscoreList
		else if (menuSelection == 2) {
			//game.ChangeState(Game::gameStates::HIGHSCORELIST);
		}

		//Go to the Settings
		else if (menuSelection == 3) {
			//game.ChangeState(Game::gameStates::SETTINGS);
		}

		//Close the Game
		else if (menuSelection == 4) {
			game.setRunning(false);
		}
	}
}

//returns true if mouse is intersecting with button
bool MainMenuState::mouseIsIntersecting()
{
	//play
	if (play.getGlobalBounds().intersects(menuMouseRect)) {
		return true;
	}

	//multiplayer
	if (multiplayer.getGlobalBounds().intersects(menuMouseRect)) {
		return true;
	}

	//highscorelist
	if (highscore.getGlobalBounds().intersects(menuMouseRect)) {
		return true;
	}
	//settings
	if (settings.getGlobalBounds().intersects(menuMouseRect)) {
		return true;
	}
	//close
	if (close.getGlobalBounds().intersects(menuMouseRect)) {
		return true;
	}

	//no intersection
	return false;
}

//set up the Buttons
void MainMenuState::initButtons()
{
	play.setStringAndSize("play", 70);
	play.setPosition(SCREEN_WIDTH/2 - play.getGlobalBounds().width, 50);

	multiplayer.setStringAndSize("multiplayer", 70);
	multiplayer.setPosition((SCREEN_WIDTH / 2) - play.getGlobalBounds().width, 150);

	highscore.setStringAndSize("highscore", 70);
	highscore.setPosition(SCREEN_WIDTH / 2 - play.getGlobalBounds().width, 250);

	settings.setStringAndSize("settings", 70);
	settings.setPosition(SCREEN_WIDTH / 2 - play.getGlobalBounds().width, 350);

	close.setStringAndSize("close", 70);
	close.setPosition(SCREEN_WIDTH / 2 - play.getGlobalBounds().width, 450);
}

// Update the Color of the Buttons depending on their selection
void MainMenuState::updateButtonColor()
{
	//Play
	if (menuSelection == 0) {
		play.setColor(sf::Color(165, 250, 0));
		multiplayer.setColor(sf::Color(255, 255, 255));
		highscore.setColor(sf::Color(255, 255, 255));
		settings.setColor(sf::Color(255, 255, 255));
		close.setColor(sf::Color(255, 255, 255));
	}

	//Multiplayer
	else if (menuSelection == 1) {
		play.setColor(sf::Color(255, 255, 255));
		multiplayer.setColor(sf::Color(0, 153, 204));
		highscore.setColor(sf::Color(255, 255, 255));
		settings.setColor(sf::Color(255, 255, 255));
		close.setColor(sf::Color(255, 255, 255));
	}

	//Highscore
	else if (menuSelection == 2) {
		play.setColor(sf::Color(255, 255, 255));
		multiplayer.setColor(sf::Color(255, 255, 255));
		highscore.setColor(sf::Color(255, 0, 255));
		settings.setColor(sf::Color(255, 255, 255));
		close.setColor(sf::Color(255, 255, 255));
	}

	//Settings
	else if (menuSelection == 3) {
		play.setColor(sf::Color(255, 255, 255));
		multiplayer.setColor(sf::Color(255, 255, 255));
		highscore.setColor(sf::Color(255, 255, 255));
		settings.setColor(sf::Color(255, 255, 0));
		close.setColor(sf::Color(255, 255, 255));
	}

	//Close
	else {
		play.setColor(sf::Color(255, 255, 255));
		multiplayer.setColor(sf::Color(255, 255, 255));
		highscore.setColor(sf::Color(255, 255, 255));
		settings.setColor(sf::Color(255, 255, 255));
		close.setColor(sf::Color(255, 0, 0));
	}
}

//Update the Selection done with the cursor
void MainMenuState::updateMouseSelection()
{
	//play
	if (play.getGlobalBounds().intersects(menuMouseRect)) {
		if (menuSelection != 0) {
			menuSelection = 0;
			menuSound.PlaySound("select");
		}
	}

	//multiplayer
	if (multiplayer.getGlobalBounds().intersects(menuMouseRect)) {
		if (menuSelection != 1) {
			menuSelection = 1;
			menuSound.PlaySound("select");
		}
	}

	//highscorelist
	if (highscore.getGlobalBounds().intersects(menuMouseRect)) {
		if (menuSelection != 2) {
			menuSelection = 2;
			menuSound.PlaySound("select");
		}
	}
	//settings
	if (settings.getGlobalBounds().intersects(menuMouseRect)) {
		if (menuSelection != 3) {
			menuSelection = 3;
			menuSound.PlaySound("select");
		}
	}
	//close
	if (close.getGlobalBounds().intersects(menuMouseRect)) {
		if (menuSelection != 4) {
			menuSelection = 4;
			menuSound.PlaySound("select");
		}
	}
}

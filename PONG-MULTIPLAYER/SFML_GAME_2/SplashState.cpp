//SplashState.cpp

#include "SplashState.h"

SplashState::SplashState() {

	 bg.setFilePath("settings.png");

	//init credits
	initCredits();

	//initialize Fading
	initFading();
}

SplashState::~SplashState() {
	//Empty
}

void SplashState::HandleEvents(Game &game) {
	sf::Event pEvent;

	while (game.window.pollEvent(pEvent)) {
		if (pEvent.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			game.setRunning(false);
	}
}


void SplashState::Update(Game &game) {

	//get the elapsed time
	 elapsedTime = (float) clock.restart().asMilliseconds();

	if (! startFading) {
		//Update the Credits to get them fading
		updateCredits();
	}

	//See if we should change the ScreenState
	if ( startFading) {
		fadeOut();
		changeState(game);
	}
}

void SplashState::Render(Game &game) {
	//draw Background
	// bg.Render(game.window);

	//draw Credits
	if (! sfmlShown) {
		game.window.draw( sfmlSprite);
	}

	if (! CreditsShown) {
		 credits.Render(game.window);
	}

	//draw fading sprite
	game.window.draw( fadingSprite);
}

//Init fading
void SplashState::initFading() {

	if (! fadingTex.loadFromFile("Resources/Images/settings.png")) {
		perror("could not load fading graphic\n");
	}
	 fadingTex.setRepeated(true);

	 fadingSprite.setTexture( fadingTex);
	 fadingSprite.setPosition(0.0f, 0.0f);

	 startFading = false;
	 fadingAlpha = 0;
	 fadingSprite.setColor(sf::Color(128, 128, 128,  fadingAlpha));

}

//fade out by increasing the alpha-Value
void SplashState::fadeOut() {

	//increase as long as value is not 255
	if ( fadingAlpha <= 255) {
		 fadingAlpha += (int) elapsedTime / 2;
		//set alpha
		 fadingSprite.setColor(sf::Color(255, 255, 255,  fadingAlpha));
	}
}

//change the state (after fadeOut is done)
void SplashState::changeState(Game& game) {
	//if the fadeOut is done we are ready to change the screen
	if ( fadingAlpha >= 255) {
		game.ChangeState(Game::gameStates::MAINMENU);
	}
}

//initialize Credits
void SplashState::initCredits() {

	//SMFL
	 sfmlTex.loadFromFile("Resources/Images/sfml-logo-small.png");
	 sfmlTex.setSmooth(true);

	 alphaSFML = 0;
	 sfmlShown = false;
	 sfmlSprite.setTexture( sfmlTex);
	 sfmlSprite.setOrigin( sfmlTex.getSize().x / 2.0f,  sfmlTex.getSize().y / 2.0f);
	 sfmlSprite.setPosition(SCREEN_WIDTH/2 - (sfmlSprite.getScale().x/2), SCREEN_HEIGHT / 2 - sfmlSprite.getScale().y);
	 sfmlSprite.setColor(sf::Color(255, 255, 255,  alphaSFML));

	//Credits
	std::string tmp;
	//Programmed by
	tmp = "PROGRAMMED BY\n\n";
	tmp += "\t - Twigz\n";
	tmp += "\t   (Twigzolupolus inc)\n\n";

	//Special thanks
	tmp += "SPECIAL THANKS TO \n\n";
	tmp += "\t - Wiznick\n";
	tmp += "\t - Armid\n";
	tmp += "\t - Dillon";

	 alphaCredits = 0;
	 CreditsShown = false;
	 credits.setFont("OptimusPrincepsSemiBold.ttf");
	 credits.setStringAndSize(tmp, 30);
	 credits.setPosition(150, 70);
	 credits.setColor(sf::Color(255, 255, 255,  alphaCredits));
}

//Update the Credits to change their alpha-Value
void SplashState::updateCredits() {

	//Start with SFML
	if ( alphaSFML <= 255 && ! sfmlShown) {
		 alphaSFML += (int) elapsedTime / 8;
		 sfmlSprite.setColor(sf::Color(255, 255, 255,  alphaSFML));
	}
	if ( alphaSFML >= 255 && ! sfmlShown) {
		 sfmlShown = true;
	}

	//Then Credits
	if ( alphaCredits <= 255 && ! CreditsShown &&  sfmlShown) {
		 alphaCredits += (int) elapsedTime / 10;
		 credits.setColor(sf::Color(255, 255, 255,  alphaCredits));
	}
	if ( alphaCredits >= 255 && ! CreditsShown) {
		 CreditsShown = true;
	}

	//After everything has been shown - init fadeout
	if ( sfmlShown &&  CreditsShown) {
		 startFading = true;
	}
}
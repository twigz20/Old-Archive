#pragma once
#include "GameState.h"

#include "Game.h"

class SplashState : public GameState {
public:
	SplashState();
	~SplashState();
	void HandleEvents(Game &game);
	void Update(Game &game);
	void Render(Game &game);

private:
	///PRIVATE MEMBER FUNCTIONS
	void initCredits();
	void initFading();
	void fadeOut();
	void changeState(Game& game);
	void updateCredits();

	///PRIVATE MEMBER VARIABLES

	//Menu related
	Background  bg;			///< Background
	sf::Clock   clock;		///< Clock to measure elapsed Time

								//SFML
	sf::Texture sfmlTex;		///< Texture of SFML-Logo
	sf::Sprite  sfmlSprite;	///< Sprite for SFML Texture
	int alphaSFML;			///< Alpha value for fading
	bool sfmlShown;			///< If this is true -> SFML logo has been shown

								//Credits
	Text credits;				///< Textholder for Credits
	int alphaCredits;			///< Alpha value for fading
	bool CreditsShown;		///< If this is true -> credits have been shown

								//Statechange
	sf::Texture fadingTex;	///< Texture for fade IN/OUT
	sf::Sprite fadingSprite;	///< Sprite for fade IN/OUT
	bool startFading;			///< if this is true -> start increasing alpha
	int fadingAlpha;			///< Alpha-Value for fade IN/OUT
	float elapsedTime;		///< time elapsed in frame
};


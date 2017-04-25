#pragma once

#include "Game.h"
#include "MenuSfx.h"

class MainMenuState :
	public GameState
{
public:
	MainMenuState();
	~MainMenuState();
	void HandleEvents(Game &game);
	void Update(Game &game);
	void Render(Game &game);

private:
	void initFading();
	void fadeOut();
	void ChangeState(Game &game);
	bool mouseIsIntersecting();

	void initButtons();
	void updateButtonColor();
	void updateMouseSelection();

	// Menu Related
	MenuSound menuSound;				/// to handle the sound
	Background menuBg;				/// Background
	Text play;						/// Buttons
	Text multiplayer;				/// Buttons
	Text highscore;					/// Buttons
	Text settings;					/// Buttons
	Text close;						/// Buttons

	short int menuSelection;		/// To see what has been selected
	int menuVolume;					/// Volume for sound

	float menuElapsedTime;			///< time elapsed in frame

	sf::Rect<float> menuMouseRect;	/// Rectangle for Mouse Intersection

	sf::Clock menuClock;			/// Clock to measure elapsedTime

	// Statechange
	sf::Texture menuFadingTex;		/// Texture for fade IN/OUT
	sf::Sprite menuFadingSprite;	/// Sprite for fade  IN/OUT
	bool menuStartFading;			/// if this is true -> start increasing alpha
	int menuFadingAlpha;			/// Alpha value for fade IN/OUT
};


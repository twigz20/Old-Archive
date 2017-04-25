#ifndef FONT_H
#define FONT_H

#include "Game.h"
#include <iostream>

class Font
{
private:
	Game* game;
	SDL_Texture* sheet;

    /*Number of characters on horizontal and vertical axis
	of the font sheet. */
	int num_charsX;
	int num_charsY;

	/* Width and height of a single character in pixels. */
	int char_width;
	int char_height;
public:
	Font(Game* game, SDL_Texture* sheet, int num_charsX, int num_charsY, int char_width, int char_height);

    /* This function will render a single character on given coordinates.
	Note we're passing unsigned char - an ASCII value of a character!
	This is beneficial because our sheet is in ASCII order. */
	void RenderChar(float x, float y, unsigned char data);

	/* This uses RenderChar to render a string of characters */
	void RenderString(float x, float y, std::string data);
};

#endif

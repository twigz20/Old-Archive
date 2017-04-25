#include "Font.h"

Font::Font(Game* game, SDL_Texture* sheet, int num_charsX, int num_charsY, int char_width, int char_height)
{
	this->game = game;
	this->sheet = sheet;
	this->num_charsX = num_charsX;
	this->num_charsY = num_charsY;
	this->char_width = char_width;
	this->char_height = char_height;
}

void Font::RenderChar(float x, float y, unsigned char data)
{
	SDL_Rect src_rect, dst_rect;

	src_rect.x = (data % num_charsX) * char_width;
	src_rect.y = (data / num_charsX) * char_height;
	src_rect.w = char_width;
	src_rect.h = char_height;

	dst_rect.x = x;
	dst_rect.y = y;
	dst_rect.w = char_width;
	dst_rect.h = char_height;

	SDL_RenderCopy(game->getRenderer(), sheet, &src_rect, &dst_rect);
}
void Font::RenderString(float x, float y, std::string data)
{
	for(unsigned int i=0; i<data.length(); i++)
	{
		RenderChar(x + i*char_width, y, data.at(i));
	}
}

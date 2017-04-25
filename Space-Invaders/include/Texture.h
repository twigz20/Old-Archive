#ifndef TEXTURE_H
#define TEXTURE_H

#include "Game.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

class Texture {
public:
	Texture(const std::string text, SDL_Color color, TTF_Font *font);
	explicit Texture(SDL_Texture* texture);
	explicit Texture(const std::string& filePath);
	virtual ~Texture();

	Texture(Texture&& src);
	Texture& operator=(Texture&& src);

	//Set color modulation
	void setColor( Uint8 red, Uint8 green, Uint8 blue );

	//Set blending
	void setBlendMode( SDL_BlendMode blending );

	//Set alpha modulation
	void setAlpha( Uint8 alpha );

	//Renders texture at given point
	void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE ) const;
    void render( SDL_Rect* src = NULL, SDL_Rect* dst = NULL );

	//Image dimensions
	int width_;
	int height_;

	SDL_Texture* texture_;
private:
	Game& game;

};

#endif // TEXTURE_H

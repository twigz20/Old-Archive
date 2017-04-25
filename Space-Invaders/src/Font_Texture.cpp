#include <iostream>
#include "Font_Texture.h"

Font_Texture::Font_Texture()
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
    gFont = nullptr;
}

Font_Texture::Font_Texture(TTF_Font* src)
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;

    if(src != nullptr)
        gFont = src;
    else
        gFont = nullptr;
}

Font_Texture::~Font_Texture()
{
    //Deallocate
    free();
}

void Font_Texture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void Font_Texture::loadFont(std::string filename)
{
    //Open the font
	gFont = TTF_OpenFont( filename.c_str(), 28 );
	if( gFont == NULL )
	{
		std::cout << "Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
}

TTF_Font* Font_Texture::getFont()
{
    return gFont;
}

void Font_Texture::render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;

    }

    //Render to screen
    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

void Font_Texture::setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    //Render text
    textColor = { r, g, b, a };
}

bool Font_Texture::loadFromRenderedText( std::string textureText, SDL_Renderer* gRenderer)
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL )
        {
            std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }

    //Return success
    return mTexture != NULL;
}

int Font_Texture::getWidth()
{
    return mWidth;
}

int Font_Texture::getHeight()
{
    return mHeight;
}

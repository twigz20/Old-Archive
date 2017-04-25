#ifndef FONT_TEXTURE_H
#define FONT_TEXTURE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <cmath>

//Texture wrapper class
class Font_Texture
{
    public:
        //Initializes variables
        Font_Texture();
        Font_Texture(TTF_Font* src);

        //Deallocates memory
        ~Font_Texture();

        void loadFont(std::string filename);
        //Renders texture at given point
        void render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
        //Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Renderer* gRenderer );

        void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        TTF_Font* getFont();
        //Deallocates texture
        void free();

        //Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        //The actual hardware texture
        SDL_Texture* mTexture;
        TTF_Font* gFont;
        SDL_Color textColor;
        //Image dimensions
        int mWidth;
        int mHeight;

        //Loads
};

#endif // TEXTURE_H

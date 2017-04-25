#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Game.h"
#include "Texture.h"

#include <SDL.h>

#include <map>
#include <memory>

typedef struct _TTF_Font TTF_Font;
// struct Mix_Chunk;

class ResourceManager
{
public:
	virtual ~ResourceManager();

	std::shared_ptr<Texture> getTexture(const std::string& text, SDL_Color color, TTF_Font* font);
	std::shared_ptr<Texture> getTexture(const std::string& filename);
	TTF_Font* getFont(const std::string& filename, int fontSize, bool bold = false);
	Mix_Music* getMusic(const std::string& filename);
	Mix_Chunk* getSound(const std::string& filename);

	static ResourceManager& getInstance()
	{
		static ResourceManager rm;
		return rm;
	}

private:
	ResourceManager();
	std::map<std::string, std::shared_ptr<Texture>> textures;
	std::map<std::string, TTF_Font*> fonts;
	std::map<std::string, Mix_Chunk*> sounds;
	std::map<std::string, Mix_Music*> music;
};

#endif

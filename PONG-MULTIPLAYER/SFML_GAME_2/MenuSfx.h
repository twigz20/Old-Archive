#pragma once

#include <SFML\Audio.hpp>

class MenuSound
{
public:
	void LoadSoundBuffer();
	void SetBuffer(int &volume);
	void PlaySound(std::string sound);

private:
	sf::SoundBuffer selectBuffer;
	sf::Sound selectSound;
};


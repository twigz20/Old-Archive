#pragma once

#include <SFML\Audio.hpp>
#include <string>

//Menu + Settings

class MenuMusic
{
public:
	void LoadMusic(int &volume);
	void PlayMusic(std::string music);
	void StopMusic();
	void Pause(std::string which);
	void MenuVolume(int &volume);
	void IntroVolume(int &volume);
private:
	sf::Music menuTheme;
	sf::Music introTheme;
};


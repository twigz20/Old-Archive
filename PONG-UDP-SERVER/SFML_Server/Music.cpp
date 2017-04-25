#include "Music.h"

void MenuMusic::LoadMusic(int &volume) {
	menuTheme.openFromFile("Resources/Sounds/menusong.ogg");
	//introTheme.openFromFile("audio//introsong.ogg");
	menuTheme.setVolume(volume);
	//introTheme.setVolume(volume);
}

void MenuMusic::PlayMusic(std::string music) {
	if (music == "menu") {
		menuTheme.play();
		menuTheme.setLoop(true);
		menuTheme.setPlayingOffset(sf::seconds(8));
	}
	/*if (music == "intro") {
		introTheme.play();
		introTheme.setLoop(true);
	}*/
}

void MenuMusic::StopMusic()
{
	menuTheme.stop();
}

void MenuMusic::Pause(std::string which) {
	if (which == "menu")
		menuTheme.pause();

	/*if (which == "intro")
		introTheme.pause();*/
}

void MenuMusic::MenuVolume(int &volume) {
	menuTheme.setVolume(volume);
}

void MenuMusic::IntroVolume(int &volume) {
	introTheme.setVolume(volume);
}
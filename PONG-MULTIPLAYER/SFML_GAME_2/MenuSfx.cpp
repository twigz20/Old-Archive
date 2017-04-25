#include "MenuSfx.h"


void MenuSound::LoadSoundBuffer()
{
	if (!selectBuffer.loadFromFile("Resources/Sounds/Select.ogg")) {
		perror("could not load Select Sound\n");
	}
}

void MenuSound::SetBuffer(int& volume)
{
	selectSound.setBuffer(selectBuffer);
	selectSound.setVolume(volume);
}

void MenuSound::PlaySound(std::string sound)
{
	if (sound == "select")
		selectSound.play();
}
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <iostream>

// Static background
class Background
{
public:
	Background();
	void setFilePath(std::string filepath);
	void Render(sf::RenderWindow& window);
private:
	sf::Texture bgTex;
	sf::Sprite bgSprite;
};

// Text to show on screen
class Text
{
public:
	Text(std::string fnt = "");
	void setFont(std::string fnt);
	void setSize(int size);
	void setStringAndSize(std::string outpur, int size);
	void setColor(sf::Color color);
	void setPosition(float x, float y);
	void setOrigin(float x, float y);
	void Update(std::stringstream& stream, int value);
	void Render(sf::RenderWindow& window);
	sf::FloatRect getGlobalBounds();
	std::string getText();
	void setText(std::string txt);
private:
	sf::Text text;
	sf::Font font;
};


#include "GUI.h"


// Background
Background::Background()
{
	bgTex.setSmooth(false);
	bgSprite.setPosition(0, 0);
}

void Background::setFilePath(std::string filepath)
{
	bgTex.loadFromFile("Resources/Images/"+ filepath);
	bgSprite.setTexture(bgTex);
}

void Background::Render(sf::RenderWindow& window)
{
	window.draw(bgSprite);
}


// Text
Text::Text(std::string fnt)
{
	if (!fnt.empty()) 
		font.loadFromFile("Resources/Fonts/" + fnt);
	else
		font.loadFromFile("Resources/Fonts/gunplay 3d.ttf");
		
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
}

void Text::setFont(std::string fnt)
{
	font.loadFromFile("Resources/Fonts/" + fnt);
	text.setFont(font);
}

void Text::setSize(int size)
{
	text.setCharacterSize(size);
}

void Text::setStringAndSize(std::string output, int size)
{
	text.setString(output);
	text.setCharacterSize(size);
}

void Text::setColor(sf::Color color)
{
	text.setColor(color);
}

void Text::setPosition(float x, float y)
{
	text.setPosition(x, y);
}

void Text::setOrigin(float x, float y)
{
	text.setOrigin(x, y);
}

void Text::Update(std::stringstream& stream, int value)
{
	text.setString(stream.str());
	stream.str("");
	stream << value;
}

void Text::Render(sf::RenderWindow& window)
{
	window.draw(text);
}

sf::FloatRect Text::getGlobalBounds()
{
	return text.getGlobalBounds();
}

std::string Text::getText()
{
	return text.getString();
}

void Text::setText(std::string txt)
{
	text.setString(txt);
}

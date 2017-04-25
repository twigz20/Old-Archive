#pragma once
#include <SFML/Graphics.hpp>

class Entity
{
public:
	Entity();
	virtual ~Entity();

	void LoadFromFile(std::string filename);

	virtual void Update(float elapsedTime);
	virtual void Draw(sf::RenderWindow& window);

	void SetPosition(float x, float y);
	void SetPositionX(float x);
	void SetPositionY(float y);
	float GetPositionX();
	float GetPositionY();
	float GetWidth();
	float GetHeight();

	sf::Vector2f GetPosition() const;
	bool IsLoaded() const;

	bool Collides(Entity* other);

protected:
	sf::Sprite sprite;

private:
	
	sf::Texture texture;
	std::string filename;
	bool isLoaded;
};


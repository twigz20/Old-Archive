#include "Entity.h"

Entity::Entity()
{
}


Entity::~Entity()
{
}

void Entity::LoadFromFile(std::string filename)
{
	if (!texture.loadFromFile("Resources/Images/" + filename))
	{
		filename = "";
		isLoaded = false;
	}
	else
	{
		this->filename = filename;
		sprite.setTexture(texture);
		isLoaded = true;
	}
}

void Entity::Draw(sf::RenderWindow& window)
{
	if (isLoaded)
		window.draw(sprite);
}

void Entity::Update(float elapsedTime)
{
}

void Entity::SetPosition(float x, float y)
{
	if (isLoaded)
		sprite.setPosition(x, y);
}

void Entity::SetPositionX(float x)
{
	if (isLoaded)
		sprite.setPosition(x, sprite.getPosition().y);
}

void Entity::SetPositionY(float y)
{
	if (isLoaded)
		sprite.setPosition(sprite.getPosition().x, y);
}

sf::Vector2f Entity::GetPosition() const
{
	if (isLoaded)
	{
		return sprite.getPosition();
	}
	return sf::Vector2f();
}

bool Entity::IsLoaded() const
{
	return isLoaded;
}

float Entity::GetPositionX()
{
	return sprite.getPosition().x;
}

float Entity::GetPositionY()
{
	return sprite.getPosition().y;
}

float Entity::GetWidth()
{
	return sprite.getTextureRect().width;
}

float Entity::GetHeight()
{
	return sprite.getTextureRect().height;
}

bool Entity::Collides(Entity* other) {

	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = GetPositionX();
	rightA = GetPositionX() + GetWidth();
	topA = GetPositionY();
	bottomA = GetPositionY() + GetHeight();

	//Calculate the sides of rect B
	leftB = other->GetPositionX();
	rightB = other->GetPositionX() + other->GetWidth();
	topB = other->GetPositionY();
	bottomB = other->GetPositionY() + other->GetHeight();

	//If no sides from A are outside of B
	if (((bottomA <= topB) || (topA >= bottomB) || (rightA <= leftB) || (leftA >= rightB)) == false)
	{
		//A collision is detected
		return true;
	}

	//If neither set of collision boxes touched
	return false;
}

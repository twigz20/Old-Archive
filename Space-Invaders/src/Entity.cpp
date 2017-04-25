#include "Entity.h"

Entity::Entity()
{
    posX = 0;
    posY = 0;
    width = 1;
    height = 1;
    mVelX = 0;
    mVelY = 0;
}

bool Entity::Collides(Entity* other) {

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = posX;
    rightA = posX + width;
    topA = posY;
    bottomA = posY + height;

    //Calculate the sides of rect B
    leftB = other->posX;
    rightB = other->posX + other->width;
    topB = other->posY;
    bottomB = other->posY + other->height;

    //If no sides from A are outside of B
    if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
    {
        //A collision is detected
        return true;
    }

    //If neither set of collision boxes touched
    return false;
}

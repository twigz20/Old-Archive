#ifndef ENTITY_H_
#define ENTITY_H_

#include "Game.h"
#include "Resource Manager.h"

class Entity {
    public:
        Entity();
        virtual ~Entity() {}
        virtual void handleEvents(SDL_Event* event) = 0;
        virtual void update(const float deltaTime) = 0;
        virtual void render() = 0;

        bool Collides(Entity* other);

        //Get Functions
        float getPosX() { return posX; }
        float getPosY() { return posY; }
        float getWidth() { return width; }
        float getHeight() { return height; }
        float getVelX() { return mVelX; }
        float getVelY() { return mVelY; }

        //Set Functions
        void setPosX(float x) { posX = x; }
        void setPosY(float y) { posY = y; }
        void setWidth(float w) { width = w; }
        void setHeight(float h) { height = h; }
        void setVelX(float velX) { mVelX = velX; }
        void setVelY(float velY) { mVelX = velY; }

        void setPositions(float x, float y) { posX = x; posY = y; }
        void setVelocity(float x, float y) { mVelX = x; mVelY = y; }

    protected:
        Game& game = Game::getInstance();
        ResourceManager& rm = ResourceManager::getInstance();

        float posX, posY, width, height;
        float mVelX, mVelY;
};

#endif

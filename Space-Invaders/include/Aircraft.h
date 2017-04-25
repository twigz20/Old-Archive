#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Entity.h"

class Aircraft : public Entity
{
    public:
        Aircraft();
        ~Aircraft();

        void handleEvents(SDL_Event* event);
        void update(const float deltaTime);
        void render();

    public:
        void moveLeft(const float deltaTime);
        void moveRight(const float deltaTime);
        void shoot(const float deltaTime);

        void setDead(bool dead) { this->dead = dead; }
        bool isDead() { return dead; }

        void setExplosionEnd(bool explosionEnd) { this->explosionEnd = explosionEnd; }
        bool getExplosionEnd() { return explosionEnd; }
        float getShotDelay() { return shotDelay; }

    private:
        std::shared_ptr<Texture> aircraft;
        std::shared_ptr<Texture> explosionTexture;

        //Explosion animation
        static const int EXPLOSION_FRAMES = 6;
        SDL_Rect explosion[ EXPLOSION_FRAMES ];

        int explosionFrame;
        bool explosionEnd;
        void setClips();
        bool dead;

        float speed;
        float shotDelay;
};

#endif // AIRCRAFT_H

#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Entity.h"
#include "Operations.h"
#include "picojson.h"

class Aircraft : public Entity
{
    public:
        Aircraft();
        ~Aircraft();

        void update(float deltaTime);
        void render();

    public:
        void moveLeft(float deltaTime);
        void moveRight(float deltaTime);
        void shoot(float deltaTime);

        void setDead(bool dead) { this->dead = dead; }
        bool isDead() { return dead; }

        void setExplosionEnd(bool explosionEnd) { this->explosionEnd = explosionEnd; }
        int getExplosionEnd() { return explosionEnd; }
        float getShotDelay() { return shotDelay; }

    private:
        Texture* aircraft;
        Texture* explosionTexture;

        //Explosion animation
        static const int EXPLOSION_FRAMES = 9;
        SDL_Rect explosion[ EXPLOSION_FRAMES ];

        int explosionFrame;
        bool explosionEnd;
        void setClips();
        bool dead;

        float speed;
        float shotDelay;
};

#endif // AIRCRAFT_H

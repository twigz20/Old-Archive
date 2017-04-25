#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Entity.h"

class Projectile : public Entity
{
    public:
        Projectile();
        ~Projectile();

        virtual void handleEvents(SDL_Event* event) = 0;
        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;
    public:

        virtual void setDead(bool dead) { this->dead = dead; }
        virtual bool isDead() { return dead; }

    protected:
        float speed;
        int damage;

        bool dead;
};

#endif // PROJECTILE_H

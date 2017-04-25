#ifndef ENEMY_ONE_BULLET_H
#define ENEMY_ONE_BULLET_H

#include "Projectile.h"

class Enemy_One_Bullet : public Projectile
{
    public:
        Enemy_One_Bullet();
        ~Enemy_One_Bullet();

        void handleEvents(SDL_Event* event);
        void update(const float deltaTime);
        void render();

        int getBulletDamage() { return damage; }

    private:
        std::shared_ptr<Texture> bullet;
};

#endif // ENEMY_ONE_BULLET_H

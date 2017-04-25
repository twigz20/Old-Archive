#ifndef BULLET_H
#define BULLET_H

#include "Projectile.h"

enum BulletType
{
	PLAYER = 1,
	ENEMY = 2,
};

std::string bulletType(BulletType TYPE);
BulletType bulletType(std::string type);

class Bullet : public Projectile
{
    public:
        Bullet(BulletType type);
        ~Bullet();

        BulletType getType() { return type_; }

        void handleEvents(SDL_Event* event);
        void update(const float deltaTime);
        void render();

        int getBulletDamage() { return damage; }
        std::string name() const;

    private:
        std::shared_ptr<Texture> bullet;
        std::string name_;
        std::string image_;
        BulletType type_;
};

#endif // BULLET_H

#ifndef CREEPINFO_H
#define CREEPINFO_H

#include <string>
#include <vector>
#include <SDL.h>
#include "Entity.h"
#include "Random.h"

enum CreepColor
{
	BLUE = 1,
	PURPLE = 2,
	RED = 3,
};

enum CreepLevel
{
	ONE = 1,
	TWO = 2,
	THREE = 3
};

std::string creepColor(CreepColor color);
CreepColor creepColor(std::string color);
std::string creepLevel(CreepLevel level);
CreepLevel creepLevel(std::string level);

class CreepInfo : public Entity
{
public:
	CreepInfo(CreepColor color, CreepLevel level);
	virtual ~CreepInfo();

	CreepColor color_;
	CreepLevel level_;

	double getHP() { return health_; }
	double getSpeed() { return speed_; }
	void setDead(bool dead) { this->dead = dead; }
    bool isDead() { return dead; }

	void animate();
	void explosion();
	void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveDown(float deltaTime);
    void shoot();
    int getShotTime() { return Random::Int(1,shotUpperBound); }
    void setUpperBound(int upper) { shotUpperBound = upper; }
    void handleEvents(SDL_Event* event);
	void update(float deltaTime);
    void render();
    void setEnemyHP(int hp) { health_ = hp; };
    int getEnemyHP() { return health_; }
    int getGold() { return gold_; }

    void setExplosionEnd(bool explosionEnd) { this->explosionEnd = explosionEnd; }
    bool getExplosionEnd() { return explosionEnd; }

private:
    std::shared_ptr<Texture> sheet;

    float mVelX, mVelY;
    double health_;
	int speed_;
	int gold_;

    //Walking animation
    static const int ENEMY_FRAMES = 3;
    SDL_Rect enemy[ ENEMY_FRAMES ];

    static const int ENEMY_EXPLOSION_FRAMES = 6;
    SDL_Rect enemyExplosion[ ENEMY_EXPLOSION_FRAMES ];

    //Current animation frame
    int frame;
    bool dead;
    int explosionFrame;
    bool explosionEnd;

    void setClips(CreepColor color);
    int shotUpperBound;
};

#endif // CREEPINFO_H

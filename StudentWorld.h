#ifndef _STUDENTWORLD_H_
#define _STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>

class Actor;
class Player;
class Alien;
class Projectile;

class StudentWorld : public GameWorld
{
public:
    StudentWorld();
    ~StudentWorld();

    virtual void init();
    virtual int move();
    virtual void cleanUp();

      // Add an actor to the space field
    void addGoodie(int imageID, int startX, int startY);

	void addProjectile(int startX, int startY, bool playerFired, int damagePoints);

      // Get the player's coordinates
    void getPlayerLocation(int& x, int& y);

      // Get the round number
    int getRound() const;

      // Get the number of alien-fired projectiles in the space field
    int getNumAlienFiredProjectiles() const;

      // Get a list of aliens at the same location as the actor
    std::list<Alien*> getCollidingAliens(Actor* a);

      // Get a pointer to the player if at the same location as the actor,
      // otherwise NULL
    Player* getCollidingPlayer(Actor* a);

	bool shotDuringPreviousTick(); 

	void setDisplayText();

	std::string stringStreams(int s, int r, double e, int t, int sL);

	void incAliensKilled();

private:
	std::list<Actor*> actor;
	std::list<Player*> player;
	int m_round;
	int m_alienFiredProjectiles;
	int m_aliensKilled;
};

#endif // _STUDENTWORLD_H_

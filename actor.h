#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Player;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY);

	virtual ~Actor();

      // Action to perform each tick
    virtual void doSomething() = 0;

      // Is this actor dead?
    bool isDead() const;

      // Mark this actor as dead
    void setDead();

      // Get the actor's world
    StudentWorld* getWorld();

private:
	StudentWorld* sp;
};

class Star : public Actor
{
public:
    Star(StudentWorld* world);
	virtual ~Star();
    virtual void doSomething();
};

class Projectile : public Actor
{
public:
    Projectile(StudentWorld* world, int imageID, int startX, int startY, bool playerFired, int damagePoints);
    virtual void doSomething();

      // Was this projectile fired by the player (as opposed to an alien)?
    bool playerFired() const;

private:
	bool m_playerFired;
	int m_damagePoints;
};

class Bullet : public Projectile
{
public:
    Bullet(StudentWorld* world, int startX, int startY, bool playerFired);
};

class Torpedo : public Projectile
{
public:
    Torpedo(StudentWorld* world, int startX, int startY, bool playerFired);
};

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* world, int imageID, int startX, int startY);
    virtual void doSomething();

      // Perform the action specific to the kind of goodie when the
      // player retrieves it
    virtual void doSpecialAction(Player* p) = 0;

private:
	int goodieTickLifetime;
	double ticksLeftToLive;
	unsigned int counter;
};

class FreeShipGoodie : public Goodie
{
public:
    FreeShipGoodie(StudentWorld* world, int startX, int startY);
    virtual void doSpecialAction(Player* p);
};

class EnergyGoodie : public Goodie
{
public:
    EnergyGoodie(StudentWorld* world, int startX, int startY);
    virtual void doSpecialAction(Player* p);
};

class TorpedoGoodie : public Goodie
{
public:
    TorpedoGoodie(StudentWorld* world, int startX, int startY);
    virtual void doSpecialAction(Player* p);
};

class Ship : public Actor
{
public:
    enum ProjectileType { BULLET, TORPEDO };

    Ship(StudentWorld* world, int imageID, int startX, int startY, int startEnergy);
	virtual ~Ship();

	virtual void damage(int points, bool hitByProjectile) = 0;

      // How much life energy does the ship have?
    int getEnergy() const;

      // What percentage of the starting life energy does the ship have?
    double getEnergyPct() const;

      // Decrease the life energy by the number of points
    void decreaseEnergy(int points);

      // Restore the life energy level to the starting level
    void restoreFullEnergy();

private:
	int m_energy;
};

class Player : public Ship
{
public:
    Player(StudentWorld* world);
	virtual ~Player();
    virtual void doSomething();

      // Cause the indicated number of points damage to the player, who was
      // either hit by a projectile or collided with an alien
    virtual void damage(int points, bool hitByProjectile);

      // How many torpedoes does this player have?
    int getNumTorpedoes() const;

      // Add n torpedoes to the players arsenal.
    void addTorpedoes(int n);

private:
	int m_torpedoes;
	
};

class Alien : public Ship
{
public:
    Alien(StudentWorld* world, int imageID, int startEnergy);

      // Cause the indicated number of points damage to the alien, who was
      // either hit by a projectile or collided with the player
    virtual void damage(int points, bool hitByProjectile);

      // Possibly drop a goodie
    virtual void maybeDropGoodie();
};

class NachlingBase : public Alien
{
public:
    NachlingBase(StudentWorld* world, int imageID, int startEnergy);
    virtual void doSomething();
private:
	unsigned int m_counter;
	int m_state;
	int MDB;
	int HMD;
	int HMR;
	int leftOrRight;
};

class Nachling : public NachlingBase
{
public:
    Nachling(StudentWorld* world, int round);
};

class WealthyNachling : public NachlingBase
{
public:
    WealthyNachling(StudentWorld* world, int round);
    virtual void doSomething();
    virtual void maybeDropGoodie();
	
private:
	bool m_malfunctioning;
	int m_count;
};

class Smallbot : public Alien
{
public:
    Smallbot(StudentWorld* world, int round);
    virtual void doSomething();
    virtual void damage(int points, bool hitByProjectile);
    virtual void maybeDropGoodie();
	virtual bool hit();

private:
	unsigned int m_alternate;
	bool attacked;
};

#endif // _ACTOR_H_
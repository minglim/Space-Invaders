#include "actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <list>

using namespace std;

// Students:  Add code to this file (if you wish), actor.h, StudentWorld.h, and StudentWorld.cpp

///////////
// Actor //
///////////

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY)
	: GraphObject(imageID, startX, startY), sp(world)
{}

Actor::~Actor()
{}

bool Actor::isDead() const
{
	if (getY() >= 0 && getY() <= 39)
		return false;
	return true;
}

void Actor::setDead()
{
	moveTo(getX(), getY()-99);
}

StudentWorld* Actor::getWorld()
{
	return sp;
}

//////////
// Star //
//////////

Star::Star(StudentWorld* world)
	: Actor(world, IID_STAR, rand() % VIEW_WIDTH, VIEW_HEIGHT-1)
{}

Star::~Star()
{}

void Star::doSomething()
{
	moveTo(getX(), getY()-1);
}

////////////////
// Projectile //
////////////////

Projectile::Projectile(StudentWorld* world, int imageID, int startX, int startY, bool playerFired, int damagePoints)
	: Actor(world, imageID, startX, startY), m_playerFired(playerFired), m_damagePoints(damagePoints)
{}

void Projectile::doSomething()
{
	if (playerFired())
	{
		list<Alien*> temp = getWorld()->getCollidingAliens(this);
		if (temp.empty())
		{
			moveTo(getX(), getY()+1);
			if (!isDead())
			{
				list<Alien*> temp1 = getWorld()->getCollidingAliens(this);
				if (!temp1.empty())
				{
					for (list<Alien*>::iterator a = temp1.begin(); a != temp1.end(); a++)
						(*a)->damage(m_damagePoints, true);
					setDead();
				}
			}
		}
		else
		{
			for (list<Alien*>::iterator b = temp.begin(); b != temp.end(); b++)
				(*b)->damage(m_damagePoints, true);
			setDead();
		}
	}
	else
	{
		Player* temp = getWorld()->getCollidingPlayer(this);
		if (temp == NULL)
		{
			moveTo(getX(), getY()-1);
			if (!isDead())
			{
				Player* temp1 = getWorld()->getCollidingPlayer(this);
				if (temp != NULL)
				{
					temp1->damage(m_damagePoints, true);
					setDead();
				}
			}
		}
		else
		{
			temp->damage(m_damagePoints, true);
			setDead();
		}
	}
}

bool Projectile::playerFired() const
{
	if (m_playerFired)
		return true;
	return false;
}

////////////
// Bullet //
////////////

Bullet::Bullet(StudentWorld* world, int startX, int startY, bool playerFired)
	: Projectile(world, IID_BULLET, startX, startY, playerFired, 2)
{}

/////////////
// Torpedo //
/////////////

Torpedo::Torpedo(StudentWorld* world, int startX, int startY, bool playerFired)
	: Projectile(world, IID_TORPEDO, startX, startY, playerFired, 8)
{}

////////////
// Goodie //
////////////

Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY)
	: Actor(world, imageID, startX, startY), goodieTickLifetime((100.0/getWorld()->getRound())+30), counter(0)
{
	ticksLeftToLive = goodieTickLifetime;
}

void Goodie::doSomething()
{
	Player* temp = getWorld()->getCollidingPlayer(this);
	if (temp == NULL)
	{
		double brightness = ((ticksLeftToLive/goodieTickLifetime)+0.2);
		setBrightness(brightness);
		if (ticksLeftToLive <= 0)
		{
			setDead();
			return;
		}
		if (counter != 0 && counter % 3 == 0)
		{
			moveTo(getX(), getY()-1);
			Player* temp1 = getWorld()->getCollidingPlayer(this);
			if (temp1 != NULL)
			{
				doSpecialAction(temp1);
			}
		}
	}
	else
	{
		doSpecialAction(temp);
	}
	counter++;
	ticksLeftToLive--;
}

////////////////////
// FreeShipGoodie //
////////////////////

FreeShipGoodie::FreeShipGoodie(StudentWorld* world, int startX, int startY)
	: Goodie(world, IID_FREE_SHIP_GOODIE, startX, startY)
{}

void FreeShipGoodie::doSpecialAction(Player* p)
{
	getWorld()->increaseScore(5000);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	setDead();
	getWorld()->incLives();
}

//////////////////
// EnergyGoodie //
//////////////////

EnergyGoodie::EnergyGoodie(StudentWorld* world, int startX, int startY)
	: Goodie(world, IID_ENERGY_GOODIE, startX, startY)
{}

void EnergyGoodie::doSpecialAction(Player* p)
{
	getWorld()->increaseScore(5000);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	setDead();
	p->restoreFullEnergy();
}

///////////////////
// TorpedoGoodie //
///////////////////

TorpedoGoodie::TorpedoGoodie(StudentWorld* world, int startX, int startY)
	: Goodie(world, IID_TORPEDO_GOODIE, startX, startY)
{}

void TorpedoGoodie::doSpecialAction(Player* p)
{
	getWorld()->increaseScore(5000);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	setDead();
	p->addTorpedoes(5);
}

//////////
// Ship //
//////////

Ship::Ship(StudentWorld* world, int imageID, int startX, int startY, int startEnergy)
	: Actor(world, imageID, startX, startY), m_energy(startEnergy)
{}

Ship::~Ship()
{}

int Ship::getEnergy() const
{
	return m_energy;
}

double Ship::getEnergyPct() const
{
	double percent = (m_energy/50.0)*100;
	return percent;
}

void Ship::decreaseEnergy(int points) 
{
	m_energy -= points;
}

void Ship::restoreFullEnergy()
{
	m_energy = 50;
}

////////////
// Player //
////////////

Player::Player(StudentWorld* world)
	: Ship(world, IID_PLAYER_SHIP, VIEW_WIDTH/2, 1, 50), m_torpedoes(0)
{}

Player::~Player()
{}

void Player::doSomething()
{
	if (getEnergy() <= 0)
		return;
	list<Alien*> temp = getWorld()->getCollidingAliens(this);
	if (!temp.empty())
	{
		for (list<Alien*>::iterator b = temp.begin(); b != temp.end(); b++)
		{
			damage(15, false);
			(*b)->setDead();
			if (getEnergy() <= 0)
				return;
		}
	}
	else
	{
	int key;
	if (getWorld()->getKey(key))
	{
		switch(key)
		{
		case KEY_PRESS_LEFT:
			{
				if (getX() > 0)
					moveTo(getX()-1, getY());
				list<Alien*> temp = getWorld()->getCollidingAliens(this);
				if (!temp.empty())
				{
					for (list<Alien*>::iterator b = temp.begin(); b != temp.end(); b++)
					{
						damage(15, false);
						(*b)->setDead();
						if (getEnergy() <= 0)
							return;
					}
				}
				break;
			}
		case KEY_PRESS_RIGHT:
			{
				if (getX() < VIEW_WIDTH-1)
					moveTo(getX()+1, getY());
				list<Alien*> temp = getWorld()->getCollidingAliens(this);
				if (!temp.empty())
				{
					for (list<Alien*>::iterator b = temp.begin(); b != temp.end(); b++)
					{
						damage(15, false);
						(*b)->setDead();
						if (getEnergy() <= 0)
							return;
					}
				}
				break;
			}
		case KEY_PRESS_UP:
			{
				if (getY() < VIEW_HEIGHT-1)
					moveTo(getX(), getY()+1);
				list<Alien*> temp = getWorld()->getCollidingAliens(this);
				if (!temp.empty())
				{
					for (list<Alien*>::iterator b = temp.begin(); b != temp.end(); b++)
					{
						damage(15, false);
						(*b)->setDead();
						if (getEnergy() <= 0)
							return;
					}
				}
				break;
			}
		case KEY_PRESS_DOWN:
			{
				if (getY() > 0)
					moveTo(getX(), getY()-1);
				list<Alien*> temp = getWorld()->getCollidingAliens(this);
				if (!temp.empty())
				{
					for (list<Alien*>::iterator b = temp.begin(); b != temp.end(); b++)
					{
						damage(15, false);
						(*b)->setDead();
						if (getEnergy() <= 0)
							return;
					}
				}
				break;
			}
		case KEY_PRESS_SPACE:
			{
				if (!getWorld()->shotDuringPreviousTick())
				{
					if (getY() != 39)
					{
						getWorld()->addProjectile(getX(), getY()+1, true, 2);
						getWorld()->playSound(SOUND_PLAYER_FIRE);
					}
				}
				break;
			}
		case KEY_PRESS_TAB:
			{
				if (!getWorld()->shotDuringPreviousTick())
				{
					if (getY() != 39 && getNumTorpedoes() >= 1)
					{
						getWorld()->addProjectile(getX(), getY()+1, true, 8);
						getWorld()->playSound(SOUND_PLAYER_TORPEDO);
						m_torpedoes--;
					}
				}
				break;
			}
		}
	}
	}
}

void Player::damage(int points, bool hitByProjectile)
{
	decreaseEnergy(points);
	if (hitByProjectile)
		getWorld()->playSound(SOUND_PLAYER_HIT);
	else
		getWorld()->playSound(SOUND_ENEMY_PLAYER_COLLISION);
	if (getEnergy() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
}

int Player::getNumTorpedoes() const
{
	return m_torpedoes;
}

void Player::addTorpedoes(int n)
{
	m_torpedoes += n;
}

///////////
// Alien //
///////////

Alien::Alien(StudentWorld* world, int imageID, int startEnergy)
	: Ship(world, imageID, rand() % VIEW_WIDTH, VIEW_HEIGHT-1, startEnergy)
{}

void Alien::damage(int points, bool hitByProjectile)
{
	if (!hitByProjectile)
	{
		setDead();
		getWorld()->playSound(SOUND_ENEMY_DIE);
	}
	else
	{
		decreaseEnergy(points);
		if (getEnergy() <= 0)
		{
			maybeDropGoodie();
		}
		else
			getWorld()->playSound(SOUND_ENEMY_HIT);
	}
}

void Alien::maybeDropGoodie()
{
	setDead();
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->increaseScore(1000);
	getWorld()->incAliensKilled();
}

//////////////////
// NachlingBase //
//////////////////

NachlingBase::NachlingBase(StudentWorld* world, int imageID, int startEnergy)
	: Alien(world, imageID, startEnergy), m_counter(0), m_state(0)
{}

void NachlingBase::doSomething()
{
	if (m_counter % 2 != 0)
	{
		m_counter++;
		return;
	}
	if (m_state == 0)
	{
		int xCoord;
		int yCoord;
		getWorld()->getPlayerLocation(xCoord, yCoord);
		if (getX() != xCoord)
		{
			int random = rand() % 3;
			if (random < 2)
			{
				moveTo(getX(), getY()-1);
				m_counter++;
				return;
			}
			else
			{
				if (getX() < xCoord)
				{
					moveTo(getX()+1, getY()-1);
					m_counter++;
					return;
				}
				else
				{
					moveTo(getX()-1, getY()-1);
					m_counter++;
					return;
				}
			}
		}
		else
		{
			if (getX() != 0 && getX() != 29)
			{
				m_state++;
				int leftDifference = getX() - 0;
				int rightDifference = 29 - getX();
				if (leftDifference < rightDifference)
					MDB = leftDifference;
				else
					MDB = rightDifference;
				if (MDB > 3)
				{
					int random = rand() % 3 + 1;
					HMD = random;
				}
				else
					HMD = MDB;
				int direction = rand() % 1;
				if (direction == 0)
					leftOrRight = 0;
				else
					leftOrRight = 1;
				HMR = HMD;
				moveTo(getX(), getY()-1);
				m_counter++;
				return;
			}
		}
	}
	if (m_state == 1)
	{
		int xCoord;
		int yCoord;
		getWorld()->getPlayerLocation(xCoord, yCoord);
		if (yCoord > getY())
		{
			m_state++;
			m_counter++;
			return;
		}
		if (HMR == 0)
		{
			if (leftOrRight == 0)
				leftOrRight = 1;
			else
				leftOrRight = 0;
			HMR = 2*HMD;
			if (leftOrRight == 0)
				moveTo(getX()-1, getY());
			else
				moveTo(getX()+1, getY());
		}
		else
		{
			HMR--;
			if (leftOrRight == 0)
				moveTo(getX()-1, getY());
			else
				moveTo(getX()+1, getY());
		}
		int chancesOfFiring = (10/getWorld()->getRound()) + 1;
		int randomCount = rand() % chancesOfFiring;
		if (randomCount == 0)
		{
			int count = getWorld()->getNumAlienFiredProjectiles();
			if (count < 2*getWorld()->getRound())
			{
				getWorld()->addProjectile(getX(), getY()-1, false, 2);
				getWorld()->playSound(SOUND_ENEMY_FIRE);
			}
		}
		int chance = rand() % 20;
		if (chance == 0)
			m_state++;
		m_counter++;
		return;
	}
	if (m_state == 2)
	{
		if (getY() == 39)
		{
			m_state = 0;
			m_counter++;
			return;
		}
		if (getX() == 0)
		{
			leftOrRight = 1;
			moveTo(getX()+1, getY()+1);
			m_counter++;
			return;
		}
		if (getX() == 29)
		{
			leftOrRight = 0;
			moveTo(getX()-1, getY()+1);
			m_counter++;
			return;
		}
		int direction = rand() % 1;
		if (direction == 0)
			leftOrRight = 0;
		else
			leftOrRight = 1;
		if (leftOrRight == 0)
			moveTo(getX()-1, getY()+1);
		else
			moveTo(getX()+1, getY()+1);
		m_counter++;
	}
}

//////////////
// Nachling //
//////////////

Nachling::Nachling(StudentWorld* world, int round)
	: NachlingBase(world, IID_NACHLING, 5*((0.1*round)+0.9))
{}

/////////////////////
// WealthyNachling //
/////////////////////

WealthyNachling::WealthyNachling(StudentWorld* world, int round)
	: NachlingBase(world, IID_WEALTHY_NACHLING, 8*((0.1*round)+0.9)), m_malfunctioning(false), m_count(0)
{}

void WealthyNachling::doSomething()
{
	if(m_malfunctioning)
	{
		if (m_count <= 30)
		{
			m_count++;
			return;
		}
		else
		{
			m_malfunctioning = false;
			m_count = 0;
		}
	}
	int random = rand() % 200;
	if (random == 0)
	{
		m_malfunctioning = true;
		return;
	}
	NachlingBase::doSomething();
}

void WealthyNachling::maybeDropGoodie()
{
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->increaseScore(1200);
	getWorld()->incAliensKilled();
	int random = rand() % 3;
	if (random == 0)
	{
		int whichGoodie = rand() % 2;
		if (whichGoodie == 0)
			getWorld()->addGoodie(IID_ENERGY_GOODIE, getX(), getY());
		else
			getWorld()->addGoodie(IID_TORPEDO_GOODIE, getX(), getY());
	}
	setDead();
}

//////////////
// Smallbot //
//////////////

Smallbot::Smallbot(StudentWorld* world, int round)
	: Alien(world, IID_SMALLBOT, 12*((0.1*round)+0.9)), m_alternate(0), attacked(false)
{}

void Smallbot::doSomething()
{
	if (m_alternate % 2 != 0)
	{
		m_alternate++;
		return;
	}
	int random = rand() % 3;
	if (attacked && random == 0)
	{
		if (getX() == 0)
			moveTo(getX()+1, getY()-1);
		else if (getX() == 29)
			moveTo(getX()-1, getY()-1);
		else if (getX() != 0 && getX() != 29)
		{
			int direction = rand() % 2;
			if (direction == 0)
				moveTo(getX()-1, getY()-1);
			else
				moveTo(getX()+1, getY()-1);
		}
	}
	else
		moveTo(getX(), getY()-1);
	int xCoord;
	int yCoord;
	getWorld()->getPlayerLocation(xCoord, yCoord);
	if (getX() == xCoord && getY() > yCoord)
	{
		int q = (100/getWorld()->getRound()) + 1;
		int bOrT = rand() % q;
		if (bOrT == 0)
		{
			if (getWorld()->getNumAlienFiredProjectiles() < 2*getWorld()->getRound())
			{
				getWorld()->addProjectile(getX(), getY()-1, false, 8);
				getWorld()->playSound(SOUND_ENEMY_FIRE);
			}
		}
		else
		{
			if (getWorld()->getNumAlienFiredProjectiles() < 2*getWorld()->getRound())
			{
				getWorld()->addProjectile(getX(), getY()-1, false, 2);
				getWorld()->playSound(SOUND_ENEMY_FIRE);
			}
		}
	}
	m_alternate++;
	attacked = false;
}

void Smallbot::damage(int points, bool hitByProjectile)
{
	Alien::damage(points, hitByProjectile);
	if (hitByProjectile)
		attacked = true;
}

void Smallbot::maybeDropGoodie()
{
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->increaseScore(1500);
	getWorld()->incAliensKilled();
	int random = rand() % 3;
	if (random == 0)
		getWorld()->addGoodie(IID_FREE_SHIP_GOODIE, getX(), getY());
	setDead();
}

bool Smallbot::hit()
{
	return attacked;
}
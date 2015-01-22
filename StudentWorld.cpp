#include "StudentWorld.h"
#include "actor.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld()
{
    return new StudentWorld();
}

// Students:  Add code to this file (if you wish), StudentWorld.h, actor.h and actor.cpp
StudentWorld::StudentWorld()
	: m_round(1)
{}

StudentWorld::~StudentWorld()
{
	list<Actor*>::iterator a = actor.begin();
	while (a != actor.end())
	{
		if (actor.empty())
			break;
		delete *a;
		a++;
		actor.pop_front();
	}
	list<Player*>::iterator p = player.begin();
	while (p != player.end())
	{
		if (player.empty())
			break;
		delete *p;
		p++;
		player.pop_front();
	}
}


void StudentWorld::init()
{
	Player* p = new Player(this);
	player.push_back(p);
	p->setVisible(true);
	m_aliensKilled = 0;
	m_alienFiredProjectiles = 0;
}

int StudentWorld::move()
{
	if (testParamsProvided())
	{
		if (!actor.empty())
		{}
		else
		{
			int test = getTestParam(TEST_PARAM_ACTOR_INDEX);
			if (test == TEST_PARAM_NACHLING)
			{
				Nachling* n = new Nachling(this, m_round);
				actor.push_back(n);
				n->setVisible(true);
			}
			if (test == TEST_PARAM_WEALTHY_NACHLING)
			{
				WealthyNachling* w = new WealthyNachling(this, m_round);
				actor.push_back(w);
				w->setVisible(true);
			}
			if (test == TEST_PARAM_SMALLBOT)
			{
				Smallbot* s = new Smallbot(this, m_round);
				actor.push_back(s);
				s->setVisible(true);
			}
			if (test == TEST_PARAM_GOODIE_ENERGY)
				addGoodie(IID_ENERGY_GOODIE, 15, 39);
			if (test == TEST_PARAM_GOODIE_TORPEDO)
				addGoodie(IID_TORPEDO_GOODIE, 15, 39);
			if (test == TEST_PARAM_GOODIE_FREE_SHIP)
				addGoodie(IID_FREE_SHIP_GOODIE, 15, 39);
		}
	}
	else
	{
	int numberOfActiveAliens = 0;
	for (list<Actor*>::iterator x = actor.begin(); x != actor.end(); x++)
	{
		Alien* n = dynamic_cast<Alien*>(*x);
		if (n != NULL)
			numberOfActiveAliens++;
	}
	int s = (0.5*m_round) + 2;
	int n = 4*m_round;
	if (numberOfActiveAliens >= s)
	{}
	else if (numberOfActiveAliens >= n)
	{}
	else
	{
		int whichAlien = rand() % 100;
		if (whichAlien < 69)
		{
			int whichNachling = rand() % 100;
			if (whichNachling < 19)
			{
				WealthyNachling* w = new WealthyNachling(this, m_round);
				actor.push_back(w);
				w->setVisible(true);
			}
			else
			{
				Nachling* n = new Nachling(this, m_round);
				actor.push_back(n);
				n->setVisible(true);
			}
		}
		else
		{
			Smallbot* s = new Smallbot(this, m_round);
			actor.push_back(s);
			s->setVisible(true);
		}
	}
	int random = rand() % 100;
	if (random < 33)
	{
		Star* s = new Star(this);
		actor.push_back(s);
		s->setVisible(true);
	}
	}

	setDisplayText();

	for (list<Actor*>::iterator a = actor.begin(); a != actor.end(); a++)
	{
		if (!(*a)->isDead())
			(*a)->doSomething();
	}
	for (list<Player*>::iterator p = player.begin(); p != player.end(); p++)
	{
		if (!(*p)->isDead())
			(*p)->doSomething();
	}

	list<Actor*>::iterator a = actor.begin();
	while (a != actor.end())
	{
		if ((*a)->isDead())
		{
			Projectile* n = dynamic_cast<Projectile*>(*a);
			if (n != NULL && n->getY() < 0)
				m_alienFiredProjectiles--;
			delete *a;
			list<Actor*>::iterator b = a;
			if (a == actor.begin())
			{
				actor.erase(b);
				a = actor.begin();
				continue;
			}
			else
				a--;
			actor.erase(b);
		}
		a++;
	}

	if (m_aliensKilled == 4*m_round)
	{
		m_round++;
		m_aliensKilled = 0;
	}

	list<Player*>::iterator p = player.begin();
	if ((*p)->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	list<Actor*>::iterator a = actor.begin();
	while (a != actor.end())
	{
		if (actor.empty())
			break;
		delete *a;
		a++;
		actor.pop_front();
	}
	list<Player*>::iterator p = player.begin();
	while (p != player.end())
	{
		if (player.empty())
			break;
		delete *p;
		p++;
		player.pop_front();
	}
}

void StudentWorld::addGoodie(int imageID, int startX, int startY)
{
	if (imageID == IID_ENERGY_GOODIE)
	{
		EnergyGoodie* e = new EnergyGoodie(this, startX, startY);
		actor.push_back(e);
		e->setVisible(true);
	}
	if (imageID == IID_FREE_SHIP_GOODIE)
	{
		FreeShipGoodie* f = new FreeShipGoodie(this, startX, startY);
		actor.push_back(f);
		f->setVisible(true);
	}
	if (imageID == IID_TORPEDO_GOODIE)
	{
		TorpedoGoodie* t = new TorpedoGoodie(this, startX, startY);
		actor.push_back(t);
		t->setVisible(true);
	}
}

void StudentWorld::addProjectile(int startX, int startY, bool playerFired, int damagePoints)
{

	if (damagePoints == 2)
	{
		Bullet* b = new Bullet(this, startX, startY, playerFired);
		actor.push_back(b);
		b->setVisible(true);
		if (!playerFired)
			m_alienFiredProjectiles++;
	}
	else
	{
		Torpedo* t = new Torpedo(this, startX, startY, playerFired);
		actor.push_back(t);
		t->setVisible(true);
		if (!playerFired)
			m_alienFiredProjectiles++;
	}
}

void StudentWorld::getPlayerLocation(int& x, int& y)
{
	list<Player*>::iterator p = player.begin();
	x = (*p)->getX();
	y = (*p)->getY();
}

int StudentWorld::getRound() const
{
	return m_round;
}

int StudentWorld::getNumAlienFiredProjectiles() const
{
	return m_alienFiredProjectiles;
}

list<Alien*> StudentWorld::getCollidingAliens(Actor* a)
{
	list<Alien*> alien;
	for (list<Actor*>::iterator x = actor.begin(); x != actor.end(); x++)
	{
		if ((*x)->getX() == a->getX() && (*x)->getY() == a->getY())
		{
			Alien* n = dynamic_cast<Alien*>(*x);
			if (n != NULL)
				alien.push_back(n);
		}
	}
	return alien;
}

Player* StudentWorld::getCollidingPlayer(Actor* a)
{
	for (list<Player*>::iterator p = player.begin(); p != player.end(); p++)
	{
		if ((*p)->getX() == a->getX() && (*p)->getY() == a->getY())
			return *p;
	}
	return NULL;
}

bool StudentWorld::shotDuringPreviousTick()
{
	list<Player*>::iterator p = player.begin();
	for (list<Actor*>::iterator a = actor.begin(); a != actor.end(); a++)
	{
		if ((*a)->getX() == (*p)->getX() && (*a)->getY() == (*p)->getY()+2)
		{
			Projectile* e = dynamic_cast<Projectile*>(*a);
			if (e != NULL)
			{
				if (e->playerFired())
					return true;
			}
		}
	}
	return false;
}

string StudentWorld::stringStreams(int s, int r, double e, int t, int sL)
{
	ostringstream oss;
	oss << "Score: ";
	oss.fill('0');
	oss << setw(7) << s;
	oss << "  Round: ";
	oss.fill('0');
	oss << setw(2) << r;
	oss << "  Energy: " << e << "%";
	oss << "  Torpedoes: ";
	oss.fill('0');
	oss << setw(3) << t;
	oss << "  Ships: ";
	oss.fill('0');
	oss << setw(2) << sL;
	string text = oss.str();
	return text;
}

void StudentWorld::setDisplayText()
{
	list<Player*>::iterator p = player.begin();
	int score = getScore();
	int round = getRound();
	double energyPercent = (*p)->getEnergyPct();
	int torpedoes = (*p)->getNumTorpedoes();
	int shipsLeft = getLives();

	string s = stringStreams(score, round, energyPercent, torpedoes, shipsLeft);

	setGameStatText(s);
}

void StudentWorld::incAliensKilled()
{
	m_aliensKilled++;
}


#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* pp, int imageID, int startX, int startY, Direction dir);
	StudentWorld* getWorld() const{	return m_world; }
	virtual bool bePushed(Direction dir){ return false; } //return true if an actor can be pushed
	virtual void setDead(){ return; } //make an actor dead
	virtual bool isDead() const { return false; } //return the status of an actor
	virtual bool complete(){ return false; } //return true if the player has fulfilled all the requirement to get to the next level
	virtual void doSomething() = 0; //allows each actor to do what they should do during each tick
	virtual void beAttacked() = 0; //allows each actor to be attacked
	virtual ~Actor()
	{}
private:
	StudentWorld* m_world;
	int m_countMove;
};

class Wall: public Actor
{
public:
	Wall(StudentWorld* pp, int startX, int startY)
		:Actor(pp, IID_WALL, startX, startY, none)
	{
		setVisible(true);
	}
	virtual void doSomething(){ return; }
	virtual void beAttacked(){ return; }
	virtual ~Wall() {}
};

class Player : public Actor
{
public:
	Player(StudentWorld* pp, int startX, int startY);
	int getHealth() const {	return m_health; }
	int getAmmo() const { return m_ammo; }
	void gainHealth(); //gain the player・s health when he receives restore health goodie
	void gainAmmo(); //gain the player・s ammunition when he receives ammo goodie
	virtual bool isDead() const;
	virtual void doSomething();
	virtual void beAttacked();
	virtual ~Player() {}
private:
	int m_health;
	int m_ammo;
};

class Boulder :public Actor
{
public:
	Boulder(StudentWorld* pp, int startX, int startY)
		:Actor(pp, IID_BOULDER, startX, startY, none)
	{
		setVisible(true);
		m_health = 10;
	}
	virtual bool bePushed(Direction dir);
	virtual bool isDead() const;
	virtual void setDead();
	virtual void doSomething(){ return; }
	virtual void beAttacked();
	virtual ~Boulder() {}
private:
	int m_health;
};

class Bullet :public Actor
{
public:
	Bullet(StudentWorld* pp, int startX, int startY, Direction dir)
		:Actor(pp, IID_BULLET, startX, startY, dir)
	{
		setVisible(true);
		m_dead = false;
	}
	virtual bool isDead() const{ return m_dead; }
	virtual void doSomething();
	virtual void beAttacked(){ return; }
	virtual ~Bullet() {}
private:
	bool m_dead;
};

class Hole :public Actor
{
public:
	Hole(StudentWorld* pp, int startX, int startY)
		:Actor(pp, IID_HOLE, startX, startY, none)
	{
		setVisible(true);
		m_dead = false;
	}
	virtual bool isDead() const{ return m_dead; }
	virtual void doSomething();
	virtual void beAttacked(){ return; }
	virtual ~Hole() {}
private:
	bool m_dead;
};

class Exit : public Actor
{
public:
	Exit(StudentWorld* pp, int startX, int startY)
		:Actor(pp, IID_EXIT, startX, startY, none)
	{
		setVisible(false);
		m_complete = false;
	}
	virtual void doSomething();
	virtual void beAttacked(){ return; }
	virtual bool complete(){ return m_complete; }
	virtual ~Exit() {}
private:
	bool m_complete;
};

class Goodie : public Actor
{
public:
	Goodie(StudentWorld* pp, int imageID, int startX, int startY);
	virtual bool isDead() const{ return m_dead; }
	virtual void setDead();
	virtual void doSomething() = 0;
	virtual void beAttacked(){ return; }
	virtual ~Goodie() {}
private:
	bool m_dead;
};

class Jewel : public Actor
{
public:
	Jewel(StudentWorld* pp, int startX, int startY)
		:Actor(pp, IID_JEWEL, startX, startY, none)
	{
		setVisible(true);
		m_dead = false;
	}
	virtual bool isDead() const{ return m_dead; }
	virtual void doSomething();
	virtual void beAttacked() { return; }
	virtual ~Jewel() {}
private:
	bool m_dead;
};

class ExtraLife : public Goodie
{
public:
	ExtraLife(StudentWorld* pp, int startX, int startY)
		:Goodie(pp, IID_EXTRA_LIFE, startX, startY) {}
	virtual void doSomething();
	virtual ~ExtraLife() {}
};

class RestoreHealth : public Goodie
{
public:
	RestoreHealth(StudentWorld* pp, int startX, int startY)
		:Goodie(pp, IID_RESTORE_HEALTH, startX, startY) {}
	virtual void doSomething();
	virtual ~RestoreHealth() {}
};

class Ammo : public Goodie
{
public:
	Ammo(StudentWorld* pp, int startX, int startY)
		:Goodie(pp, IID_AMMO, startX, startY) {}
	virtual void doSomething();
	virtual ~Ammo() {}
};

class SnarlBot : public Actor
{
public:
	SnarlBot(StudentWorld* pp, int startX, int startY, Direction dir)
		:Actor(pp, IID_SNARLBOT, startX, startY, dir)
	{
		setVisible(true);
		m_health = 10;
		m_ticks = (28 - getWorld()->getLevel()) / 4; // levelNumber is the current
		// level number (0, 1, 2, etc.)
		if (m_ticks < 3)
			m_ticks = 3; //no SnarlBot moves more frequently than this
		m_restCount = m_ticks;
	}
	virtual bool isDead() const;
	virtual void doSomething();
	virtual void beAttacked();
	virtual ~SnarlBot() {}
private:
	int m_health;
	int m_restCount;
	int m_ticks;
};

class KleptoBot :public Actor
{
public:
	KleptoBot(StudentWorld* pp, int imageID, int startX, int startY)
		:Actor(pp, imageID, startX, startY, right)
	{
		m_dTurning = rand() % 6 + 1;
		setHealth(5);
		m_count = 0;
		setVisible(true);
		m_ticks = (28 - getWorld()->getLevel()) / 4; // levelNumber is the current
		// level number (0, 1, 2, etc.)
		if (m_ticks < 3)
			m_ticks = 3; //no SnarlBot moves more frequently than this
		m_restCount = m_ticks;
		m_store = nullptr;
 	}
	virtual bool isDead() const;
	virtual void doSomething();
	virtual void beAttacked();
	bool change(int dir, int n); // check which direction has no obstacle and change to that direction.
	bool move(int x, int y); //check if the robot is allowed to move to the next square in its direction.
	bool getGoodie(); //if the goodie is at the same square as the robot, try to get it.
	bool moveIfCan(); //check if the robot has reached the distance before turning. If not move the robot and return true.
	void setStore(Actor* pp); //if the robot gets a goodie, store the goodie in this actor container.
	void setHealth(int n); //set the robot・s health to n
	void decHealth(int n); //decrease the robot・s health by n
	Actor* store()const { return m_store; }
	int health()const { return m_health; }
	int ticks() const { return m_ticks; }
	virtual ~KleptoBot() {}
private:
	int m_health;
	int m_count;
	int m_dTurning;
	int m_ticks;
	int m_restCount;
	Actor* m_store;
};

class AngryKleptoBot :public KleptoBot
{
public:
	AngryKleptoBot(StudentWorld* pp, int startX, int startY)
		:KleptoBot(pp, IID_ANGRY_KLEPTOBOT, startX, startY)
	{
		m_dTurning = rand() % 6 + 1;
		setHealth(8);
		m_count = 0;
		m_restCount = ticks();
	}
	virtual void doSomething();
	virtual void beAttacked();
	virtual ~AngryKleptoBot() {}
private:
	int m_count;
	int m_dTurning;
	int m_restCount;
};

class RobotFactory :public Actor
{
public:
	RobotFactory(StudentWorld* pp, int startX, int startY, int type)
		:Actor(pp, IID_ROBOT_FACTORY, startX, startY, none)
	{
		setVisible(true);
		m_type = type;
	}
	virtual void doSomething();
	virtual void beAttacked(){ return; }
	virtual ~RobotFactory() {}
private:
	int m_type;
};

#endif // ACTOR_H_

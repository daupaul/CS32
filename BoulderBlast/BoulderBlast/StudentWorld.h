#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Player;
class Actor;
class Wall;
class Bullet;
class Hole;
class Jewel;
class Exit;
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
		m_number = 0;
		m_bonus = 1000;
	}

	virtual int init();
	virtual int move();
	virtual void cleanUp();
	
	int numJewel() const;
	void eraseActor(int number);
	void clearActor();
	Actor* objectAt(int x, int y) const;
	Actor* overlap(int x, int y) const;
	Actor* getExit() const;
	Actor* goodie(int x, int y);
	Player* getPlayer() const { return m_player; }
	bool countKBot(int x, int y) const;
	bool canMove(int x, int y) const;
	bool isBoulder(int x, int y) const;
	bool isHole(int x, int y) const;
	bool getMe(int x, int y) const;
	bool fireIfOK(int x, int y, int dir);
	bool noObstacle(int x, int y, int dir);
	void fire(int x, int y, int dir);
	void newBot(int x, int y, int type);
	void updateText();
	std::string textFormat(int score, int level, int lives, int health, int ammo, unsigned int bonus);
	~StudentWorld();

private:
	Player* m_player;
	std::vector <Actor*> m_actor;
	int m_number;
	unsigned int m_bonus;
};

#endif // STUDENTWORLD_H_

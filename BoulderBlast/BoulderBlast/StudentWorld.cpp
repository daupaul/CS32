#include "StudentWorld.h"
#include "Level.h"
#include "Actor.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{
	m_bonus = 1000;
	m_number = 0;
	Level lev(assetDirectory());
	int curLevel = getLevel();
	ostringstream oss;
	oss << "level";
	oss.fill('0');
	oss << setw(2) << curLevel;
	oss	<< ".dat";
	string target = oss.str();
	Level::LoadResult result = lev.loadLevel(target);
	if (curLevel >= 99 || result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return  GWSTATUS_LEVEL_ERROR;
	else if (result == Level::load_success)
	{
		for (int x = 0; x != VIEW_WIDTH; x++)
		{
			for (int y = 0; y != VIEW_HEIGHT; y++)
			{
				Level::MazeEntry item = lev.getContentsOf(x, y); // x=5, y=10
				switch (item)
				{
				case Level::empty:
					break;
				case Level::exit:
					m_actor.push_back(new Exit(this, x, y));
					m_number++;
					break;
				case Level::player:
					m_player = new Player(this, x, y);
					break;
				case Level::horiz_snarlbot:
					m_actor.push_back(new SnarlBot(this, x, y, GraphObject::right));
					m_number++;
					break;
				case Level::vert_snarlbot:
					m_actor.push_back(new SnarlBot(this, x, y, GraphObject::down));
					m_number++;
					break;
				case Level::kleptobot_factory:
					m_actor.push_back(new RobotFactory(this, x, y, 1));
					m_number++;
					break;
				case Level::angry_kleptobot_factory:
					m_actor.push_back(new RobotFactory(this, x, y, 2));
					m_number++;
					break;
				case Level::wall:
					m_actor.push_back(new Wall(this, x, y));
					m_number++;
					break;
				case Level::boulder:
					m_actor.push_back(new Boulder(this, x, y));
					m_number++;
					break;
				case Level::hole:
					m_actor.push_back(new Hole(this, x, y));
					m_number++;
					break;
				case Level::jewel:
					m_actor.push_back(new Jewel(this, x, y));
					m_number++;
					break;
				case Level::extra_life:
					m_actor.push_back(new ExtraLife(this, x, y));
					m_number++;
					break;
				case Level::restore_health:
					m_actor.push_back(new RestoreHealth(this, x, y));
					m_number++;
					break;
				case Level::ammo:
					m_actor.push_back(new Ammo(this, x, y));
					m_number++;
					break;
				}
			}
		}	
	}
	return  GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	updateText();
	for (int k = m_number; k > 0; k--)
		m_actor[k - 1]->doSomething();
	m_player->doSomething();
	for (int k = 0; k < m_number; k++)
		if (m_actor[k]->isDead())
			eraseActor(k);
	getExit()->doSomething();
	if (getExit()->complete())
	{
		increaseScore(m_bonus);
		return GWSTATUS_FINISHED_LEVEL;
	}
	if (m_bonus > 0)
		m_bonus--;
	if (numJewel() == 0 && !getExit()->isVisible())
	{
		getExit()->setVisible(true);
		playSound(SOUND_REVEAL_EXIT);
	}
	if (m_player->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::numJewel() const
{
	int count = 0;
	for (int k = 0; k < m_number; k++)
	{
		Actor* target = m_actor[k];
		Jewel* wp = dynamic_cast<Jewel*>(target);
		if (wp != nullptr)
			count++;
	}
	return count;
}

Actor* StudentWorld::objectAt(int x, int y) const 
{
	for (int k = 0; k < m_number; k++)
		if (m_actor[k]->getX() == x && m_actor[k]->getY() == y)
			return m_actor[k];
	return nullptr;
}

Actor* StudentWorld::overlap(int x, int y) const
{
	if (getPlayer()->getX() == x && getPlayer()->getY() == y)
		return m_player;
	for (int k = 0; k != m_number; k++)
	{
		if (m_actor[k]->getX() == x && m_actor[k]->getY() == y)
		{
			SnarlBot* wp11 = dynamic_cast<SnarlBot*>(m_actor[k]);
			KleptoBot* wp12 = dynamic_cast<KleptoBot*>(m_actor[k]);
			if (wp11 != nullptr && wp12 != nullptr)
				return m_actor[k];
			Bullet* wp1 = dynamic_cast<Bullet*>(m_actor[k]);
			Hole* wp2 = dynamic_cast<Hole*>(m_actor[k]);
			Goodie* wp3 = dynamic_cast<Goodie*>(m_actor[k]);
			Exit* wp4 = dynamic_cast<Exit*>(m_actor[k]);
			Jewel* wp5 = dynamic_cast<Jewel*>(m_actor[k]);
			if (wp1 == nullptr && wp2 == nullptr && wp3 == nullptr && wp4 == nullptr && wp5 == nullptr)
				return m_actor[k];
		}
	}
	return nullptr;
}

Actor* StudentWorld::getExit() const
{
	for (int k = 0; k < m_number; k++)
	{
		Exit* wp = dynamic_cast<Exit*>(m_actor[k]);
		if (wp != nullptr)
			return m_actor[k];
	}
	return nullptr;
}

bool StudentWorld::countKBot(int x, int y) const
{
	int maxX, minX, maxY, minY;
	maxX = x + 3;
	minX = x - 3;
	maxY = y + 3;
	minY = y - 3;
	int count = 0;
	for (int k = 0; k != m_number; k++)
	{
		Actor* target = m_actor[k];
		KleptoBot* wp = dynamic_cast<KleptoBot*>(target);
		if (wp != nullptr)
		{
			if (target->getX() == x && target->getY() == y)
				return false;
			if (target->getX() <= maxX && target->getX() >= minX)
				if (target->getY() <= maxY && target->getY() >= minY)
					count++;
		}
	}
	if (count < 3)
		return true;
	return false;
}

bool StudentWorld::canMove(int x, int y) const
{
	if (getPlayer()->getX() == x && getPlayer()->getY() == y)
		return false;
	for (int k = 0; k != m_number; k++)
	{
		Actor* target = m_actor[k];
		if (target->getX() == x && target->getY() == y)
		{
			Wall* wp1 = dynamic_cast<Wall*>(target);
			Boulder* wp2 = dynamic_cast<Boulder*>(target);
			Hole* wp3 = dynamic_cast<Hole*>(target);
			SnarlBot* wp4 = dynamic_cast<SnarlBot*>(target);
			KleptoBot* wp5 = dynamic_cast<KleptoBot*>(target);
			RobotFactory* wp6 = dynamic_cast<RobotFactory*>(target);
			if (wp1 != nullptr || wp2 != nullptr || wp3 != nullptr || wp4 != nullptr || wp5 != nullptr || wp6 != nullptr)
				return false;
		}
	}
	return true;
}

bool StudentWorld::isBoulder(int x, int y) const
{
	Actor* target = objectAt(x, y);
	if (target != nullptr)
	{
		Boulder* wp = dynamic_cast<Boulder*>(target);
		if (wp != nullptr)
			return true;
	}
	return false;
}

bool StudentWorld::isHole(int x, int y) const
{
	Actor* target = objectAt(x, y);
	if (target != nullptr)
	{
		Hole* wp = dynamic_cast<Hole*>(target);
		if (wp != nullptr)
			return true;
	}
	return false;
}

bool StudentWorld::getMe(int x, int y) const
{
	if (getPlayer()->getX() == x && getPlayer()->getY() == y)
		return true;
	return false;
}

bool StudentWorld::fireIfOK(int x, int y, int dir)
{
	GraphObject::Direction point;
	switch (dir)
	{
	case 1:
		point = GraphObject::up;
		break;
	case 2:
		point = GraphObject::down;
		break;
	case 3:
		point = GraphObject::left;
		break;
	case 4:
		point = GraphObject::right;
		break;
	}
	switch (dir)
	{
	case 1:
		if (m_player->getY() > y && m_player->getX() == x)
		{
			if (noObstacle(x, y, dir))
			{
				fire(x, y + 1, point);
				playSound(SOUND_ENEMY_FIRE);
				return true;
			}	
		}
		break;
	case 2:
		if (m_player->getY() < y && m_player->getX() == x)
		{
			if (noObstacle(x, y, dir))
			{
				fire(x, y - 1, point);
				playSound(SOUND_ENEMY_FIRE);
				return true;
			}
		}
		break;
	case 3:
		if (m_player->getX() < x && m_player->getY() == y)
		{
			if (noObstacle(x, y, dir))
			{
				fire(x - 1, y, point);
				playSound(SOUND_ENEMY_FIRE);
				return true;
			}
		}
		break;
	case 4:
		if (m_player->getX() > x && m_player->getY() == y)
		{
			if (noObstacle(x, y, dir))
			{
				fire(x + 1, y, point);
				playSound(SOUND_ENEMY_FIRE);
				return true;
			}
		}
		break;
	}
	return false;
}

bool StudentWorld::noObstacle(int x, int y, int dir)
{
	switch (dir)
	{
	case 1:
		for (int k = y + 1; k != m_player->getY(); k++)
		{
			Actor* check = objectAt(x, k);
			if (check == nullptr)
				continue;
			Wall* wp1 = dynamic_cast<Wall*>(check);
			Boulder* wp2 = dynamic_cast<Boulder*>(check);
			SnarlBot* wp3 = dynamic_cast<SnarlBot*>(check);
			KleptoBot* wp4 = dynamic_cast<KleptoBot*>(check);
			RobotFactory* wp5 = dynamic_cast<RobotFactory*>(check);
			if (wp1 != nullptr || wp2 != nullptr || wp3 != nullptr || wp4 != nullptr || wp5 != nullptr)
				return false;
		}
		break;
	case 2:
		for (int k = y - 1; k != m_player->getY(); k--)
		{
			Actor* check = objectAt(x, k);
			if (check == nullptr)
				continue;
			Wall* wp1 = dynamic_cast<Wall*>(check);
			Boulder* wp2 = dynamic_cast<Boulder*>(check);
			SnarlBot* wp3 = dynamic_cast<SnarlBot*>(check);
			KleptoBot* wp4 = dynamic_cast<KleptoBot*>(check);
			RobotFactory* wp5 = dynamic_cast<RobotFactory*>(check);
			if (wp1 != nullptr || wp2 != nullptr || wp3 != nullptr || wp4 != nullptr || wp5 != nullptr)
				return false;
		}
		break;
	case 3:
		for (int k = x - 1; k != m_player->getX(); k--)
		{
			Actor* check = objectAt(k, y);
			if (check == nullptr)
				continue;
			Wall* wp1 = dynamic_cast<Wall*>(check);
			Boulder* wp2 = dynamic_cast<Boulder*>(check);
			SnarlBot* wp3 = dynamic_cast<SnarlBot*>(check);
			KleptoBot* wp4 = dynamic_cast<KleptoBot*>(check);
			RobotFactory* wp5 = dynamic_cast<RobotFactory*>(check);
			if (wp1 != nullptr || wp2 != nullptr || wp3 != nullptr || wp4 != nullptr || wp5 != nullptr)
				return false;
		}
		break;
	case 4:
		for (int k = x + 1; k != m_player->getX(); k++)
		{
			Actor* check = objectAt(k, y);
			if (check == nullptr)
				continue;
			Wall* wp1 = dynamic_cast<Wall*>(check);
			Boulder* wp2 = dynamic_cast<Boulder*>(check);
			SnarlBot* wp3 = dynamic_cast<SnarlBot*>(check);
			KleptoBot* wp4 = dynamic_cast<KleptoBot*>(check);
			RobotFactory* wp5 = dynamic_cast<RobotFactory*>(check);
			if (wp1 != nullptr || wp2 != nullptr || wp3 != nullptr || wp4 != nullptr || wp5 != nullptr)
				return false;
		}
		break;
	}
	return true;
}

void StudentWorld::fire(int x, int y, int dir)
{
	GraphObject::Direction point;
	switch (dir)
	{
	case 1:
		point = GraphObject::up;
		break;
	case 2:
		point = GraphObject::down;
		break;
	case 3:
		point = GraphObject::left;
		break;
	case 4:
		point = GraphObject::right;
		break;
	}
	m_actor.push_back(new Bullet(this, x, y, point));
	m_number++;
}

void StudentWorld::newBot(int x, int y, int type)
{
	if (type == 1)
	{
		m_actor.push_back(new KleptoBot(this, IID_KLEPTOBOT, x, y));
		m_number++;
	}
	if (type == 2)
	{
		m_actor.push_back(new AngryKleptoBot(this, x, y));
		m_number++;
	}
}

Actor* StudentWorld::goodie(int x, int y)
{
	for (int k = 0; k != m_number; k++)
	{
		if (m_actor[k]->getX() == x && m_actor[k]->getY() == y)
		{
			Goodie* wp = dynamic_cast<Goodie*>(m_actor[k]);
			if (wp != nullptr)
				return m_actor[k];
		}
	}
	return nullptr;
}

void StudentWorld::updateText()
{
	int level = getLevel();
	int score = getScore();
	int lives = getLives();
	int health = m_player->getHealth() * 5;
	int ammo = m_player->getAmmo();
	string text = textFormat(score, level, lives, health, ammo, m_bonus);
	setGameStatText(text);
}

string StudentWorld::textFormat(int score, int level, int lives, int health, int ammo, unsigned int bonus)
{
	ostringstream p_score;
	p_score << "Score: ";
	p_score.fill('0');
	p_score << setw(7) << score;
	ostringstream p_level;
	p_level << "Level: ";
	p_level.fill('0');
	p_level << setw(2) << level;
	ostringstream p_lives;
	p_lives << "Lives: ";
	p_lives << setw(2) << lives;
	ostringstream p_health;
	p_health << "Health: ";
	p_health << setw(3) << health << "%";
	ostringstream p_ammo;
	p_ammo << "Ammo: ";
	p_ammo << setw(3) << ammo;
	ostringstream p_bonus;
	p_bonus << "Bonus: ";
	p_bonus << setw(4) << bonus;

	string result = p_score.str() + "  " + p_level.str() + "  " + p_lives.str() + "  " + p_health.str() + "  " + p_ammo.str() + "  " + p_bonus.str();
	return result;
}

void StudentWorld::clearActor()
{
	for (int k = m_number; k != 0; k--)
	{
		delete m_actor[k - 1];
		m_actor.pop_back();
	}
	m_number = 0;
}

void StudentWorld::cleanUp()
{
	delete m_player;
	clearActor();
	m_bonus = 0;
}

void StudentWorld::eraseActor(int number)
{
	delete m_actor[number];
	m_actor.erase(m_actor.begin() + number);
	m_number--;
}

StudentWorld::~StudentWorld()
{
	delete m_player;
	clearActor();
	m_bonus = 0;
}
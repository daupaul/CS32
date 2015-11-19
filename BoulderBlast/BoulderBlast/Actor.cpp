#include "Actor.h"
#include "StudentWorld.h"
#include "Level.h"
#include <cstdlib>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* pp, int imageID, int startX, int startY, Direction dir)
	:GraphObject(imageID, startX, startY, dir)
{
	m_world = pp;
	m_countMove = 0;
}

Player::Player(StudentWorld* pp, int startX, int startY)
	:Actor(pp, IID_PLAYER, startX, startY, right)
{
	setVisible(true);
	m_health = 20;
	m_ammo = 20;
}

void Player::gainHealth()
{
	m_health = 20;
}

void Player::gainAmmo()
{
	m_ammo += 20;
}

bool Player::isDead() const
{
	if (m_health == 0)
		return true;
	return false;
}

void Player::doSomething()
{
	if (isDead())
		return;
	int key;
	if (getWorld()->getKey(key))
	{
		// user hit a key this tick!
		switch (key)
		{
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (getWorld()->isBoulder(getX() - 1, getY()))
			{
				Actor* target = getWorld()->objectAt(getX() - 1, getY());
				if (target->bePushed(left))
					moveTo(getX() - 1, getY());
			}
			else if (getWorld()->canMove(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (getWorld()->isBoulder(getX() + 1, getY()))
			{
				Actor* target = getWorld()->objectAt(getX() + 1, getY());
				if (target->bePushed(right))
					moveTo(getX() + 1, getY());
			}
			else if (getWorld()->canMove(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (getWorld()->isBoulder(getX(), getY() + 1))
			{
				Actor* target = getWorld()->objectAt(getX(), getY() + 1);
				if (target->bePushed(up))
					moveTo(getX(), getY() + 1);
			}
			else if (getWorld()->canMove(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (getWorld()->isBoulder(getX(), getY() - 1))
			{
				Actor* target = getWorld()->objectAt(getX(), getY() - 1);
				if (target->bePushed(down))
					moveTo(getX(), getY() - 1);
			}
			else if (getWorld()->canMove(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_SPACE:
			if (m_ammo > 0)
			{
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				m_ammo--;
				Direction dir = getDirection();
				switch (dir)
				{
				case up:
					getWorld()->fire(getX(), getY() + 1, 1);
					break;
				case down:
					getWorld()->fire(getX(), getY() - 1, 2);
					break;
				case left:
					getWorld()->fire(getX() - 1, getY(), 3);
					break;
				case right:
					getWorld()->fire(getX() + 1, getY(), 4);
					break;
				}
			}
			break;
		case KEY_PRESS_ESCAPE:
			m_health = 0;
			break;
		}
	}
}

void Player::beAttacked()
{
	m_health -= 2;
	if (m_health == 0)
		getWorld()->playSound(SOUND_PLAYER_DIE);
	else
		getWorld()->playSound(SOUND_PLAYER_IMPACT);
}

void Boulder::beAttacked()
{
	m_health = m_health - 2;
}

bool Boulder::isDead() const
{
	if (m_health == 0)
		return true;
	return false;
}

void Boulder::setDead()
{
	m_health = 0;
}

bool Boulder::bePushed(Direction dir)
{
	switch (dir)
	{
	case left:
		if (getWorld()->objectAt(getX() - 1, getY()) == nullptr || getWorld()->isHole(getX() - 1, getY()))
		{
			moveTo(getX() - 1, getY());
			return true;
		}
		break;
	case right:
		if (getWorld()->objectAt(getX() + 1, getY()) == nullptr || getWorld()->isHole(getX() + 1, getY()))
		{
			moveTo(getX() + 1, getY());
			return true;
		}
		break;
	case up:
		if (getWorld()->objectAt(getX(), getY() + 1) == nullptr || getWorld()->isHole(getX(), getY() + 1))
		{
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;
	case down:
		if (getWorld()->objectAt(getX(), getY() - 1) == nullptr || getWorld()->isHole(getX(), getY() - 1))
		{
			moveTo(getX(), getY() - 1);
			return true;
		}
		break;
	}
	return false;
}

void Bullet::doSomething()
{
	if (isDead())
		return;
	Actor* target = getWorld()->overlap(getX(), getY());
	if (target != nullptr)
	{
		Player* wp = dynamic_cast<Player*>(target);
		if (wp != nullptr)
			getWorld()->getPlayer()->beAttacked();
		else
			target->beAttacked();
		m_dead = true;
		return;
	}

	Direction dir = getDirection();
	switch (dir)
	{
	case left:
		moveTo(getX() - 1, getY());
		break;
	case right:
		moveTo(getX() + 1, getY());
		break;
	case up:
		moveTo(getX(), getY() + 1);
		break;
	case down:
		moveTo(getX(), getY() - 1);
		break;
	}
	target = getWorld()->overlap(getX(), getY());
	if (target != nullptr)
	{
		Player* wp = dynamic_cast<Player*>(target);
		if (wp != nullptr)
			getWorld()->getPlayer()->beAttacked();
		else
			target->beAttacked();
		m_dead = true;
		return;
	}
}

void Hole::doSomething()
{
	if (m_dead)
		return;
	Actor* pos = getWorld()->overlap(getX(), getY());
	if (pos != nullptr)
	{
		m_dead = true;
		pos->setDead();
	}
}

void Jewel::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->getMe(getX(), getY()))
	{
		getWorld()->increaseScore(50);
		m_dead = true;
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}

void Exit::doSomething()
{
	if (isVisible() && getWorld()->getMe(getX(), getY()))
	{
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->increaseScore(2000);
		m_complete = true;
	}
}

Goodie::Goodie(StudentWorld* pp, int imageID, int startX, int startY)
	:Actor(pp, imageID, startX, startY, none)
{
	setVisible(true);
	m_dead = false;
}

void Goodie::setDead()
{
	m_dead = true;
}

void ExtraLife::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->getMe(getX(),getY()) && isVisible())
	{
		getWorld()->increaseScore(1000);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		setDead();
		getWorld()->incLives();
	}
}

void RestoreHealth::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->getMe(getX(), getY()) && isVisible())
	{
		getWorld()->increaseScore(500);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->gainHealth();
	}
}

void Ammo::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->getMe(getX(), getY()) && isVisible())
	{
		getWorld()->increaseScore(100);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->gainAmmo();
	}
}

bool SnarlBot::isDead() const
{
	if (m_health == 0)
		return true;
	return false;
}

void SnarlBot::doSomething()
{
	if (isDead())
		return;
	m_restCount--;
	if (m_restCount != 0)
		return;
	m_restCount = m_ticks;
	Direction dir = getDirection();
	switch (dir)
	{
	case up:
		if(getWorld()->fireIfOK(getX(), getY(), 1))
			return;
		else if (getWorld()->canMove(getX(), getY() + 1))
			moveTo(getX(), getY() + 1);
		else
			setDirection(down);
		break;
	case down:
		if(getWorld()->fireIfOK(getX(), getY(), 2))
			return;
		else if (getWorld()->canMove(getX(), getY() - 1))
			moveTo(getX(), getY() - 1);
		else
			setDirection(up);
		break;
	case left:
		if (getWorld()->fireIfOK(getX(), getY(), 3))
			return;
		else if (getWorld()->canMove(getX() - 1, getY()))
			moveTo(getX() - 1, getY());
		else
			setDirection(right);
		break;
	case right:
		if(getWorld()->fireIfOK(getX(), getY(), 4))
			return;
		else if (getWorld()->canMove(getX() + 1, getY()))
			moveTo(getX() + 1, getY());
		else
			setDirection(left);
		break;
	}
}

void SnarlBot::beAttacked()
{
	m_health -= 2;
	if (m_health == 0)
	{
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(100);
	}
	else
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

bool KleptoBot::isDead() const
{
	if (m_health <= 0)
		return true;
	return false;
}

void KleptoBot::doSomething()
{
	if (isDead())
		return;
	m_restCount--;
	if (m_restCount != 0)
		return;
	m_restCount = m_ticks;
	if (getGoodie())
		return;
	if (moveIfCan())
		return;
	m_dTurning = rand() % 6 + 1;
	m_count = 0;
	int d = rand() % 4 + 1;
	if (!change(d,0))
	{
		Direction point;
		switch (d)
		{
		case 1:
			point = up;
			break;
		case 2:
			point = down;
			break;
		case 3:
			point = left;
			break;
		case 4:
			point = right;
			break;
		}
		setDirection(point);
	}
}

void KleptoBot::beAttacked()
{
	m_health -= 2;
	if (m_health > 0)
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		if (m_store != nullptr)
		{
			m_store->moveTo(getX(), getY());
			m_store->setVisible(true);
			m_store = nullptr;
		}
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(10);
	}
}

bool KleptoBot::change(int dir, int n)
{
	if (n == 4)
		return false;
	if (dir > 4)
		dir = dir % 4;
	switch (dir)
	{
	case 1:
		if (move(getX(), getY() + 1))
		{
			setDirection(up);
			return true;
		}
		break;
	case 2:
		if (move(getX(), getY() - 1))
		{
			setDirection(down);
			return true;
		}
		break;
	case 3:
		if (move(getX() - 1, getY()))
		{
			setDirection(left);
			return true;
		}
		break;
	case 4:
		if (move(getX() + 1, getY()))
		{
			setDirection(right);
			return true;
		}
		break;
	}
	return change(dir + 1, n + 1);
}

bool KleptoBot::getGoodie()
{
	Actor* object = getWorld()->goodie(getX(), getY());
	if (object != nullptr && store() == nullptr)
	{
		int chance = rand() % 10;
		if (chance == 0)
		{
			object->setVisible(false);
			setStore(object);
			getWorld()->playSound(SOUND_ROBOT_MUNCH);
			return true;
		}
	}
	return false;
}

bool KleptoBot::moveIfCan()
{
	if (m_count != m_dTurning)
	{
		m_count++;
		Direction dir = getDirection();
		switch (dir)
		{
		case up:
			if (move(getX(), getY() + 1))
				return true;
			break;
		case down:
			if (move(getX(), getY() - 1))
				return true;
			break;
		case left:
			if (move(getX() - 1, getY()))
				return true;
			break;
		case right:
			if (move(getX() + 1, getY()))
				return true;
			break;
		}
	}
	return false;
}

bool KleptoBot::move(int x, int y)
{
	if (getWorld()->canMove(x, y))
	{
		moveTo(x, y);
		m_count++;
		return true;
	}
	return false;
}

void KleptoBot::setHealth(int n)
{
	m_health = n;
}

void KleptoBot::decHealth(int n)
{
	m_health -= n;
}

void KleptoBot::setStore(Actor* pp)
{
	m_store = pp;
}

void AngryKleptoBot::doSomething()
{
	if (isDead())
		return;
	m_restCount--;
	if (m_restCount != 0)
		return;
	m_restCount = ticks();
	
	Direction dir = getDirection();
	switch (dir)
	{
	case up:
		if (getWorld()->fireIfOK(getX(), getY(), 1))
			return;
		break;
	case down:
		if (getWorld()->fireIfOK(getX(), getY(), 2))
			return;
		break;
	case left:
		if (getWorld()->fireIfOK(getX(), getY(), 3))
			return;
		break;
	case right:
		if (getWorld()->fireIfOK(getX(), getY(), 4))
			return;
		break;
	}

	if (getGoodie())
		return;
	if (moveIfCan())
		return;

	m_dTurning = rand() % 6 + 1;
	m_count = 0;
	int d = rand() % 4 + 1;
	if (!change(d, 0))
	{
		Direction point;
		switch (d)
		{
		case 1:
			point = up;
			break;
		case 2:
			point = down;
			break;
		case 3:
			point = left;
			break;
		case 4:
			point = right;
			break;
		}
		setDirection(point);
	}
}

void AngryKleptoBot::beAttacked()
{
	decHealth(2);
	if (health() > 0)
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		if (store() != nullptr)
		{
			store()->moveTo(getX(), getY());
			store()->setVisible(true);
			setStore(nullptr);
		}
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(20);
	}
}

void RobotFactory::doSomething()
{
	if (getWorld()->countKBot(getX(),getY()))
	{
		int chance = rand() % 50;
		if (chance == 0)
		{
			getWorld()->newBot(getX(), getY(), m_type);
			getWorld()->playSound(SOUND_ROBOT_BORN);
		}
	}
}


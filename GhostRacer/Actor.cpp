#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
using namespace std;

#define PI 3.14159265


	//	Helper Functions

bool isInBounds(double x, double y)
{
	return !(x < 0 || y < 0 || x > VIEW_WIDTH || y > VIEW_HEIGHT);
}

bool isOverlapping(Actor* i, Actor* j)
{
	double deltaX = abs(i->getX() - j->getX());
	double deltaY = abs(i->getY() - j->getY());
	double radSum = i->getRadius() + j->getRadius();
	if (deltaX < radSum * 0.25 && deltaY < radSum * 0.6)
		return true;
	return false;
}


///////////////////////////////////////////////
//
//	Actor Class
//
///////////////////////////////////////////////

void Actor::doOtherCircumstances()
{
	if (isCollidable())
		doCollision();
	if (isSpinnable())
		doSpin();
	if (isAffectedByHW())
		doHW();
	if (isHealable())
		doHeal();
}


///////////////////////////////////////////////
//
//	Human Pedestrian Class
//
///////////////////////////////////////////////

HumanPedestrian::HumanPedestrian(double startX, double startY, StudentWorld* wPtr)
	: Character(IID_HUMAN_PED, startX, startY, 2, 0, 2, 0), m_movementPlanDistance(0)
{
	setVertSpeed(-4);
	setHorizSpeed(0);
	setCollidable(true);
	setAffectedByHW(true);
	setSpinnable(false);
	setHealable(false);
	setWorld(wPtr);
	// Movement plan of 0?
}

void HumanPedestrian::doSomething()
{
	updateLifeStatus();
	if (!isAlive())
		return;

	// If overlap with ghost racer
	Actor* grPtr = getWorld()->getGhostRacer();
	if (isOverlapping(this, grPtr))
	{
		grPtr->setLife(false);
		return;
	}

	// Move human pedestrian
	int vSpeed = getVertSpeed() - getWorld()->getGhostRacer()->getVertSpeed();
	int hSpeed = getHorizSpeed();
	double newY = getY() + vSpeed;
	double newX = getX() + hSpeed;
	moveTo(newX, newY);
	if (!isInBounds(getX(), getY()))
	{
		setLife(false);
		return;
	}

	// Determine movement plan
	decrementMovementPlanDist();
	if (getMovementPlanDistance() > 0)
		return;
	else
	{
		int newHSpeed = randInt(-3, 3);
		while (newHSpeed == 0)
			newHSpeed = randInt(-3, 3);
		setHorizSpeed(newHSpeed);		// Check speed != 0		!!!
		setMovementPlanDist(randInt(4, 32));
		if (getHorizSpeed() < 0)
			setDirection(180);
		else if (getHorizSpeed() > 0)
			setDirection(0);
	}

	//doOtherCircumstances();		//		DETERMINE WHEN THIS OCCURS	!!!
}

void HumanPedestrian::doHW()
{
	if (true)					// FIGURE OUT HOW TO DETERMINE IF THEY OVERLAP	!!!
	{
		setHorizSpeed(getHorizSpeed() * -1);
		if (getHorizSpeed() < 0)				//	Duplicate code with line 98	!!!
			setDirection(180);
		else if (getHorizSpeed() > 0)
			setDirection(0);
		getWorld()->playSound(SOUND_PED_HURT);
	}
}


///////////////////////////////////////////////
//
//	Ghost Racer Class
//
///////////////////////////////////////////////

GhostRacer::GhostRacer(StudentWorld* wPtr)
	: Character(IID_GHOST_RACER, 128, 32, 100, 90, 4.0)
{
	m_unitsOfHolyWater = 10;
	setAffectedByHW(false);
	setVertSpeed(0);
	setHorizSpeed(0);
	setWorld(wPtr);
	setCollidable(true);
	setHealable(true);
	setSpinnable(true);	// Check healable and spinnable				!!!
}

void GhostRacer::moveGR()
{
	double maxShiftPerTick = 4.0;
	int dir = getDirection();			// Some stuff in algo seems superfluous				!!!
	double delX = cos(dir * PI / 180.0) * maxShiftPerTick;
	double curX = getX();
	double curY = getY();
	moveTo(curX + delX, curY);
}

void GhostRacer::doSomething()
{
	updateLifeStatus();
	if (!isAlive())
		return;

	// Check for swerving into boundary
	int dir = getDirection();
	if (getX() <= LEFT_EDGE)
	{
		if (dir > 90)
		{
			damageItself(10);
			updateLifeStatus();
			if (!isAlive())
			{
				setLife(false);
				getWorld()->playSound(SOUND_PLAYER_DIE);			// NOT SURE IF I SHOULD RETURN HERE...SAME FOR RIGHT_EDGE
			}
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		moveGR();
		return;
	}
	if (getX() >= RIGHT_EDGE)		// CODE IS VERY SIMILAR TO LEFT EDGE			!!!
	{
		if (dir < 90)
		{
			damageItself(10);
			updateLifeStatus();
			if (!isAlive())
			{
				setLife(false);
				getWorld()->playSound(SOUND_PLAYER_DIE);
			}
		}
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		moveGR();
		return;
	}

	// Check for inputs
	int key;
	int vSpeed = getVertSpeed();
	dir = getDirection();
	if (getWorld()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_SPACE:
			if (getUnitsOfHolyWater() < 1)		//		NEEDS TO BE IMPLEMENTED		!!!
				break;
			break;
		case KEY_PRESS_LEFT:
			if (dir >= 114)
			{
				moveGR();
				break;
			}
			setDirection(dir + 8);
			moveGR();
			break;
		case KEY_PRESS_RIGHT:
			if (dir <= 66)
			{
				moveGR();
				break;
			}			setDirection(dir - 8);
			moveGR();
			break;
		case KEY_PRESS_UP:
			if (vSpeed >= 5)
			{
				moveGR();
				break;
			}			setVertSpeed(vSpeed + 1);
			moveGR();
			break;
		case KEY_PRESS_DOWN:
			if (vSpeed <= -1)
			{
				moveGR();
				break;
			}			setVertSpeed(vSpeed - 1);
			moveGR();
			break;
		}
	}
	else
		moveGR();
}


///////////////////////////////////////////////
//
//	BorderLine Class
//
///////////////////////////////////////////////

BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr)
// Contradiction with depth, says 2 and 1 at different parts in spec			!!!
	: Environmentals(imageID, startX, startY, 0, 2.0)
{
	setVertSpeed(-4);
	setAffectedByHW(false);
	setHorizSpeed(0);
	setWorld(wPtr);
	setCollidable(false);
	setHealable(false);
	setSpinnable(false);	// Check healable and spinnable				!!!
} // CHECK DEPTH

void BorderLine::doSomething()
{
	// Calculate speed
	int vSpeed = getVertSpeed() - getWorld()->getGhostRacer()->getVertSpeed();
	int hSpeed = getHorizSpeed();

	// Get new position and update
	double newY = getY() + vSpeed;
	double newX = getX() + hSpeed;
	moveTo(newX, newY);

	if (!isInBounds(getX(), getY()))
	{
		setLife(false);
		return;
	}
}


///////////////////////////////////////////////
//
//	Soul Class
//
///////////////////////////////////////////////

Soul::Soul(double startX, double startY, StudentWorld* wPtr)
	: Consumables(IID_SOUL_GOODIE, startX, startY, 0, 4.0)
{
	setVertSpeed(-4);
	setHorizSpeed(0);
	setWorld(wPtr);
	setCollidable(false);
	setAffectedByHW(false);
	setHealable(false);
	setSpinnable(false);
}

void Soul::doSomething()
{
	Actor* grPtr = getWorld()->getGhostRacer();
	int vSpeed = getVertSpeed() - grPtr->getVertSpeed();
	int hSpeed = getHorizSpeed();

	double newY = getY() + vSpeed;
	double newX = getX() + hSpeed;
	moveTo(newX, newY);

	if (!isInBounds(getX(), getY()))
	{
		setLife(false);
		return;
	}

	if (isOverlapping(this, grPtr))
	{
		getWorld()->incrementSoulsSaved();
		setLife(false);
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->setScore(getWorld()->getScore() + 100);
	}

	if (getDirection() >= 10)
		setDirection(getDirection() - 10);
	else
	{
		// 360 + dir - 10
		int angle = 350 + getDirection();
		setDirection(angle);
	}
}
#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <math.h>

#define PI 3.14159265

///////////////////////////////////////////////
//
//	Ghost Racer Class
//
///////////////////////////////////////////////

GhostRacer::GhostRacer(StudentWorld* wPtr)
	: Character(IID_GHOST_RACER, 128, 32, 100, 90, 4.0)
{
	m_unitsOfHolyWater = 10;
	m_soulsSaved = 0;
	setVertSpeed(0.0);
	setHorizSpeed(0.0);
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
	if (!isAlive())
		return;

	// Check for swerving into boundary
	int dir = getDirection();
	if (getX() <= LEFT_EDGE)
	{
		if (dir > 90)
		{
			damageItself(10);
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
	double vSpeed = getVertSpeed();
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

BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr, GhostRacer* grPtr)
// Contradiction with depth, says 2 and 1 at different parts in spec			!!!
	: Environmentals(imageID, startX, startY, 0, 2.0, grPtr)
{
	setVertSpeed(-4.0);
	setHorizSpeed(0.0);
	setWorld(wPtr);
	setCollidable(false);
	setHealable(false);
	setSpinnable(false);	// Check healable and spinnable				!!!
} // CHECK DEPTH

void BorderLine::doSomething()
{
	// Calculate speed
	double vSpeed = getVertSpeed() - getGhostRacer()->getVertSpeed();
	double hSpeed = getHorizSpeed();

	// Get new position and update
	double newY = getY() + vSpeed;
	double newX = getX() + hSpeed;
	moveTo(newX, newY);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)	// Check to see if out of bounds issues occur and move up if so			!!!
	{
		setLife(false);
		return;
	}
}

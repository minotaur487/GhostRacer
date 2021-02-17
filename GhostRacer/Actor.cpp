#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"


///////////////////////////////////////////////
//
//	Ghost Racer Class
//
///////////////////////////////////////////////

GhostRacer::GhostRacer(StudentWorld* wPtr)
	: Character(IID_GHOST_RACER, 128, 32, 100, 90, 4.0), m_unitsOfHolyWater(10)
{
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
	double delX = cos(dir) * maxShiftPerTick;
	double curX = getX();
	double curY = getY();
	moveTo(curX + delX, curY);
}

void GhostRacer::doSomething()
{
	if (!isAlive())
		return;

	int dir = getDirection();
	double vSpeed = getVertSpeed();
	if (getX() <= LEFT_EDGE)
	{
		if (dir > 90)
		{
			damageItself(10);
			if (!isAlive())
			{
				setLife(false);
				getWorld()->playSound(SOUND_PLAYER_DIE);
			}
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		moveGR();
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
	}
	int key;
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
				break;
			setDirection(dir + 8);
			moveGR();
			break;
		case KEY_PRESS_RIGHT:
			if (dir <= 66)
				break;
			setDirection(dir - 8);
			moveGR();
			break;
		case KEY_PRESS_UP:
			if (vSpeed >= 5)
				break;
			setVertSpeed(vSpeed + 1);
			moveGR();
			break;
		case KEY_PRESS_DOWN:
			if (vSpeed <= -1)
				break;
			setVertSpeed(vSpeed - 1);
			moveGR();
			break;
		}
	}
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
	double v_speed = getVertSpeed() - getGhostRacer()->getVertSpeed();
	double h_speed = getHorizSpeed();

	// Get new position and update
	double new_y = getY() + v_speed;
	double new_x = getX() + h_speed;
	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)	// Check to see if out of bounds issues occur and move up if so			!!!
	{
		setLife(false);
		return;
	}
}

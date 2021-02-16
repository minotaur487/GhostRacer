#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Character.h, StudentWorld.h, and StudentWorld.cpp
void GhostRacer::doSomething()
{
	if (!isAlive())
		return;

//	if (getX() <= leftBoundary)
//	{
//		if (getDirection() > 90)
//			damageItself(10);
//		setDirection(82);
//
//	}
}

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

Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth)
	: GraphObject(imageID, startX, startY, dir, size, depth) {}

GhostRacer::GhostRacer(StudentWorld* wPtr)
	: Character(IID_GHOST_RACER, 128, 32, 100, 90, 4.0), m_unitsOfHolyWater(10)
{
	setVertSpeed(0.0);
	setHorizSpeed(0.0);
	setWorld(wPtr);
	setCollidable(true);
	setHealable(true);
	setSpinnable(false);	// Check healable and spinnable				!!!
}

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
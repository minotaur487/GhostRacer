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
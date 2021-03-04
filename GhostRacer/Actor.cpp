#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
#include <cassert>	// remove after testing
using namespace std;

#define PI 3.14159265


///////////////////////////////////////////////
//
//	Actor Class
//
///////////////////////////////////////////////

void Actor::moveActor()
{
	// Get new speed
	GhostRacer* grPtr = getWorld()->getGhostRacer();
	double vSpeed = getVertSpeed() - grPtr->getVertSpeed();
	double hSpeed = getHorizSpeed();

	// Get new position and update
	double newY = getY() + vSpeed;
	double newX = getX() + hSpeed;
	moveTo(newX, newY);

	// Check if out of bounds
	if (!isInBounds(getX(), getY()))
	{
		setLife(false);
		return;
	}
}


///////////////////////////////////////////////
//
//	Character Class
//
///////////////////////////////////////////////

bool Character::beSprayedIfAppropriate() {
	if (this != getWorld()->getGhostRacer())
	{
		damageItself(1);
		return true;
	}
	return false;
}


///////////////////////////////////////////////
//
//	Holy Water Projectile Class
//
///////////////////////////////////////////////

HolyWaterProjectile::HolyWaterProjectile(double startX, double startY, int dir, StudentWorld* wPtr)
	: Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1)
{
	setCollisionWorthy(false);
	setWorld(wPtr);
	m_travelDist = 0;
}

void HolyWaterProjectile::doSomething()
{
	if (!isAlive())
		return;

	// if the projectile hit something, return
  	if (getWorld()->executeProjectileImpact(this))
		return;
	// else, move forward
	else
	{
		moveForward(SPRITE_HEIGHT);
		addTravelDist(SPRITE_HEIGHT);
	}

	if (!isInBounds(getX(), getY()))
	{
		setLife(false);
		return;
	}

	if (getDistTravelled() == 160)
	{
		setLife(false);
	}
}


///////////////////////////////////////////////
//
//	Autonomous Class
//
///////////////////////////////////////////////

void Autonomous::doSomething()
{
	if (!isAlive())
		return;

	// If overlap with ghost racer
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();
	if (wPtr->isOverlapping(this, grPtr))
	{
		doDifferentiatedOverlappingAction();
	}

	moveActor();

	// Do intermediate steps if any
	doDifferentiatedIntermediateSteps();

	// Determine movement plan
	doDifferentiatedMovement();
	if (getMovementPlanDistance() <= 0)
	{
		doDifferentiatedNewMovement();
	}
}

void Autonomous::doDifferentiatedNewMovement()
{
	// get new horizontal speed
	double newHSpeed = randInt(-3, 3);
	while (newHSpeed == 0)
		newHSpeed = randInt(-3, 3);

	// update horizontal speed and movement plan distance
	setHorizSpeed(newHSpeed);
	setMovementPlanDist(randInt(4, 32));

	// update direction
	if (getHorizSpeed() < 0)
		setDirection(180);
	else if (getHorizSpeed() > 0)
		setDirection(0);
}

void Autonomous::doDifferentiatedMovement()
{
	decrementMovementPlanDist();
	if (getMovementPlanDistance() > 0)
		return;
};


///////////////////////////////////////////////
//
//	Zombie Cab Class
//
///////////////////////////////////////////////

ZombieCab::ZombieCab(double startX, double startY, double vSpeed, StudentWorld* wPtr)
	: Autonomous(IID_ZOMBIE_CAB, startX, startY, 3, 90, 4.0)
{
	setVertSpeed(vSpeed);
	m_hasDamagedGhostRacer = false;
	setWorld(wPtr);
	setCollisionWorthy(true);
}

void ZombieCab::doDifferentiatedIntermediateSteps()
{
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();

	// find which lane the zombie cab is in
	double zombieCabX = getX();
	const int* curLane;
	if (zombieCabX >= R_LANE[0] && zombieCabX <= R_LANE[1])
		curLane = R_LANE;
	else if (zombieCabX >= L_LANE[0] && zombieCabX <= L_LANE[1])
		curLane = L_LANE;
	else
		curLane = M_LANE;

	// try to find the closest collision worthy actor in front
	Actor* closestCollisionWorthyActor = wPtr->findClosestCollisionWorthyActor(curLane, BOTTOM, this);
	bool CWActorPresent = false;
	if (closestCollisionWorthyActor != nullptr)
		CWActorPresent = true;

	// if there is a collision worthy actor in front and the current vertical speed is greater than ghost racer's
	if (getVertSpeed() > grPtr->getVertSpeed() && CWActorPresent)
	{
		// if the collision worthy actor is too close, slow down
		double delY = abs(closestCollisionWorthyActor->getY() - getY());
		if (delY < 96)
		{
			setVertSpeed(getVertSpeed() - 0.5);
			return;
		}
	}

	// try to find the closest collision worthy actor behind
	closestCollisionWorthyActor = wPtr->findClosestCollisionWorthyActor(curLane, TOP, this);
	CWActorPresent = false;
	if (closestCollisionWorthyActor != nullptr)
		CWActorPresent = true;

	// if there is a collision worthy actor behind and the current vertical speed is less than or equal to ghost racer's
	if (getVertSpeed() <= grPtr->getVertSpeed() && CWActorPresent)
	{
		// if the collision worthy actor is too close and is not ghost racer, speed up
		double delY = abs(getY() - closestCollisionWorthyActor->getY());
		if (delY < 96 && closestCollisionWorthyActor != grPtr)
		{
			setVertSpeed(getVertSpeed() + 0.5);
			return;
		}
	}
}

void ZombieCab::doDifferentiatedOverlappingAction()
{
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();

	// if the zombie cab has damaged ghost racer
	if (hasDamagedGhostRacer())
	{
		moveActor();
		return;
	}
	else
	{
		// execute crash sequence
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		grPtr->damageItself(20);
		grPtr->actionsWhenDamaged();
		// if the zombie cab's x coordinate is less than or equal to ghost racer's, swerve off to the left
		if (getX() <= grPtr->getX())
		{
			setHorizSpeed(-5);
			int rand = randInt(0, 19);
			setDirection(120 + rand);
		}
		// else if the zombie cab's x coordinate is greater than ghost racer's, swerve off to the right
		else if (getX() > grPtr->getX())
		{
			setHorizSpeed(5);
			int rand = randInt(0, 19);
			setDirection(60 - rand);
		}
		indicateDamagedGhostRacer();
	}
};

bool ZombieCab::beSprayedIfAppropriate()
{
	Character::beSprayedIfAppropriate();
	actionsWhenDamaged();
	return true;
}

void ZombieCab::actionsWhenDamaged()
{
	StudentWorld* wPtr = getWorld();
	// if zombie cab is not alive, execute death sequence
	if (!isAlive())
	{
		wPtr->playSound(SOUND_VEHICLE_DIE);
		// oil slick has 1 in 5 chance of spawning
		int rand = randInt(1, 5);
		if (rand == 1)
		{
			int randSize = randInt(2, 5);
			wPtr->addActor(new OilSlick(getX(), getY(), randSize, wPtr));
		}
		wPtr->increaseScore(200);
		return;
	}
	// otherwise it got hurt
	else
		wPtr->playSound(SOUND_VEHICLE_HURT);
}

void ZombieCab::doDifferentiatedNewMovement()
{
	setMovementPlanDist(randInt(4, 32));
	setVertSpeed(getVertSpeed() + randInt(-2, 2));
}

///////////////////////////////////////////////
//
//	Zombie Pedestrian Class
//
///////////////////////////////////////////////

ZombiePedestrian::ZombiePedestrian(double startX, double startY, StudentWorld* wPtr)
	: Pedestrian(IID_ZOMBIE_PED, startX, startY, 3.0), ticksTilGrunt(0)
{
	setVertSpeed(-4);
	setCollisionWorthy(true);
	setWorld(wPtr);
}

void ZombiePedestrian::moveActor()
{
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();

	// if zombie pedestrian is within 30 pixels horizontally and ahead of the ghost racer
	double delX = getX() - grPtr->getX();
	double delY = getY() - grPtr->getY();
	if (abs(delX) <= 30 && delY >= 0)
	{
		// have zombie ped face it and follow ghost racer
		setDirection(270);
		if (delX < 0)
		{
			setHorizSpeed(1);
		}
		else if (delX > 0)
		{
			setHorizSpeed(-1);
		}
		else
		{
			setHorizSpeed(0);
		}
		// have zombie ped attack ghost racer
		ticksTilGrunt--;
		if (ticksTilGrunt <= 0)
		{
			wPtr->playSound(SOUND_ZOMBIE_ATTACK);
			ticksTilGrunt = 20;
		}
	}

	Actor::moveActor();
}

void ZombiePedestrian::doDifferentiatedOverlappingAction()
{
	getWorld()->getGhostRacer()->damageItself(5);
	damageItself(2);
	actionsWhenDamaged();
	return;
};

void ZombiePedestrian::doDifferentiatedMovement()
{
	if (getMovementPlanDistance() > 0)
	{
		decrementMovementPlanDist();
		return;
	}
};

bool ZombiePedestrian::beSprayedIfAppropriate()
{
	Character::beSprayedIfAppropriate();
	actionsWhenDamaged();
	return true;
}

void ZombiePedestrian::actionsWhenDamaged()
{
	// if zombie ped is not alive
	StudentWorld* wPtr = getWorld();
	if (!isAlive())
	{
		wPtr->playSound(SOUND_PED_DIE);
		// if zombie ped is overlapping with ghost racer
		if (!wPtr->isOverlapping(this, wPtr->getGhostRacer()))
		{
			// there is a 1 and 5 chance for a healing goodie to spawn
			if (randInt(1, 5) == 1)
				wPtr->addActor(new HealingGoodie(getX(), getY(), wPtr));
		}
		wPtr->increaseScore(150);
	}
	else
		wPtr->playSound(SOUND_PED_HURT);
}

///////////////////////////////////////////////
//
//	Human Pedestrian Class
//
///////////////////////////////////////////////

HumanPedestrian::HumanPedestrian(double startX, double startY, StudentWorld* wPtr)
	: Pedestrian(IID_HUMAN_PED, startX, startY, 2.0)
{
	setVertSpeed(-4);
	setCollisionWorthy(true);
	setWorld(wPtr);
}

void HumanPedestrian::doDifferentiatedOverlappingAction()
{
	getWorld()->getGhostRacer()->setLife(false);
	return;
};

bool HumanPedestrian::beSprayedIfAppropriate()
{
	// move the other way
 	setHorizSpeed(getHorizSpeed() * -1);

	// face the other way
	if (getHorizSpeed() < 0)
		setDirection(180);
	else if (getHorizSpeed() > 0)
		setDirection(0);

	getWorld()->playSound(SOUND_PED_HURT);
	return true;
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
	setVertSpeed(0);
	setWorld(wPtr);
	setCollisionWorthy(true);
}

void GhostRacer::moveActor()
{
	// get change in x
	double maxShiftPerTick = 4.0;
	double dir = getDirection() * PI / 180.0;
	double delX = cos(dir) * maxShiftPerTick;

	// update position
	double curX = getX();
	double curY = getY();
	moveTo(curX + delX, curY);
}

void GhostRacer::actionsWhenDamaged()
{
	if (!isAlive())
	{
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
}

void GhostRacer::doSomething()
{
	if (!isAlive())
		return;

	StudentWorld* wPtr = getWorld();
	// Check for swerving into left boundary
	int dir = getDirection();
	if (getX() <= LEFT_EDGE)
	{
		// if gr is swerving into left boundary
		if (dir > 90)
		{
			damageItself(10);
			actionsWhenDamaged();
		}
		// make gr rebound other way
		setDirection(82);
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		moveActor();
		return;
	}
	// Check for swerving into right boundary
	if (getX() >= RIGHT_EDGE)
	{
		// if gr is swerving into right boundary
		if (dir < 90)
		{
			damageItself(10);
			actionsWhenDamaged();
		}
		// make gr rebound other way
		setDirection(98);
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		moveActor();
		return;
	}

	int key;
	double vSpeed = getVertSpeed();
	dir = getDirection();
	// if there is a key input
	if (wPtr->getKey(key))
	{
		switch (key)
		{
		// if space is pressed
		case KEY_PRESS_SPACE:
		{
			// if there is no more units of holy water projectile, don't do anything
			if (getUnitsOfHolyWater() < 1)
			{
				moveActor();
				break;
			}
			// Otherwise, add a new holy water projectile
			double angleRad = (90 - dir) * PI / 180.0;
			double newX = SPRITE_HEIGHT * sin(angleRad) + getX();
			double newY = SPRITE_HEIGHT * cos(angleRad) + getY();
			wPtr->addActor(new HolyWaterProjectile(newX, newY, getDirection(), wPtr));

			wPtr->playSound(SOUND_PLAYER_SPRAY);
			decrementUnitsOfHolyWater();
			moveActor();
			break;
		}
		// if left key is pressed
		case KEY_PRESS_LEFT:
			// if gr is not too far left, rotate gr 8 degrees left
			if (dir >= 114)
			{
				moveActor();
				break;
			}
			setDirection(dir + 8);
			moveActor();
			break;
		// if right key is pressed
		case KEY_PRESS_RIGHT:
			// if gr is not too far right, rotate gr 8 degrees right
			if (dir <= 66)
			{
				moveActor();
				break;
			}			
			setDirection(dir - 8);
			moveActor();
			break;
		// if up key is pressed
		case KEY_PRESS_UP:
			// if gr is not too fast, speed up by one unit
			if (vSpeed >= 5)
			{
				moveActor();
				break;
			}			
			setVertSpeed(vSpeed + 1);
			moveActor();
			break;
		// if down key is pressed
		case KEY_PRESS_DOWN:
			// if gr is not too slow, slow down up by one unit
			if (vSpeed <= -1)
			{
				moveActor();
				break;
			}			
			setVertSpeed(vSpeed - 1);
			moveActor();
			break;
		}
	}
	else
		moveActor();
}

///////////////////////////////////////////////
//
//	Consumables Class
//
///////////////////////////////////////////////

void Consumables::doSomething()
{
    GhostRacer* grPtr = getWorld()->getGhostRacer();
	moveActor();
	doActivity(grPtr);
}

///////////////////////////////////////////////
//
//	Healing Goodie Class
//
///////////////////////////////////////////////

HealingGoodie::HealingGoodie(double startX, double startY, StudentWorld* wPtr)
	: Consumables(IID_HEAL_GOODIE, startX, startY, 0, 1.0)
{
	setWorld(wPtr);
	setVertSpeed(-4);
	setCollisionWorthy(false);
}

void HealingGoodie::doActivity(GhostRacer* gr)
{
	StudentWorld* wPtr = getWorld();
	// if healing goodie is overlapping with ghost racer
	if (wPtr->isOverlapping(this, gr))
	{
		// increase gr's hitpoints and destroy healing goodie
		gr->setHitPoints(gr->getHitPoints() + 10);
		setLife(false);
		wPtr->playSound(SOUND_GOT_GOODIE);
		wPtr->increaseScore(250);
	}
}

bool HealingGoodie::beSprayedIfAppropriate()
{
	setLife(false);
	return true;
}


///////////////////////////////////////////////
//
//	Oil Slick Class
//
///////////////////////////////////////////////

OilSlick::OilSlick(double startX, double startY, double size, StudentWorld* wPtr)
	: Consumables(IID_OIL_SLICK, startX, startY, 0, size)
{
	setWorld(wPtr);
	setVertSpeed(-4);
	setCollisionWorthy(false);
}

void OilSlick::doActivity(GhostRacer* gr)
{
	StudentWorld* wPtr = getWorld();

	// if oil slick is overlapping with ghost racer
	if (wPtr->isOverlapping(this, gr))
	{
		// Determine direction to spin ghost racer
		wPtr->playSound(SOUND_OIL_SLICK);
		int rand = randInt(5, 20);
		int grDir = gr->getDirection();
		int prospectiveClockwise = grDir - rand;
		int prospectiveCC = grDir + rand;
		// If grDir can adjust counterclockwise, go counterclockwise
		if (prospectiveCC <= 120)
		{
			gr->setDirection(prospectiveCC);
		}
		// Else if grDir can adjust clockwise, go clockwise
		else if (prospectiveClockwise >= 60)
		{
			gr->setDirection(prospectiveClockwise);
		}
	}
}


///////////////////////////////////////////////
//
//	BorderLine Class
//
///////////////////////////////////////////////

BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr)
	: Actor(imageID, startX, startY, 0, 2.0, 2)
{
	setVertSpeed(-4);
	setWorld(wPtr);
	setCollisionWorthy(false);
}

void BorderLine::doSomething()
{
	moveActor();
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
	setWorld(wPtr);
	setCollisionWorthy(false);
}

void Soul::doActivity(GhostRacer* gr)
{
	StudentWorld* wPtr = getWorld();
	// if lost soul is overlapping with ghost racer
	if (wPtr->isOverlapping(this, gr))
	{
		// increment souls saved and destroy the soul
		wPtr->incrementSoulsSaved();
		setLife(false);
		wPtr->playSound(SOUND_GOT_SOUL);
		wPtr->increaseScore(100);
	}

	spinClockwise(10, this);
}


///////////////////////////////////////////////
//
//	Holy Water Goodie Class
//
///////////////////////////////////////////////

HolyWaterGoodie::HolyWaterGoodie(double startX, double startY, StudentWorld* wPtr)
	: Consumables(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0)
{
	setVertSpeed(-4);
	setWorld(wPtr);
	setCollisionWorthy(false);
}

void HolyWaterGoodie::doActivity(GhostRacer* gr)
{
	StudentWorld* wPtr = getWorld();
	// if holy water refill goodie is overlapping with ghost racer
	if (wPtr->isOverlapping(this, gr))
	{
		// increase the units of available spray by 10 and destroy the refill goodie
		gr->addUnitsOfHolyWater(10);
		setLife(false);
		wPtr->playSound(SOUND_GOT_GOODIE);
		wPtr->increaseScore(50);
	}
}

bool HolyWaterGoodie::beSprayedIfAppropriate()
{
	setLife(false);
	return true;
}

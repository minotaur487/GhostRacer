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
	// Get new position
	GhostRacer* grPtr = getWorld()->getGhostRacer();
	double vSpeed = getVertSpeed() - grPtr->getVertSpeed();
	double hSpeed = getHorizSpeed();

	// Update position
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

  	if (getWorld()->executeProjectileImpact(this))
		return;
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

	// Move pedestrian
	moveActor();

	// Do intermediate steps if any
	doDifferentiatedIntermediateSteps();

	// Determine movement plan
	doDifferentiatedMovement();	// Difference is minor, but a difference is a difference
	if (getMovementPlanDistance() <= 0)
	{
		doDifferentiatedNewMovement();
	}
}

void Autonomous::doDifferentiatedNewMovement()
{
	double newHSpeed = randInt(-3, 3);
	while (newHSpeed == 0)
		newHSpeed = randInt(-3, 3);
	setHorizSpeed(newHSpeed);
	setMovementPlanDist(randInt(4, 32));
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

	if (getVertSpeed() > grPtr->getVertSpeed() && CWActorPresent)
	{
		double delY = abs(closestCollisionWorthyActor->getY() - getY());
		if (delY < 96)
		{
			setVertSpeed(getVertSpeed() - 0.5);
			return;
		}
	}

	// try to find the closest collision worthy actor behind that's not gr
	closestCollisionWorthyActor = wPtr->findClosestCollisionWorthyActor(curLane, TOP, this, true);
	CWActorPresent = false;
	if (closestCollisionWorthyActor != nullptr)
		CWActorPresent = true;

	if (getVertSpeed() <= grPtr->getVertSpeed() && CWActorPresent)
	{
		double delY = abs(getY() - closestCollisionWorthyActor->getY());
		// the function to find closest collision worthy actor should have excluded gr
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
	if (hasDamagedGhostRacer())
	{
		moveActor();
		return;
	}
	else
	{
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		grPtr->damageItself(20);
		grPtr->actionsWhenDamaged();
		if (getX() <= grPtr->getX())
		{
			setHorizSpeed(-5);
			int rand = randInt(0, 19);
			setDirection(120 + rand);
		}
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
	if (!isAlive())
	{
		wPtr->playSound(SOUND_VEHICLE_DIE);
		int rand = randInt(1, 5);
		if (rand == 1)
		{
			int randSize = randInt(2, 5);
			wPtr->addActor(new OilSlick(getX(), getY(), randSize, wPtr));
		}
		wPtr->increaseScore(200);
		return;
	}
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
	double delX = getX() - grPtr->getX();
	double delY = getY() - grPtr->getY();
	if (abs(delX) <= 30 && delY >= 0)	// =0 should already be taken care of above?
	{
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
	StudentWorld* wPtr = getWorld();
	if (!isAlive())
	{
		wPtr->playSound(SOUND_PED_DIE);
		if (!wPtr->isOverlapping(this, wPtr->getGhostRacer()))
		{
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
 	setHorizSpeed(getHorizSpeed() * -1);
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
	double maxShiftPerTick = 4.0;
	double dir = getDirection() * PI / 180.0;
	double delX = cos(dir) * maxShiftPerTick;
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
	// Check for swerving into boundary
	int dir = getDirection();
	if (getX() <= LEFT_EDGE)
	{
		if (dir > 90)
		{
			damageItself(10);
			actionsWhenDamaged();
		}
		setDirection(82);
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		moveActor();
		return;
	}
	if (getX() >= RIGHT_EDGE)
	{
		if (dir < 90)
		{
			damageItself(10);
			actionsWhenDamaged();
		}
		setDirection(98);
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		moveActor();
		return;
	}

	// Check for inputs
	int key;
	double vSpeed = getVertSpeed();
	dir = getDirection();
	if (wPtr->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_SPACE:
		{
			if (getUnitsOfHolyWater() < 1)
			{
				moveActor();
				break;
			}
			// Add new holy water projectile
			double angleRad = (90 - dir) * PI / 180.0;
			double newX = SPRITE_HEIGHT * sin(angleRad) + getX();
			double newY = SPRITE_HEIGHT * cos(angleRad) + getY();
			wPtr->addActor(new HolyWaterProjectile(newX, newY, getDirection(), wPtr));

			wPtr->playSound(SOUND_PLAYER_SPRAY);
			decrementUnitsOfHolyWater();
			moveActor();
			break;
		}
		case KEY_PRESS_LEFT:
			if (dir >= 114)
			{
				moveActor();
				break;
			}
			setDirection(dir + 8);
			moveActor();
			break;
		case KEY_PRESS_RIGHT:
			if (dir <= 66)
			{
				moveActor();
				break;
			}			
			setDirection(dir - 8);
			moveActor();
			break;
		case KEY_PRESS_UP:
			if (vSpeed >= 5)
			{
				moveActor();
				break;
			}			
			setVertSpeed(vSpeed + 1);
			moveActor();
			break;
		case KEY_PRESS_DOWN:
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
	if (wPtr->isOverlapping(this, gr))
	{
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

	// Spin if overlap with ghost racer
	if (wPtr->isOverlapping(this, gr))
	{
		wPtr->playSound(SOUND_OIL_SLICK);
		// Going to spin ghost racer here
		int rand = randInt(5, 20);
		int grDir = gr->getDirection();
		int prospectiveClockwise = grDir - rand;
		int prospectiveCC = grDir + rand;
		if (prospectiveCC <= 120)	// If grDir can adjust CC, go CC
		{
			gr->setDirection(prospectiveCC);
		}
		else if (prospectiveClockwise >= 60)	// Else if grDir can adjust C, go C
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
	if (wPtr->isOverlapping(this, gr))
	{
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
	if (wPtr->isOverlapping(this, gr))
	{
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

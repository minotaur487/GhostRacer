#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
#include <cassert>	// remove after testing
using namespace std;

#define PI 3.14159265


	//	Helper Functions

bool isInBounds(double x, double y)
{
	return !(x < 0 || y < 0 || x > VIEW_WIDTH || y > VIEW_HEIGHT);
}

void spinClockwise(int delTheta, Actor* self)
{
	if (self->getDirection() >= delTheta)
		self->setDirection(self->getDirection() - delTheta);
	else
	{
		// 360 + dir - delTheta
		int angle = 360 + self->getDirection() - delTheta;
		self->setDirection(angle);
	}
}


///////////////////////////////////////////////
//
//	Actor Class
//
///////////////////////////////////////////////

void Actor::moveActor()
{
	GhostRacer* grPtr = getWorld()->getGhostRacer();
	double vSpeed = getVertSpeed() - grPtr->getVertSpeed();
	double hSpeed = getHorizSpeed();
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
//	Character Class
//
///////////////////////////////////////////////

bool Character::beSprayedIfAppropriate() {
	if (this != getWorld()->getGhostRacer())	// check ghost racer bit and inline	!!!
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
//	Zombie Pedestrian Class
//
///////////////////////////////////////////////

ZombiePedestrian::ZombiePedestrian(double startX, double startY, StudentWorld* wPtr)
	: Pedestrian(IID_ZOMBIE_PED, startX, startY, 3.0), ticksTilGrunt(0)
{
	setVertSpeed(-4);
	setCollisionWorthy(true);
	setWorld(wPtr);
	// Movement plan of 0?
}

void ZombiePedestrian::doSomething()
{
	if (!isAlive())
		return;

	// If overlap with ghost racer
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();
	if (wPtr->isOverlapping(this, grPtr))
	{
		grPtr->damageItself(5);
		damageItself(2);
		actionsWhenDamaged();
		return;
	}

	// Move zombie pedestrian
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

	moveActor();

	// Determine movement plan
	if (getMovementPlanDistance() > 0)
	{
		decrementMovementPlanDist();
		return;
	}
	else
	{
		double newHSpeed = randInt(-3, 3);
		while (newHSpeed == 0)
			newHSpeed = randInt(-3, 3);
		setHorizSpeed(newHSpeed);		// Check speed != 0		!!!
		setMovementPlanDist(randInt(4, 32));
		if (getHorizSpeed() < 0)
			setDirection(180);
		else if (getHorizSpeed() > 0)
			setDirection(0);
	}
}

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
	// Movement plan of 0?
}

void HumanPedestrian::doSomething()
{
	if (!isAlive())
		return;

	// If overlap with ghost racer
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();
	if (wPtr->isOverlapping(this, grPtr))
	{
		grPtr->setLife(false);
		return;
	}

	// Move human pedestrian
	moveActor();

	// Determine movement plan
	decrementMovementPlanDist();
	if (getMovementPlanDistance() > 0)
		return;
	else
	{
		double newHSpeed = randInt(-3, 3);
		while (newHSpeed == 0)
			newHSpeed = randInt(-3, 3);
		setHorizSpeed(newHSpeed);		// Check speed != 0		!!!
		setMovementPlanDist(randInt(4, 32));
		if (getHorizSpeed() < 0)
			setDirection(180);
		else if (getHorizSpeed() > 0)
			setDirection(0);
	}
}

bool HumanPedestrian::beSprayedIfAppropriate()
{
 	setHorizSpeed(getHorizSpeed() * -1);
	if (getHorizSpeed() < 0)				//	Duplicate code with line 98	!!!
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
	double dir = getDirection() * PI / 180.0;			// Some stuff in algo seems superfluous				!!!
	double delX = cos(dir) * maxShiftPerTick;
	double curX = getX();
	double curY = getY();
	moveTo(curX + delX, curY);
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
			if (!isAlive())
			{
				wPtr->playSound(SOUND_PLAYER_DIE);			// NOT SURE IF I SHOULD RETURN HERE...SAME FOR RIGHT_EDGE
			}
		}
		setDirection(82);
		wPtr->playSound(SOUND_VEHICLE_CRASH);
		moveActor();
		return;
	}
	if (getX() >= RIGHT_EDGE)		// CODE IS VERY SIMILAR TO LEFT EDGE			!!!
	{
		if (dir < 90)
		{
			damageItself(10);
			if (!isAlive())
			{
				wPtr->playSound(SOUND_PLAYER_DIE);
			}
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
	// Move consumable
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
	: Environmentals(IID_OIL_SLICK, startX, startY, 0, size)
{
	setWorld(wPtr);
	setVertSpeed(-4);
	setCollisionWorthy(false);
}

void OilSlick::doSomething()
{
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();
	// Move oil slick
	moveActor();

	// Spin if overlap with ghost racer
	if (wPtr->isOverlapping(this, grPtr))
	{
		wPtr->playSound(SOUND_OIL_SLICK);
		// Going to spin ghost racer here					TEST I actually can't tell by visuals if it works properly		!!!
		int rand = randInt(5, 20);
		int grDir = grPtr->getDirection();
		int prospectiveClockwise = grDir - rand;
		int prospectiveCC = grDir + rand;
		if (prospectiveCC <= 120)	// If grDir can adjust CC, go CC
		{
			grPtr->setDirection(prospectiveCC);
		}
		else if (prospectiveClockwise >= 60)	// Else if grDir can adjust C, go C
		{
			grPtr->setDirection(prospectiveClockwise);
		}
	}
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
	setWorld(wPtr);
	setCollisionWorthy(false);
} // CHECK DEPTH

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

bool HolyWaterGoodie::beSprayedIfAppropriate()	// SAME CODE AS HEALING GOODIE
{
	setLife(false);
	return true;
}


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

void ZombieCab::doSomething()
{
	if (!isAlive())
		return;

	// If overlap with ghost racer
	StudentWorld* wPtr = getWorld();
	GhostRacer* grPtr = wPtr->getGhostRacer();
	if (wPtr->isOverlapping(this, grPtr))
	{
		if (hasDamagedGhostRacer())
		{
			moveActor();
			return;
		}
		else
		{
			wPtr->playSound(SOUND_VEHICLE_CRASH);
			grPtr->damageItself(20);	// do steps that follow when it gets damaged
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
	}

	// Move zombie cab		// this is the step 3 mentioned earlier			!!!
	moveActor();

	double zombieCabX = getX();
	const int* curLane;
	if (zombieCabX >= R_LANE[0] && zombieCabX <= R_LANE[1])
		curLane = R_LANE;
	else if (zombieCabX >= L_LANE[0] && zombieCabX <= L_LANE[1])
		curLane = L_LANE;
	else
		curLane = M_LANE;
	Actor* closestCollisionWorthyActor = wPtr->findClosestCollisionWorthyActor(curLane, BOTTOM);
	bool CWActorPresent = false;
	if (closestCollisionWorthyActor != nullptr)
		CWActorPresent = true;
	if (getVertSpeed() > grPtr->getVertSpeed() && CWActorPresent)
	{
		double delY = closestCollisionWorthyActor->getY() - getY();
		assert(delY > 0);
		if (delY < 96)
		{
			setVertSpeed(getVertSpeed() - 0.5);
			return;
		}
	}
	closestCollisionWorthyActor = wPtr->findClosestCollisionWorthyActor(curLane, TOP);
	CWActorPresent = false;
	if (closestCollisionWorthyActor != nullptr)
		CWActorPresent = true;
	if (getVertSpeed() <= grPtr->getVertSpeed() && CWActorPresent)
	{
		double delY = getY() - closestCollisionWorthyActor->getY();
		assert(delY > 0);
		if (delY < 96 && closestCollisionWorthyActor != grPtr)	// cover issue if two are there and closer one is ghost racer
		{
			setVertSpeed(getVertSpeed() + 0.5);
			return;
		}
	}

	// Determine movement plan
	decrementMovementPlanDist();
	if (getMovementPlanDistance() > 0)
		return;
	else
	{
		setMovementPlanDist(randInt(4, 32));
		setVertSpeed(getVertSpeed() + randInt(-2, 2));
	}
}

bool ZombieCab::beSprayedIfAppropriate()	// SAME CODE AS HEALING GOODIE
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
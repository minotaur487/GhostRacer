#ifndef Character_H_
#define Character_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth)
		: GraphObject(imageID, startX, startY, dir, size, depth), m_alive(true) {
		setHorizSpeed(0.0);
	}
	virtual ~Actor() {}

		// Functions that do

	virtual void doSomething() = 0;
	void setVertSpeed(double speed) { m_param.m_vertSpeed = speed; }
	void setHorizSpeed(double speed) { m_param.m_horizSpeed = speed; }
	void setLife(bool life) { m_alive = life; }
	void setWorld(StudentWorld* ptr) { m_param.m_worldPtr = ptr; }
	virtual void moveActor();
	void setCollisionWorthy(bool flag) { m_param.m_collisionWorthy = flag; }
	virtual bool beSprayedIfAppropriate() {	return false; };
	
		// Functions that get/return

	bool isInBounds(double x, double y) const;
	double getVertSpeed() const { return m_param.m_vertSpeed; }
	double getHorizSpeed() const { return m_param.m_horizSpeed; }
	bool isCollisionWorthy() const { return m_param.m_collisionWorthy; }
	virtual bool isAlive() const { return m_alive; }
	StudentWorld* getWorld() const { return m_param.m_worldPtr; }
private:
		//	struct
	struct additionalParam
	{
		double m_vertSpeed;
		double m_horizSpeed;
		bool m_collisionWorthy;
		StudentWorld* m_worldPtr;
	};

		// Data members
	bool m_alive;
	additionalParam m_param;
};

inline
bool Actor::isInBounds(double x, double y) const
{
	return !(x < 0 || y < 0 || x > VIEW_WIDTH || y > VIEW_HEIGHT);
}

class HolyWaterProjectile : public Actor
{
public:
	HolyWaterProjectile(double startX, double startY, int dir, StudentWorld* wPtr);
	~HolyWaterProjectile() {}

		// Functions that do

	virtual void doSomething();
	void addTravelDist(int dist) { m_travelDist += dist; }

		// Functions that get/return

	int getDistTravelled() { return m_travelDist; }
private:
	int m_travelDist;
};

class Character : public Actor
{
public:
	Character(int imageID, double startX, double startY, int hitPoints,  int dir = 0, double size = 1.0,
		unsigned int depth = 0) : Actor(imageID, startX, startY, dir, size, depth), m_hitPoints(hitPoints) {}
	virtual ~Character() {}

		// Functions that do

	void damageItself(int hitPoints) { m_hitPoints -= hitPoints; }
	virtual bool beSprayedIfAppropriate();
	void setHitPoints(int hitPoints)
	{ 
		if (hitPoints >= 100)
		{
			m_hitPoints = 100;
			return;
		}
		else
			m_hitPoints = hitPoints;
	}

		// Functions that get/return

	virtual bool isAlive() {
		if (getHitPoints() <= 0)
			setLife(false);
		return Actor::isAlive();
	}
	int getHitPoints() const { return m_hitPoints; }
private:
		// data members
	int m_hitPoints;

		// functions
	virtual void actionsWhenDamaged() {}
};

class Autonomous : public Character
{
public:
	Autonomous(int imageID, double startX, double startY, int hitpoints, int dir, double size)
		: Character(imageID, startX, startY, hitpoints, dir, size), m_movementPlanDistance(0) {}
	virtual ~Autonomous() {}

		// Functions that do

	void decrementMovementPlanDist() { m_movementPlanDistance--; }
	void setMovementPlanDist(int dist) { m_movementPlanDistance = dist; }
	virtual void doSomething();

		// Functions that get

	int getMovementPlanDistance() const { return m_movementPlanDistance; }
private:
		// Data members
	int m_movementPlanDistance;
		// functions
	virtual void doDifferentiatedOverlappingAction() = 0;
	virtual void doDifferentiatedMovement();
	virtual void doDifferentiatedIntermediateSteps() = 0;
	virtual void doDifferentiatedNewMovement();
};

class ZombieCab : public Autonomous
{
public:
	ZombieCab(double startX, double startY, double vSpeed, StudentWorld* wPtr);
	virtual ~ZombieCab() {}

		// functions that do

	virtual bool beSprayedIfAppropriate();
	void indicateDamagedGhostRacer() { m_hasDamagedGhostRacer = true; }

		// functions that get

	bool hasDamagedGhostRacer() const { return m_hasDamagedGhostRacer; }
private:
		// data members
	bool m_hasDamagedGhostRacer;

		// functions
	virtual void actionsWhenDamaged();
	virtual void doDifferentiatedIntermediateSteps();
	virtual void doDifferentiatedOverlappingAction();
	virtual void doDifferentiatedNewMovement();
};

class Pedestrian : public Autonomous
{
public:
	Pedestrian(int imageID, double startX, double startY, double size)
		: Autonomous(imageID, startX, startY, 2, 0, size) {}
	virtual ~Pedestrian() {}
private:
	virtual void doDifferentiatedIntermediateSteps() {}
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(double startX, double startY, StudentWorld* wPtr);
	virtual ~ZombiePedestrian() {}

		// Functions that do

	virtual bool beSprayedIfAppropriate();
	virtual void moveActor();
private:
		// data members
	int ticksTilGrunt;
		
		// functions
	virtual void actionsWhenDamaged();
	virtual void doDifferentiatedOverlappingAction();
	virtual void doDifferentiatedMovement();
};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(double startX, double startY, StudentWorld* wPtr);
	virtual ~HumanPedestrian() {}

		// Functions that do

	virtual bool beSprayedIfAppropriate();
private:
	virtual void doDifferentiatedOverlappingAction();
};

class GhostRacer : public Character
{
public:
	GhostRacer(StudentWorld* wPtr);
	virtual ~GhostRacer() {}

		// Functions that do

	virtual void doSomething();
	virtual void moveActor();
	virtual void actionsWhenDamaged();
	void addUnitsOfHolyWater(int units) { m_unitsOfHolyWater += units; }
	void decrementUnitsOfHolyWater() { m_unitsOfHolyWater--; }

		// Functions that get/return

	int getUnitsOfHolyWater() const { return m_unitsOfHolyWater; }
private:
	int m_unitsOfHolyWater;
};

class Consumables : public Actor
{
public:
	Consumables(int imageID, double startX, double startY, int dir, double size, unsigned int depth = 2)
		: Actor(imageID, startX, startY, dir, size, depth) {}
	virtual ~Consumables() {}

		// Functions that do

	virtual void doSomething();
private:
	virtual void doActivity(GhostRacer* gr) = 0;
};

class HealingGoodie : public Consumables
{
public:
	HealingGoodie(double startX, double startY, StudentWorld* wPtr);
	virtual ~HealingGoodie() {}

		// Functions that do

	virtual bool beSprayedIfAppropriate();
private:
	virtual void doActivity(GhostRacer* gr);
};

class HolyWaterGoodie : public Consumables
{
public:
	HolyWaterGoodie(double startX, double startY, StudentWorld* wPtr);
	virtual ~HolyWaterGoodie() {}

		// Functions that do

	virtual bool beSprayedIfAppropriate();
private:
	virtual void doActivity(GhostRacer* gr);
};

class Soul : public Consumables
{
public:
	Soul(double startX, double startY, StudentWorld* wPtr);
	virtual ~Soul() {}

private:
	virtual void doActivity(GhostRacer* gr);
	void spinClockwise(int delTheta, Actor* self);
};

inline
void Soul::spinClockwise(int delTheta, Actor* self)
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

class OilSlick : public Consumables
{
public:
	OilSlick(double startX, double startY, double size, StudentWorld* wPtr);
	virtual ~OilSlick() {}

private:
	virtual void doActivity(GhostRacer* gr);
};

class BorderLine : public Actor
{
public:
	BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr);
	virtual ~BorderLine() {}

		// Functions that do

	virtual void doSomething();
};

#endif // Character_H_

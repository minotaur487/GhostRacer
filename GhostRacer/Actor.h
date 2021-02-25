#ifndef Character_H_
#define Character_H_

#include "GraphObject.h"

class StudentWorld;
//#include "StudentWorld.h" // using getGhostRacer for beSprayed bool func CRASHING??????? WITH THIS LINE

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth)
		: GraphObject(imageID, startX, startY, dir, size, depth), m_alive(true) {
		setActivatedBool(false);
		setHorizSpeed(0);
	}
	virtual ~Actor() {}

		// Functions that do

	virtual void doSomething() = 0;
	//virtual void applyImpact() { return; };	// FIND BETTER WAY			!!!
	void setVertSpeed(int speed) { m_param.m_vertSpeed = speed; }
	void setHorizSpeed(int speed) { m_param.m_horizSpeed = speed; }
	void setLife(bool life) { m_alive = life; }
	void setWorld(StudentWorld* ptr) { m_param.m_worldPtr = ptr; }
	void setCollisionWorthy(bool flag) { m_param.m_collisionWorthy = flag; }
	void setActivatedBool(bool flag) { m_param.m_canBeActivated = flag; }
	virtual bool beSprayedIfAppropriate() {	return false;	};
	
		// Functions that get/return

	int getVertSpeed() const { return m_param.m_vertSpeed; }
	int getHorizSpeed() const { return m_param.m_horizSpeed; }
	bool isSprayable() const { return m_param.m_isSprayable; }
	bool isCollisionWorthy() const { return m_param.m_collisionWorthy; }
	virtual bool isAlive() const { return m_alive; }
	bool canBeActivated() const { return m_param.m_canBeActivated; }
	StudentWorld* getWorld() const { return m_param.m_worldPtr; }
private:
		//	struct
	struct additionalParam
	{
		bool m_canBeActivated;
		int m_vertSpeed;
		int m_horizSpeed;
		bool m_collisionWorthy;
		StudentWorld* m_worldPtr;
		bool m_isSprayable;
	};

		// Data members
	bool m_alive;
	additionalParam m_param;
};

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

	void setHitPoints(int hitPoints) { m_hitPoints = hitPoints; }
	void damageItself(int hitPoints) { m_hitPoints -= hitPoints; }
	virtual bool beSprayedIfAppropriate();
	void updateLifeStatus() {
		if (getHitPoints() <= 0)
			setLife(false);	// Maybe declare as inline			!!!
	}

	// Functions that get/return

	//virtual bool isAlive() { return getHitPoints() > 0 && m_alive; }
	int getHitPoints() const { return m_hitPoints; }

private:
	int m_hitPoints;
};

class Pedestrian : public Character
{
public:
	Pedestrian(int imageID, double startX, double startY, double size)
		: Character(imageID, startX, startY, 2, 0, size), m_movementPlanDistance(0) {}
	virtual ~Pedestrian() {}

	// Functions that do

	void decrementMovementPlanDist() { m_movementPlanDistance--; }
	void setMovementPlanDist(int dist) { m_movementPlanDistance = dist; }

	// Functions that get

	int getMovementPlanDistance() const { return m_movementPlanDistance; }
private:
	// Data members
	int m_movementPlanDistance;
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(double startX, double startY, StudentWorld* wPtr);
	virtual ~ZombiePedestrian() {}

	// Functions that do

	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
private:
	int ticksTilGrunt;
};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(double startX, double startY, StudentWorld* wPtr);
	virtual ~HumanPedestrian() {}

	// Functions that do

	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
};

class GhostRacer : public Character
{
public:
	GhostRacer(StudentWorld* wPtr);	// Ghost racer doesn't seem to have a horiz speed		!!!
	virtual ~GhostRacer() {}

	// Functions that do

	virtual void doSomething();
	void moveGR();
	void addUnitsOfHolyWater(int units) { m_unitsOfHolyWater += units; }
	void decrementUnitsOfHolyWater() { m_unitsOfHolyWater--; }

	// Functions that get/return
	int getUnitsOfHolyWater() const { return m_unitsOfHolyWater; }

private:
	int m_unitsOfHolyWater;
};

class Goodies : public Actor
{
public:
	Goodies(int imageID, double startX, double startY, int dir, double size, unsigned int depth = 2)
		: Actor(imageID, startX, startY, dir, size, depth) {}
	virtual ~Goodies() {}
	//virtual bool isActive() = 0;			// FOR CONSUMABLES I THINK				!!!
};

class Consumables : public Goodies
{
public:
	Consumables(int imageID, double startX, double startY, int dir, double size)
		: Goodies(imageID, startX, startY, dir, size) {}
	virtual ~Consumables() {}
};

class Soul : public Consumables
{
public:
	Soul(double startX, double startY, StudentWorld* wPtr);
	virtual ~Soul() {}
	virtual void doSomething();
};

class Environmentals : public Goodies
{
public:
	Environmentals(int imageID, double startX, double startY, int dir, double size)
		// Check if oil slick has depth 2 aka is a goodie								!!!
		: Goodies(imageID, startX, startY, dir, size) {}
	virtual ~Environmentals() {}
};

class BorderLine : public Environmentals
{
public:
	BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr);
	virtual ~BorderLine() {}
	virtual void doSomething();
};

class OilSlick : public Environmentals
{
public:
	OilSlick(double startX, double startY, double size, StudentWorld* wPtr);
	virtual ~OilSlick() {}
	virtual void doSomething();
};

#endif // Character_H_

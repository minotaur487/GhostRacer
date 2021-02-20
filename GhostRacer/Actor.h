#ifndef Character_H_
#define Character_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth)
		: GraphObject(imageID, startX, startY, dir, size, depth), m_alive(true) {}
	virtual ~Actor() {}

		// Functions that do

	virtual void doSomething() = 0;
	void setVertSpeed(int speed) { m_param.m_vertSpeed = speed; }
	void setHorizSpeed(int speed) { m_param.m_horizSpeed = speed; }
	void setLife(bool life) { m_alive = life; }
	void setWorld(StudentWorld* ptr) { m_param.m_worldPtr = ptr; }
	void setCollidable(bool flag) { m_param.m_collidable = flag; }
	void setAffectedByHW(bool flag) { m_param.m_affectedByHW = flag; }
	void setHealable(bool flag) { m_param.m_healable = flag; }
	void setSpinnable(bool flag) { m_param.m_spinnable = flag; }
	void doOtherCircumstances();
	
		// Functions that get/return

	int getVertSpeed() const { return m_param.m_vertSpeed; }
	int getHorizSpeed() const { return m_param.m_horizSpeed; }
	bool isCollidable() const { return m_param.m_collidable; }
	virtual bool isAlive() const { return m_alive; }
	bool isSpinnable() const { return m_param.m_spinnable; }
	bool isHealable() const { return m_param.m_healable; }
	bool isAffectedByHW() const { return m_param.m_affectedByHW; }
	StudentWorld* getWorld() const { return m_param.m_worldPtr; }
private:
		//	Functions and struct
	virtual void doSpin() { return; };
	virtual void doCollision() { return; };		// Not done for GR or BorderLines yet		!!!
	virtual void doHW() { return; };
	virtual void doHeal() { return; };
	struct additionalParam
	{
		int m_vertSpeed;
		int m_horizSpeed;
		bool m_collidable;
		bool m_spinnable;
		bool m_affectedByHW;
		bool m_healable;
		StudentWorld* m_worldPtr;
	};

		// Data members
	bool m_alive;
	additionalParam m_param;
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

class HumanPedestrian : public Character
{
public:
	HumanPedestrian(double startX, double startY, StudentWorld* wPtr);
	virtual ~HumanPedestrian() {}

	// Functions that do

	virtual void doSomething();
	void decrementMovementPlanDist() { m_movementPlanDistance--; }
	void setMovementPlanDist(int dist) { m_movementPlanDistance = dist; }

	// Functions that get

	int getMovementPlanDistance() const { return m_movementPlanDistance; }
private:
		// Functions
	virtual void doHW();

		// Data members
	int m_movementPlanDistance;
};

class GhostRacer : public Character
{
public:
	GhostRacer(StudentWorld* wPtr);	// Ghost racer doesn't seem to have a horiz speed		!!!
	virtual ~GhostRacer() {}

	// Functions that do

	virtual void doSomething();
	void moveGR();
	void setUnitsOfHolyWater(int units) { m_unitsOfHolyWater = units; }

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

#endif // Character_H_

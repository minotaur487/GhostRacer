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
	void setVertSpeed(double speed) { m_param.m_vertSpeed = speed; }
	void setHorizSpeed(double speed) { m_param.m_horizSpeed = speed; }
	void setLife(bool life) { m_alive = life; }
	void setWorld(StudentWorld* ptr) { m_param.m_worldPtr = ptr; }
	void setCollidable(bool flag) { m_param.m_collidable = flag; }
	void setAffectedByHW(bool flag) { m_param.m_affectedByHW = flag; }
	void setHealable(bool flag) { m_param.m_healable = flag; }
	void setSpinnable(bool flag) { m_param.m_spinnable = flag; }
	
		// Functions that get/return

	double getVertSpeed() const { return m_param.m_vertSpeed; }
	double getHorizSpeed() const { return m_param.m_horizSpeed; }
	bool isCollidable() const { return m_param.m_collidable; }
	virtual bool isAlive() const { return m_alive; }
	bool isSpinnable() const { return m_param.m_spinnable; }
	bool isHealable() const { return m_param.m_healable; }
	bool isAffectedByHW() const { return m_param.m_affectedByHW; }
	StudentWorld* getWorld() const { return m_param.m_worldPtr; }
private:
	struct additionalParam
	{
		double m_vertSpeed;
		double m_horizSpeed;
		bool m_collidable;
		bool m_spinnable;
		bool m_affectedByHW;
		bool m_healable;
		StudentWorld* m_worldPtr;
	};
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

	// Functions that get/return

	virtual bool isAlive() { return getHitPoints() > 0; }
	int getHitPoints() const { return m_hitPoints; }

private:
	int m_hitPoints;
};

class GhostRacer : public Character
{
public:
	GhostRacer(StudentWorld* wPtr);	// Ghost racer doesn't seem to have a horiz speed		!!!
	virtual ~GhostRacer() {}

	// Functions that do

	virtual void doSomething();
	void moveGR();
	void incrementSoulsSaved() { m_soulsSaved++; }
	void decrementSoulsSaved() { m_soulsSaved--; }

	// Functions that get/return

	int getUnitsOfHolyWater() const { return m_unitsOfHolyWater; }
	int getNumOfSoulsSaved() const { return m_soulsSaved; }

private:
	int m_soulsSaved;
	int m_unitsOfHolyWater;
};

class Goodies : public Actor
{
public:
	Goodies(int imageID, double startX, double startY, int dir, double size, GhostRacer* grPtr, unsigned int depth = 2)
		: Actor(imageID, startX, startY, dir, size, depth), m_gRacerPtr(grPtr) {}
	virtual ~Goodies() {};
	//virtual bool isActive() = 0;			// FOR CONSUMABLES I THINK				!!!

	// Functions that get/return

	GhostRacer* getGhostRacer() const { return m_gRacerPtr; }

private:
	GhostRacer* m_gRacerPtr;
};

class Environmentals : public Goodies
{
public:
	Environmentals(int imageID, double startX, double startY, int dir, double size, GhostRacer* grPtr)
		// Check if oil slick has depth 2 aka is a goodie								!!!
		: Goodies(imageID, startX, startY, dir, size, grPtr) {}
	virtual ~Environmentals() {};
};

class BorderLine : public Environmentals
{
public:
	BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr, GhostRacer* grPtr);
	virtual ~BorderLine() {}
	virtual void doSomething();
};

#endif // Character_H_

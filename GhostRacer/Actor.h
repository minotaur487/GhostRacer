#ifndef Character_H_
#define Character_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, double horizSpeed,
		double vertSpeed, bool collidable, StudentWorld* wPtr, bool healable, bool spinnable, bool affectedByHolyWater) : 
		GraphObject(imageID, startX, startY, dir, size, depth), m_horizSpeed(horizSpeed), m_vertSpeed(vertSpeed), 
		m_collidable(collidable), m_worldPtr(wPtr), m_alive(true), m_healable(healable), m_spinnable(spinnable),
		m_affectedByHW(affectedByHolyWater) {}
	virtual ~Actor() {}

		// Functions that do

	virtual void doSomething() = 0;
	double setVertSpeed(double speed) { m_vertSpeed = speed; }
	double setHorizSpeed(double speed) { m_vertSpeed = speed; }
	void setLife(bool life) { m_alive = life; }
	
		// Functions that get/return

	double getVertSpeed() const { return m_vertSpeed; }
	double getHorizSpeed() const { return m_horizSpeed; }
	bool isCollidable() const { return m_collidable; }
	bool isAlive() const { return m_alive; }
	StudentWorld* getWorld() const { return m_worldPtr; }

private:
	bool m_alive;
	double m_vertSpeed;
	double m_horizSpeed;
	bool m_collidable;
	bool m_spinnable;
	bool m_affectedByHW;
	bool m_healable;
	StudentWorld* m_worldPtr;
};

class Character : public Actor
{
public:
	Character(int imageID, double startX, double startY, double horizSpeed, double vertSpeed, int hitPoints,
		StudentWorld* wPtr, bool healable, bool spinnable, bool affectedByHolyWater,  int dir = 0, double size = 1.0,
		unsigned int depth = 0) : Actor(imageID, startX, startY, dir, size, depth, horizSpeed, 
			vertSpeed, true, wPtr, healable, spinnable, affectedByHolyWater), m_hitPoints(hitPoints) {}
	// Check blanket statement of is collidable
	virtual ~Character() {}

	// Functions that do

	int setHitPoints(int hitPoints) { m_hitPoints = hitPoints; }
	int damageItself(int hitPoints) { m_hitPoints -= hitPoints; }

	// Functions that get/return

	int getHitPoints() const { return m_hitPoints; }
	//bool isAlive() const { return m_hitPoints > 0; }	// Problematic for determining consumables as alive			!!!

private:
	int m_hitPoints;
};

class GhostRacer : public Character
{
public:
	GhostRacer(StudentWorld* wPtr) : Character(IID_GHOST_RACER, 128, 32, 0, 0, 100, wPtr, 
		90, 4.0, true, false, true), m_unitsOfHolyWater(10) {}	// Ghost racer doesn't seem to have a horiz speed		!!!
	virtual ~GhostRacer() {}
	virtual void doSomething();

	// Functions that get/return

	int getUnitsOfHolyWater() const { return m_unitsOfHolyWater; }

private:
	int m_unitsOfHolyWater;
};

class Goodies : public Actor
{
public:
	Goodies(int imageID, double startX, double startY, int dir, double size, double horizSpeed, double vertSpeed, 
		StudentWorld* wPtr, GhostRacer* grPtr, bool affectedByHolyWater, unsigned int depth = 2)
		// Check if Goodies are supposed to be "Alive"																!!!
		: Actor(imageID, startX, startY, dir, size, depth, horizSpeed, vertSpeed, false, wPtr, false, false, affectedByHolyWater), 
		m_gRacerPtr(grPtr) {}
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
	Environmentals(int imageID, double startX, double startY, int dir, double size,
		double horizSpeed, double vertSpeed, StudentWorld* wPtr, GhostRacer* grPtr)
		// Check if oil slick has depth 2 aka is a goodie								!!!
		: Goodies(imageID, startX, startY, dir, size, horizSpeed, vertSpeed, wPtr, grPtr, false) {}
	virtual ~Environmentals() {};
};

class BorderLine : public Environmentals
{
public:
	BorderLine(int imageID, double startX, double startY, StudentWorld* wPtr, GhostRacer* grPtr)
		// Contradiction with depth, says 2 and 1 at different parts in spec			!!!
		: Environmentals(imageID, startX, startY, 0, 2.0, 0, -4, wPtr, grPtr) {} // CHECK DEPTH
	virtual ~BorderLine() {}
	virtual void doSomething();
};

#endif // Character_H_

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class GameObject : public GraphObject
{
public:
	GameObject(int imageID, double startX, double startY, int dir, double size, unsigned int depth)
		: GraphObject(imageID, startX, startY, dir, size, depth) {}
	virtual ~GameObject() {}
	virtual void doSomething() = 0;
private:
	struct Location {};
	Location m_location;
};

class Actor : public GameObject
{
public:
	Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
		: GameObject(imageID, startX, startY, dir, size, depth) {}
	virtual ~Actor() {}
};

class GhostRacer : public Actor
{
public:
	GhostRacer() : Actor(IID_GHOST_RACER, 128, 32, 90, 4.0) {}
	virtual ~GhostRacer() {}
	virtual void doSomething() {}
private:
};

//class Goodies : public GameObject
//{
//public:
//	Goodies(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth)
//		: GameObject(imageID, startX, startY, dir, size, depth) {}
//	virtual ~Goodies() {};
//	virtual bool isActive() = 0;
//};
//
//class Environmentals : public Goodies
//{
//public:
//	Environmentals(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth)
//		: Goodies(imageID, startX, startY, dir, size, depth) {}
//	virtual ~Environmentals() {};
//};
//
//class BorderLine : public Environmentals
//{
//public:
//	BorderLine(int imageID, double startX, double startY, int dir = 0, double size = 1.0)
//		: Environmentals(IID_WHITE_BORDER_LINE, startX, startY, dir, size, 2) {} // CHECK DEPTH
//	virtual ~BorderLine() {}
//};

#endif // ACTOR_H_

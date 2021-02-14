#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_ghostRacer = new GhostRacer;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    m_ghostRacer->doSomething();
    return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
    delete m_ghostRacer;
}

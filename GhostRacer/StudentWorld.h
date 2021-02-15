#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
// Added by me
#include "Actor.h"
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Character.h, and Character.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

private:
    GhostRacer* m_ghostRacer;
    vector<Actor*> m_actorList;
};

#endif // STUDENTWORLD_H_

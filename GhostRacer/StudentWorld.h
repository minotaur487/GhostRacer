#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
// Added by me
#include "Actor.h"
#include <vector>
using namespace std;

    // Student World Constants                  !!! I don't know if I'm allowed to do this since I can't add to the actual
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;       // header file for this
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
const int N = VIEW_HEIGHT / SPRITE_HEIGHT;    // N number of border lines on each side

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

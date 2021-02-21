#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
// Added by me
#include "Actor.h"
#include <list>
using namespace std;

    // Student World Constants
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
const int N = VIEW_HEIGHT / SPRITE_HEIGHT;    // N number of yellow border lines on each side
const int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);      // M number of white border lines on each side

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // Helper functions
    void deleteDeadActors();
    void addNewActors();

    // Functions that do

    void incrementSoulsSaved() { m_soulsSaved++; }
    void decrementSoulsSaved() { m_soulsSaved--; }

    // Get functions

    Actor* getGhostRacer() const { return m_ghostRacer; }     // Check if this can be ghostRacer        !!!
    int getNumOfSoulsSaved() const { return m_soulsSaved; }

private:
    Actor* m_ghostRacer;
    list<Actor*> m_actorList;
    double m_lastBDY;
    int m_soulsSaved;
};

#endif // STUDENTWORLD_H

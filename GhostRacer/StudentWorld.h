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

    // Functions that do

    void incrementSoulsSaved() { m_soulsSaved++; }
    void decrementSoulsSaved() { m_soulsSaved--; }
    bool executeProjectileImpact(Actor* projectile);
    void addActor(Actor* actor) { m_actorList.push_back(actor); }
    void addUnitsOfHolyWater(int units) { m_unitsOfHolyWater += units; }
    void decrementUnitsOfHolyWater() { m_unitsOfHolyWater--; }

    // Get functions

    Actor* getGhostRacer() const { return m_ghostRacer; }     // Check if this can be ghostRacer        !!!
    int getNumOfSoulsSaved() const { return m_soulsSaved; }
    int getUnitsOfHolyWater() const { return m_unitsOfHolyWater; }

private:
        // Helper functions
    void deleteDeadActors();
    void addNewActors();

        // Data Members
    Actor* m_ghostRacer;
    list<Actor*> m_actorList;
    double m_lastBDY;
    int m_soulsSaved;
    int m_unitsOfHolyWater;
};

#endif // STUDENTWORLD_H

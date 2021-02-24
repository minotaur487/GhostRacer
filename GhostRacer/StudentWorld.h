#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
// Added by me
#include <list>
#include <cmath>
using namespace std;

#include "Actor.h"  // Because of the inline functions
//class Actor;
//class GhostRacer;

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

    bool isOverlapping(const Actor* a1, const Actor* a2) const;
    void incrementSoulsSaved() { m_soulsSaved++; }
    void decrementSoulsSaved() { m_soulsSaved--; }
    bool executeProjectileImpact(Actor* projectile);
    void addActor(Actor* actor) { m_actorList.push_back(actor); }

    // Get functions

    GhostRacer* getGhostRacer() const { return m_ghostRacer; }     // Check if this can be ghostRacer        !!!
    int getNumOfSoulsSaved() const { return m_soulsSaved; }

private:
        // Helper functions
    void deleteDeadActors();
    void addNewActors();

        // Data Members
    GhostRacer* m_ghostRacer;
    list<Actor*> m_actorList;
    double m_lastBDY;
    int m_soulsSaved;
};

inline
bool StudentWorld::isOverlapping(const Actor* i, const Actor* j) const
{
    double deltaX = abs(i->getX() - j->getX());
    double deltaY = abs(i->getY() - j->getY());
    double radSum = i->getRadius() + j->getRadius();
    if (deltaX < radSum * 0.25 && deltaY < radSum * 0.6)
        return true;
    return false;
}

#endif // STUDENTWORLD_H

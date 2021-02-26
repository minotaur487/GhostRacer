#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"  // For isOverlapping, considering changing cuz this would take the whole file
#include <string>
#include <sstream>
#include <iomanip>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

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
    m_soulsSaved = 0;
    m_bonusPoints = 5000;
    m_ghostRacer = new GhostRacer(this);

    // Initialize yellow border lines
    for (int j = 0; j < N; j++)
    {
        int y = j * SPRITE_HEIGHT;
        m_actorList.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, y, this));
        m_actorList.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, y, this));
    }

    // Initialize white border lines
    for (int j = 0; j < M; j++)
    {
        int y = j * (4 * SPRITE_HEIGHT);
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ML_EDGE, y, 
            this));
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, MR_EDGE, y, 
            this));
    }
    // Save last white border line added
    Actor* lastWB = *(--m_actorList.end());
    m_lastBDY = lastWB->getY();

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*>::iterator it;
    // Have each active actor do something
    m_ghostRacer->doSomething();
    if (!m_ghostRacer->isAlive())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    else if (getNumOfSoulsSaved() >= getLevel() * 2 + 5)
    {
        //AWARD BONUS POINTS                                            !!!
        return GWSTATUS_FINISHED_LEVEL;
    }
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        if ((*it)->isAlive())
        {
            (*it)->doSomething();
            if (!m_ghostRacer->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            else if (getNumOfSoulsSaved() >= getLevel() * 2 + 5)
            {
                //AWARD BONUS POINTS                                            !!!
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    // Delete dead actors
    deleteDeadActors();
    
    // Add necessary new objects
    addNewActors();

    // Update status text
    m_bonusPoints--;
    generateStatistics();
    // !!!  //

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_ghostRacer;
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end();)
    {
        delete* it;
        it = m_actorList.erase(it);
    }
}

void StudentWorld::deleteDeadActors()
{
    // Delete dead actors
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end();)
    {
        if (!(*it)->isAlive())
        {
            delete* it;
            it = m_actorList.erase(it);
        }
        it++;
    }
}

void StudentWorld::addNewActors()
{
    // Add border lines
    int newBorderY = VIEW_HEIGHT - SPRITE_HEIGHT;
    double lastWLSpeed = -4 - m_ghostRacer->getVertSpeed();
    m_lastBDY = m_lastBDY + lastWLSpeed;
    double deltaY = newBorderY - m_lastBDY;

    if (deltaY >= SPRITE_HEIGHT)
    {
        m_actorList.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE,
            newBorderY, this));
        m_actorList.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE,
            newBorderY, this));
    }
    if (deltaY >= 4.0 * SPRITE_HEIGHT)
    {
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ML_EDGE,
            newBorderY, this));
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, MR_EDGE,
            newBorderY, this));

        // Save last white border line added
        Actor* lastWB = *(--m_actorList.end());
        m_lastBDY = lastWB->getY();
    }

    // Add human pedestrians
    int chanceHumanPed = max(200 - getLevel() * 10, 30);
    int rand = randInt(0, chanceHumanPed - 1);
    if (rand == 0)
    {
        int x = randInt(0, VIEW_WIDTH);
        m_actorList.push_back(new HumanPedestrian(x, VIEW_HEIGHT, this));
    }

    // Add Zombie Pedestrians
    int chanceZombiePed = max(100 - getLevel() * 10, 20);
    rand = randInt(0, chanceZombiePed - 1);
    if (rand == 0)
    {
        int x = randInt(0, VIEW_WIDTH);
        m_actorList.push_back(new ZombiePedestrian(x, VIEW_HEIGHT, this));
    }

    // Add lost souls
    int chanceLostSoul = 100;
    rand = randInt(0, chanceLostSoul - 1);
    if (rand == 0)
    {
        int x = randInt(LEFT_EDGE + 1, RIGHT_EDGE - 1);     // DO BORDERS COUNT AS PART OF THE ROAD??? PROBABLY NOT     !!!
        m_actorList.push_back(new Soul(x, VIEW_HEIGHT, this));
    }

    // Add oil slicks
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    rand = randInt(0, chanceOilSlick - 1);
    if (rand == 0)
    {
        int x = randInt(LEFT_EDGE + 1, RIGHT_EDGE - 1);     // DO BORDERS COUNT AS PART OF THE ROAD??? PROBABLY NOT     !!!
        int randSize = randInt(2, 5);
        m_actorList.push_back(new OilSlick(x, VIEW_HEIGHT, randSize, this));
    }

    // Add Holy Water Goodie
    int chanceOfHolyWater = 100 + 10 * getLevel();
    rand = randInt(0, chanceOfHolyWater - 1);
    if (rand == 0)
    {
        int x = randInt(LEFT_EDGE + 1, RIGHT_EDGE - 1);     // DO BORDERS COUNT AS PART OF THE ROAD??? PROBABLY NOT     !!!
        m_actorList.push_back(new HolyWaterGoodie(x, VIEW_HEIGHT, this));
    }

    // Add Zombie Cabs
    int chanceVehicle = max(100 - getLevel() * 10, 20);
    rand = randInt(0, chanceVehicle - 1);
    if (rand == 0)
    {
        int curLane[2];
        double newY = NULL;
        double initialVSpeed = NULL;
        double centerOfChosenLane = NULL;

        // Find viable lane if possible
        int potentialLOne = randInt(1, 3);
        int potentialLTwo = potentialLOne == 1 ? 2 : 1;
        int potentialLThree = potentialLOne == 1 || potentialLOne == 2 ? 3 : 2;
        int potentialLanes[] = { potentialLOne, potentialLTwo, potentialLThree };
        for (int i = 0; i < 3; i++)
        {
            switch (potentialLanes[i])
            {
            case 1:
                curLane[0] = R_LANE[0];
                curLane[1] = R_LANE[1];
                centerOfChosenLane = ROAD_CENTER + ROAD_WIDTH / 3.0;
                break;
            case 2:
                curLane[0] = L_LANE[0];
                curLane[1] = L_LANE[1];
                centerOfChosenLane = ROAD_CENTER - ROAD_WIDTH / 3.0;
                break;
            case 3:
                curLane[0] = M_LANE[0];
                curLane[1] = M_LANE[1];
                centerOfChosenLane = ROAD_CENTER;
                break;
            }
            if (determineLane(curLane, initialVSpeed, newY))
                break;
        }
        if (!(initialVSpeed == NULL))    // avoid introducing cab if there are no viable lanes
        {
            addActor(new ZombieCab(centerOfChosenLane, newY, initialVSpeed, this));
        }
    }

}

bool StudentWorld::determineLane(const int* lane, double& speed, double& y)
{
    Actor* closestCWActor = findClosestCollisionWorthyActor(lane, BOTTOM);
    if (closestCWActor == nullptr || closestCWActor->getY() > VIEW_HEIGHT / 3.0)
    {
        y = SPRITE_HEIGHT / 2;
        int rand = randInt(2, 4);
        speed = getGhostRacer()->getVertSpeed() + rand;
        // break out of loop and proceed to step 3
        return true;
    }
    closestCWActor = findClosestCollisionWorthyActor(lane, TOP);
    if (closestCWActor == nullptr || closestCWActor->getY() < VIEW_HEIGHT * 2.0 / 3.0)
    {
        y = VIEW_HEIGHT - SPRITE_HEIGHT / 2.0;
        int rand = randInt(2, 4);
        speed = getGhostRacer()->getVertSpeed() - rand;
        // break out of loop and proceed to step 3
        return true;
    }
    return false;
}

Actor* StudentWorld::findClosestCollisionWorthyActor(const int lane[], const int sideComingInFrom, const Actor* self, bool flagToNotConsiderGR)
{
    list<Actor*>::iterator it;
    list<Actor*>::iterator res = m_actorList.end();

    // set ry to allow for first collision worthy actor encountered to be the initial one
    double ry = sideComingInFrom == BOTTOM ? VIEW_HEIGHT + 1 : -1;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        double x = (*it)->getX();
        double y = (*it)->getY();

        // if the current collision worthy actor's x is in the lane and has a y less than the current lowest actor
        if (!(self != (*it) && x >= lane[0] && x <= lane[1] && (*it)->isCollisionWorthy()))
            continue;

        // determine if this current actor is closer to whichever side specified
        if (sideComingInFrom == BOTTOM && y < ry)
        {
            res = it;
            ry = (*res)->getY();
        }
        else if (sideComingInFrom == TOP && y > ry)
        {
            res = it;
            ry = (*res)->getY();
        }
    }

    // adjust flag to consider or not consider ghost racer as a collision worthy actor
    bool flag = flagToNotConsiderGR ? false : true;
    double grX = getGhostRacer()->getX();
    double grY = getGhostRacer()->getY();
    // determine if ghost racer is the closest collision worthy actor
    if (grX >= lane[0] && grX <= lane[1] && flag)
    {    
        if (sideComingInFrom == BOTTOM && grY < ry)
        {
            return getGhostRacer();
        }
        else if (sideComingInFrom == TOP && grY > ry)
        {
            return getGhostRacer();
        }
    }

    // if there is a collision worthy actor, return a pointer to it
    if (res != m_actorList.end())
        return (*res);
    else
        return nullptr;
}

bool StudentWorld::executeProjectileImpact(Actor* projectile)
{
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        // Compare addresses, continue if the same
        if ((*it) == projectile)
            continue;
        // Find overlapping actor that can be activated
        if (isOverlapping(projectile, *it) && (*it)->beSprayedIfAppropriate())
        {
            projectile->setLife(false);
            return true;
        }
    }
    return false;
}

void StudentWorld::generateStatistics()
{
    ostringstream oss;
    oss << "Score: " << getScore()
}
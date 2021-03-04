#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
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
    // initialize starting values and ghost racer
    m_soulsSaved = 0;
    m_bonusPoints = 5000;
    m_ghostRacer = new GhostRacer(this);

    // Initialize yellow border lines
    for (int j = 0; j < N; j++)
    {
        // initialize them on extreme edges of the road
        int y = j * SPRITE_HEIGHT;
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, y, this));
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, y, this));
    }

    // Initialize white border lines
    for (int j = 0; j < M; j++)
    {
        // initialize them on between lanes
        int y = j * (4 * SPRITE_HEIGHT);
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, ML_EDGE, y, 
            this));
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, MR_EDGE, y, 
            this));
    }
    // Save y coordinate of the last white border line added
    Actor* lastWB = *(--m_actorList.end());
    m_lastBDY = lastWB->getY();

    // initialize game stats
    setGameStatText(generateStatistics());

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

    // Have ghost racer do something
    int returnVal;
    returnVal = tellActorToDoSomething(m_ghostRacer);
    // if gr dies or level finishes, return that
    if (returnVal != 999)
        return returnVal;
    // Have each active actor do something
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        returnVal = tellActorToDoSomething((*it));
        // if gr dies or level finishes, return that
        if (returnVal != 999)
            return returnVal;
    }

    deleteDeadActors();
    
    addNewActors();

    // Update status text
    m_bonusPoints--;
    setGameStatText(generateStatistics());

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete ghost racer
    delete m_ghostRacer;
    // delete all actors in the game
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end();)
    {
        delete* it;
        it = m_actorList.erase(it);
    }
}

void StudentWorld::deleteDeadActors()
{
    // Delete dead actors and remove the pointers
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end();)
    {
        if (!(*it)->isAlive())
        {
            delete* it;
            it = m_actorList.erase(it);
        }
        else
            it++;
    }
}

void StudentWorld::addNewActors()
{
    // Calculate y coordinate for new borderlines
    int newBorderY = VIEW_HEIGHT - SPRITE_HEIGHT;
    double lastWLSpeed = -4 - m_ghostRacer->getVertSpeed();
    m_lastBDY = m_lastBDY + lastWLSpeed;
    double deltaY = newBorderY - m_lastBDY;

    // add yellow borderlines if there is space
    if (deltaY >= SPRITE_HEIGHT)
    {
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE,
            newBorderY, this));
        addActor(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE,
            newBorderY, this));
    }
    // add white borderlines if there is space
    if (deltaY >= 4.0 * SPRITE_HEIGHT)
    {
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, ML_EDGE,
            newBorderY, this));
        addActor(new BorderLine(IID_WHITE_BORDER_LINE, MR_EDGE,
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
        addActor(new HumanPedestrian(x, VIEW_HEIGHT, this));
    }

    // Add Zombie Pedestrians
    int chanceZombiePed = max(100 - getLevel() * 10, 20);
    rand = randInt(0, chanceZombiePed - 1);
    if (rand == 0)
    {
        int x = randInt(0, VIEW_WIDTH);
        addActor(new ZombiePedestrian(x, VIEW_HEIGHT, this));
    }

    // Add lost souls
    int chanceLostSoul = 100;
    rand = randInt(0, chanceLostSoul - 1);
    if (rand == 0)
    {
        int x = randInt(LEFT_EDGE, RIGHT_EDGE);
        addActor(new Soul(x, VIEW_HEIGHT, this));
    }

    // Add oil slicks
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    rand = randInt(0, chanceOilSlick - 1);
    if (rand == 0)
    {
        int x = randInt(LEFT_EDGE, RIGHT_EDGE);
        int randSize = randInt(2, 5);
        addActor(new OilSlick(x, VIEW_HEIGHT, randSize, this));
    }

    // Add Holy Water Goodie
    int chanceOfHolyWater = 100 + 10 * getLevel();
    rand = randInt(0, chanceOfHolyWater - 1);
    if (rand == 0)
    {
        int x = randInt(LEFT_EDGE, RIGHT_EDGE);
        addActor(new HolyWaterGoodie(x, VIEW_HEIGHT, this));
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

        // start with a randomly chosen lane
        int potentialLOne = randInt(1, 3);
        int potentialLTwo = potentialLOne == 1 ? 2 : 1;
        int potentialLThree = potentialLOne == 1 || potentialLOne == 2 ? 3 : 2;
        int potentialLanes[] = { potentialLOne, potentialLTwo, potentialLThree };
        // try each lane to see if any has space for a new zombie cab
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
            // if there is space in the current lane, break
            if (tryLane(curLane, initialVSpeed, newY))
                break;
        }
        // if there are no viable lanes, avoid introducing new zombie cab this tick 
        if (!(initialVSpeed == NULL))
        {
            addActor(new ZombieCab(centerOfChosenLane, newY, initialVSpeed, this));
        }
    }

}

bool StudentWorld::tryLane(const int* lane, double& speed, double& y)
{
    // try to get closest collision worthy actor with respect to the bottom
    Actor* closestCWActor = findClosestCollisionWorthyActor(lane, BOTTOM);
    // if there is space for a zombie cab
    if (closestCWActor == nullptr || closestCWActor->getY() > VIEW_HEIGHT / 3.0)
    {
        // save speed and y values
        y = SPRITE_HEIGHT / 2;
        int rand = randInt(2, 4);
        speed = getGhostRacer()->getVertSpeed() + rand;
        // break out of loop and proceed to step 3
        return true;
    }
    // try to get closest collision worthy actor with respect to the top
    closestCWActor = findClosestCollisionWorthyActor(lane, TOP);
    // if there is space for a zombie cab
    if (closestCWActor == nullptr || closestCWActor->getY() < VIEW_HEIGHT * 2.0 / 3.0)
    {
        // save speed and y values
        y = VIEW_HEIGHT - SPRITE_HEIGHT / 2.0;
        int rand = randInt(2, 4);
        speed = getGhostRacer()->getVertSpeed() - rand;
        // break out of loop and proceed to step 3
        return true;
    }
    return false;
}

Actor* StudentWorld::findClosestCollisionWorthyActor(const int lane[], const int sideComingInFrom, const Actor* self) const
{
    list<Actor*>::const_iterator it;
    list<Actor*>::const_iterator res = m_actorList.end();

    // set ry to allow for first collision worthy actor encountered to be the initial one
    double ry = sideComingInFrom == BOTTOM ? VIEW_HEIGHT + 1 : -1;
    // for each actor in the list of present actors
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        double x = (*it)->getX();
        double y = (*it)->getY();

        // if the current actor is not collision worthy or the actor's x is not in the lane or the one being
        // looked at is the same as the one looking for this, continue
        if (!(self != (*it) && x >= lane[0] && x <= lane[1] && (*it)->isCollisionWorthy()))
            continue;

        // if the side specified is the bottom and this actor is closer than the current closest, update this actor to be the closest
        if (sideComingInFrom == BOTTOM && y < ry)
        {
            res = it;
            ry = (*res)->getY();
        }
        // else if the side specified is the top and this actor is closer than the current closest, update this actor
        // to be the closest
        else if (sideComingInFrom == TOP && y > ry)
        {
            res = it;
            ry = (*res)->getY();
        }
    }

    double grX = getGhostRacer()->getX();
    double grY = getGhostRacer()->getY();
    // determine if ghost racer is the closest collision worthy actor
    if (grX >= lane[0] && grX <= lane[1] && self != getGhostRacer())
    {    
        // if ghost racer is the closest with respect to the specified side, return ghost racer's pointer
        if (sideComingInFrom == BOTTOM && grY < ry)
        {
            return getGhostRacer();
        }
        else if (sideComingInFrom == TOP && grY > ry)
        {
            return getGhostRacer();
        }
    }

    // if there is a closest collision worthy actor, return a pointer to it
    if (res != m_actorList.end())
        return (*res);
    else
        return nullptr;
}

bool StudentWorld::executeProjectileImpact(Actor* projectile)
{
    list<Actor*>::iterator it;
    // for each actor in the list of present actors
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        // Compare addresses, continue if the same
        if ((*it) == projectile)
            continue;
        // Find overlapping actor that can be sprayed
        if (isOverlapping(projectile, *it) && (*it)->beSprayedIfAppropriate())
        {
            // update that the projectile is used
            projectile->setLife(false);
            return true;
        }
    }
    // else, the projectile didn't hit anything
    return false;
}

string StudentWorld::generateStatistics() const
{
    ostringstream oss;
    string doublespace = "  ";
    string score = "Score: " + to_string(getScore());
    string level = "Lvl: " + to_string(getLevel());
    string souls2save = "Souls2Save: " + to_string((2 * getLevel() + 5 - getSoulsSaved()));
    string lives = "Lives: " + to_string(getLives());
    string health = "Health: " + to_string(getGhostRacer()->getHitPoints());
    string sprays = "Sprays: " + to_string(getGhostRacer()->getUnitsOfHolyWater());
    string bonus = "Bonus: " + to_string(getBonusScore());
    oss << score << doublespace << level << doublespace << souls2save << doublespace << lives << doublespace << health << doublespace << sprays << doublespace << bonus;
    return oss.str();
}

int StudentWorld::tellActorToDoSomething(Actor* ptr)
{
    // if the actor is alive
    if (ptr->isAlive())
    {
        // have the actor do something
        ptr->doSomething();
        // if the ghost racer is now dead, decrease lives and indicate so
        if (!m_ghostRacer->isAlive())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        // if the number of souls saved is enough, add bonus score to score and indicate that the level is finished
        else if (getNumOfSoulsSaved() >= getLevel() * 2 + 5)
        {
            increaseScore(getBonusScore());
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    // else, the actor is dead and skips it
    return 999;
}

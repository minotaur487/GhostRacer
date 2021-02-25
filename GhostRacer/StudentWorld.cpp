#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"  // For isOverlapping, considering changing cuz this would take the whole file
#include <string>
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
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, y, 
            this));
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, y, 
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
    int lastWLSpeed = -4 - m_ghostRacer->getVertSpeed();
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
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3.0,
            newBorderY, this));
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3.0,
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

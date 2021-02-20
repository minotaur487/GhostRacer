#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Character.h, and Character.cpp

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
    else if (m_ghostRacer->getNumOfSoulsSaved() >= getLevel() * 2 + 5)
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
            else if (m_ghostRacer->getNumOfSoulsSaved() >= getLevel() * 2 + 5)
            {
                //AWARD BONUS POINTS                                            !!!
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    // Delete dead actors
    list<Actor*>::iterator temp;
    for (it = temp = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        if (!(*it)->isAlive())
        {
            if (it == m_actorList.begin())
                temp++;
            delete* it;
            m_actorList.erase(it);
            it = temp;
        }
        if (it != m_actorList.begin() && it != temp)
            temp = it;
    }
    
    // Add necessary new objects
    int newBorderY = VIEW_HEIGHT - SPRITE_HEIGHT;
    double lastWLSpeed = -4.0 - m_ghostRacer->getVertSpeed();
    m_lastBDY = m_lastBDY + lastWLSpeed;
    double deltaY = newBorderY - m_lastBDY;

    if (deltaY >= SPRITE_HEIGHT)
    {
        m_actorList.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE,
            newBorderY, this));
        m_actorList.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE,
            newBorderY, this));
    }
    if (deltaY >= 4 * SPRITE_HEIGHT)
    {
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3,
            newBorderY, this));
        m_actorList.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3,
            newBorderY, this));

        // Save last white border line added
        Actor* lastWB = *(--m_actorList.end());
        m_lastBDY = lastWB->getY();
    }

    // Update status text
    // !!!  //

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_ghostRacer;
    list<Actor*>::iterator it, temp;
    for (it = temp = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        if (it == m_actorList.begin())
            temp++;
        delete* it;
        m_actorList.erase(it);
        it = temp;
        if (it != m_actorList.begin() && it != temp)
            temp = it;
    }
}

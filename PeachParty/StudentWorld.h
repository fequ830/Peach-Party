#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
//    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp(); //NONE OF THESE CAN BE CALLED, except StudentWorld's destructor on cleanUp()
    bool isMovable(int x, int y); //returns true if there's a square at x and y
    void introduceVortex(StudentWorld* worldPtr, int startX, int startY, int startDir);
    bool isThereAnActorToCollideWith(Actor*& collideActor, int x, int y);
//    void directionToMoveIn(Actor* ptrToActor, int x, int y, bool onlyPerpendicular); //determines the direction for the actor to move in
    Player* getPtrToPlayer(int player); //retrieves the pointer to either peach (1) or yoshi (2)
    int getBankAmount();
    void setBankAmount(int amt);
    void getRandomSquare(int& x, int& y);
    bool isEntityAtFork(Entity* entity);
    void placeDroppingSquare(int x, int y);

private:
    int bankAmount;    
    
    std::vector<Actor*> actorVect;
//    std::vector<Square*> otherVect; //contains everything else, such as squares and projectiles
    Player* ptrToPeach;
    Player* ptrToYoshi;
};

#endif // STUDENTWORLD_H_

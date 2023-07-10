#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameWorld.h"

//GraphObject(int imageID, double startX, double startY,
//int startDirection = right, int depth = 0, double size = 1.0);
//int getX() const; // in pixels (0-255)
//int getY() const; // in pixels (0-255)
//void moveTo(int x, int y); // in pixels (0-255)
//int getDirection() const; // sprite angle in degrees (0-359)
//void setDirection(Direction d); // sprite angle in degrees (0-359)
//void getPositionInThisDirection(int angle, int distance, int& newX, int& newY) const;
//void moveAtAngle(int angle, int distance);


//GraphObject(int imageID,
//int startX, // column first: x
//int startY, // then row: y
//int startDirection // 0, 90, 180, 270 / right, up, left, down
//int depth = 0, // graphical depth
//double size = 1.0) // don’t worry about size for this project


class StudentWorld;


//BASE CLASS
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* worldPtr, int imageID, int startX, int startY, int startDir, int depth, int size);
    virtual ~Actor() = 0;
    StudentWorld* getPointerToWorld();
    virtual void doSomething() = 0;
    bool amIAlive();
    void setAliveStatus(bool status);
    bool canBeOccupied();
    void setOccupability(bool occ);
    bool isHittableByVector();
    void setHittable(bool badOrNot);
    void setImpacted(bool impactStatus);
    bool amIImpacted();
private:
    StudentWorld* pointerToWorld;
    bool aliveStatus;
    bool isOccupiable;
    bool hittableByVector;
    bool impactedOrNot;
};

//ENTITY CLASS: THINGS THAT MOVE
class Entity : public Actor
{
public:
    Entity(StudentWorld* worldPtr, int imageID, int startX, int startY, int startDir, int depth);
    virtual ~Entity() = 0;
    virtual void doSomething() = 0;
    int getMovingDirection();
    void setMovingDirection(int dir);
    int getProgressTowardsNext();
    void setProgressTowardsNext(int prog);
    void directionToMoveIn(int x, int y, bool onlyPerpendicular); //determines the direction for the entity to move in
    int get_ticks_to_move();
    void set_ticks_to_move(int ticks);
    int getRollNumber();
    void setRollNumber(int num);
    bool getWaitingStatus();
    void setWaitingStatus(bool status);
private:
    int movingDirection;
    int progressTowardsNext;
    bool waitingToRoll;
    int rollNumber;
    int ticks_to_move;
};


//PLAYER CLASSES
class Player : public Entity {
public:
    Player(StudentWorld* worldPtr, int imageID, int startX, int startY);
    virtual void doSomething(int playerNum);
    int vortexCount();
    void setVortexCount(int vo);
    int getCoins();
    int getStars();
    void setCoins(int coins);
    void setStars(int stars);
    void setDirectionSquareForced(int dir);
    bool isDirectionSquareForcing();
    void setForcing(bool force);
private:
    int numVortex;
    int numCoins;
    int numStars;
    int directionSquareForced;
    bool isForced;
};

class Yoshi : public Player {
public:
    Yoshi(StudentWorld* worldPtr, int startX, int startY);
    virtual void doSomething();
private:
};

class Peach : public Player {
public:
    Peach(StudentWorld* worldPtr, int startX, int startY);
    virtual void doSomething();
private:
};


//VORTEX CLASS
class Vortex : public Entity {
public:
  Vortex(StudentWorld* worldPtr, int startX, int startY, int startDir);
  virtual void doSomething();

private:
};


//BADDIE CLASSES
class Baddie : public Entity {
public:
  Baddie(StudentWorld* worldPtr, int imageID, int startX, int startY, int maxMoveAmount);
    virtual void doSomething();
    virtual void doDifferentiatedAction() = 0;
    virtual void doSpecialPower() = 0;
    int getPauseTime();
    void setPauseTime(int time);
    bool getDidDifferentiatedAction();
    void setDidDifferentiatedAction(bool did);
private:
    int pauseTime;
    int maxMoveAmount;
    bool didDifferentiatedAction;
};

class Boo : public Baddie {
public:
  Boo(StudentWorld* worldPtr, int startX, int startY);
    virtual void doDifferentiatedAction();
    virtual void doSpecialPower();
private:
};

class Bowser : public Baddie {
public:
  Bowser(StudentWorld* worldPtr, int startX, int startY);
    virtual void doDifferentiatedAction();
    virtual void doSpecialPower();
private:
};



//SQUARE CLASSES
class Square : public Actor {
public:
  Square(StudentWorld* worldPtr, int imageID, int startX, int startY, int startDir);
    virtual void doSomething();
    void setActiveStatus(bool status);
    int whoIsHere();
    bool isPlayerStillHere(int player);
    void setPlayersHere(int here);
    virtual void differentiatedSquareAction(int player) = 0;
private:
    bool peachIsHere;
    bool yoshiIsHere;
};

class CoinSquare : public Square {
public:
    CoinSquare(StudentWorld* worldPtr, int imageID, int startX, int startY);
private:
    virtual void differentiatedSquareAction(int player) = 0;
};

class BlueCoinSquare : public CoinSquare {
public:
    BlueCoinSquare(StudentWorld* worldPtr, int startX, int startY, bool isPlayerStartingSquare);
private:
    virtual void differentiatedSquareAction(int player);
};

class RedCoinSquare : public CoinSquare {
public:
    RedCoinSquare(StudentWorld* worldPtr, int startX, int startY);
private:
    virtual void differentiatedSquareAction(int player);
};

class DirectionSquare : public Square {
public:
    DirectionSquare(StudentWorld* worldPtr, int startX, int startY, int startDir);
    virtual void differentiatedSquareAction(int player);
private:
};

class EventSquare : public Square {
public:
    EventSquare(StudentWorld* worldPtr, int startX, int startY);
    virtual void differentiatedSquareAction(int player);
private:
};

class BankSquare : public Square {
public:
    BankSquare(StudentWorld* worldPtr, int startX, int startY);
    virtual void differentiatedSquareAction(int player);
private:
};

class StarSquare : public Square {
public:
    StarSquare(StudentWorld* worldPtr, int startX, int startY);
    virtual void differentiatedSquareAction(int player);
private:
};

class DroppingSquare : public Square {
public:
    DroppingSquare(StudentWorld* worldPtr, int startX, int startY);
    virtual void differentiatedSquareAction(int player);
private:
};

#endif // ACTOR_H_

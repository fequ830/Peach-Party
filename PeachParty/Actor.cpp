#include "Actor.h"
#include "StudentWorld.h"

//GraphObject(int imageID,
//int startX, // column first: x
//int startY, // then row: y
//int startDirection // 0, 90, 180, 270 / right, up, left, down
//int depth = 0, // graphical depth
//double size = 1.0) // don’t worry about size for this project

//GraphObject(int imageID, double startX, double startY,
//int startDirection = right, int depth = 0, double size = 1.0);
//int getX() const; // in pixels (0-255)
//int getY() const; // in pixels (0-255)
//void moveTo(int x, int y); // in pixels (0-255)
//int getDirection() const; // sprite angle in degrees (0-359)
//void setDirection(Direction d); // sprite angle in degrees (0-359)
//void getPositionInThisDirection(int angle, int distance, int& newX, int& newY) const;
//void moveAtAngle(int angle, int distance);

Actor::Actor(StudentWorld* worldPtr, int imageID, int startX, int startY, int startDir, int depth, int size)
: GraphObject(imageID, startX, startY, startDir, depth, 1.0), aliveStatus(true), pointerToWorld(worldPtr), isOccupiable(false), hittableByVector(false), impactedOrNot(false)
{
    
}

Actor::~Actor() {
}

void Actor::setHittable(bool badOrNot) {
    hittableByVector = badOrNot;
}

bool Actor::isHittableByVector() {
    return hittableByVector;
}

StudentWorld* Actor::getPointerToWorld() {
    return pointerToWorld;
}

bool Actor::amIAlive() {
    return aliveStatus;
}

void Actor::setAliveStatus(bool status) {
    aliveStatus = status;
    return;
}

bool Actor::canBeOccupied() {
    return isOccupiable;
}

void Actor::setOccupability(bool occ) {
    isOccupiable = occ;
}

void Actor::setImpacted(bool impactStatus) {
    impactedOrNot = impactStatus;
}

bool Actor::amIImpacted() {
    return impactedOrNot;
}



Entity::Entity(StudentWorld* worldPtr, int imageID, int startX, int startY, int startDir, int depth)
: Actor(worldPtr, imageID, startX, startY, startDir, depth, 1.0), movingDirection(right), progressTowardsNext(16), waitingToRoll(true), rollNumber(0), ticks_to_move(0)
{
    moveTo(startX*16, startY*16);
    setDirection(0);
}

Entity::~Entity() {
    
}

int Entity::getMovingDirection() {
    return movingDirection;
}


void Entity::setMovingDirection(int dir) {
    movingDirection = dir;
}


int Entity::getProgressTowardsNext() {
    return progressTowardsNext;
}

void Entity::setProgressTowardsNext(int prog) {
    progressTowardsNext = prog;
}

void Entity::setWaitingStatus(bool status) {
    waitingToRoll = status;
}

int Entity::get_ticks_to_move() {
    return ticks_to_move;
}

void Entity::set_ticks_to_move(int ticks) {
    ticks_to_move = ticks;
}

int Entity::getRollNumber() {
    return rollNumber;
}

void Entity::setRollNumber(int num) {
    rollNumber = num;
}


bool Entity::getWaitingStatus() {
    return waitingToRoll;
}

void Entity::directionToMoveIn(int x, int y, bool onlyPerpendicular) {
    int checkX, checkY;
    getPositionInThisDirection(getMovingDirection(), getProgressTowardsNext(), checkX, checkY); //see if it's possible to move 1 square in the current facing direction
    if (getPointerToWorld()->isMovable(checkX, checkY) == false) { //if the player CANNOT move in the current direction
        bool firstPossible = false, secondPossible = false;
        int chosenDir = 0;
        int firstPerp = 0, secondPerp = 0;
        switch (getMovingDirection()) {
            case Actor::up:
                firstPerp = Actor::right;
                secondPerp = Actor::left;
                break;
            case Actor::down:
                firstPerp = Actor::right;
                secondPerp = Actor::left;
                break;
            case Actor::left:
                firstPerp = Actor::up;
                secondPerp = Actor::down;
                break;
            case Actor::right:
                firstPerp = Actor::up;
                secondPerp = Actor::down;
                break;
        }
        
        getPositionInThisDirection(firstPerp, getProgressTowardsNext(), checkX, checkY); //check one perpendicular direction
        if (getPointerToWorld()->isMovable(checkX, checkY) == true) {
            firstPossible = true;
        }
        
        getPositionInThisDirection(secondPerp, getProgressTowardsNext(), checkX, checkY); //and check the other perpendicular direction
        if (getPointerToWorld()->isMovable(checkX, checkY) == true) {
            secondPossible = true;
        }
        
        if (firstPossible == true)
            chosenDir = firstPerp;
        else if (secondPossible == true)
            chosenDir = secondPerp;
        else if (onlyPerpendicular == false)
            chosenDir = abs(getMovingDirection()-180); //if we're allowed to go in the opposite direction we came from, then do so
                    
        setMovingDirection(chosenDir);
        setDirection(chosenDir == Actor::left ? Actor::left : Actor::right); //if it will face left then set its direction to left; otherwise set to right
    }
}



Player::Player(StudentWorld* worldPtr, int imageID, int startX, int startY)
: Entity(worldPtr, imageID, startX, startY, 0, 0), numStars(0), numCoins(0), numVortex(0), directionSquareForced(0), isForced(false)
{
}



int Player::vortexCount() {
    return numVortex;
}

void Player::setVortexCount(int vo) {
    numVortex = vo;
}


int Player::getCoins() {
    return numCoins;
}

int Player::getStars() {
    return numStars;
}

void Player::setCoins(int coins) {
    numCoins = coins;
}

void Player::setStars(int stars) {
    numStars = stars;
}

void Player::setDirectionSquareForced(int dir) {
    directionSquareForced = dir;
}

bool Player::isDirectionSquareForcing() {
    return isForced;
}

void Player::setForcing(bool force) {
    isForced = force;
}



void Player::doSomething(int playerNum) {
    if (getProgressTowardsNext() == 0) {
        if (getRollNumber() > 0) //after each successfully traversed square, reduce roll number by 1
            setRollNumber(getRollNumber()-1);
        setProgressTowardsNext(16);
    }
    
    if (getWaitingStatus() == true) {
        directionToMoveIn(getX(), getY(), false); //check if invalid direction
        
        int action = getPointerToWorld()->getAction(playerNum);
        switch (action) {
            case ACTION_ROLL: {
                srand((unsigned) time(NULL));
                int die_roll = 1 + rand() % 10;
                set_ticks_to_move(die_roll*8);
                setWaitingStatus(false); //set walking state to true
                setRollNumber(die_roll);
                break;
            }
            case ACTION_FIRE: {
                if (vortexCount() <= 0) //if player doesn't have any vortexes
                    break;
                getPointerToWorld()->introduceVortex(getPointerToWorld(), getX(), getY(), getMovingDirection()); //create a vortex
                getPointerToWorld()->playSound(SOUND_PLAYER_FIRE);
                numVortex--; //remove the vortex
                break;
            }
            case ACTION_NONE: //no action or some other key
            default:
                return;
        }
    }
    
    if (getWaitingStatus() == false) { //player is not in waitingtoroll state (ie, in the walking state)
        if (isForced == true) { //check if the player is on a direction square
            setMovingDirection(directionSquareForced);
            setDirection(directionSquareForced == left ? left : right);
            isForced = false;
        }
        else if (getPointerToWorld()->isEntityAtFork(this) == true) {
            int action = getPointerToWorld()->getAction(playerNum);
            int moveDir = getPointerToWorld()->getPtrToPlayer(playerNum)->getMovingDirection();
            switch (action) {
                case ACTION_UP:
                    if (getPointerToWorld()->isMovable(getX(), getY()+16) == true && moveDir != down) {
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setMovingDirection(up);
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setDirection(right);
                    }
                    else
                        return;
                    break;
                case ACTION_DOWN:
                    if (getPointerToWorld()->isMovable(getX(), getY()-16) == true && moveDir != up) {
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setMovingDirection(down);
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setDirection(right);
                    }
                    else
                        return;
                    break;
                case ACTION_LEFT:
                    if (getPointerToWorld()->isMovable(getX()-16, getY()) == true && moveDir != right) {
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setMovingDirection(left);
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setDirection(left);
                    }
                    else
                        return;
                    break;
                case ACTION_RIGHT:
                    if (getPointerToWorld()->isMovable(getX()+16, getY()) == true && moveDir != left) {
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setMovingDirection(right);
                        getPointerToWorld()->getPtrToPlayer(playerNum)->setDirection(right);
                    }
                    else
                        return;
                    break;
                case ACTION_NONE: //no action or some invalid key
                default:
                    return;
            }
        }
        else
            directionToMoveIn(getX(), getY(), true); //if player can't continue moving forward in its current direction
        
        moveAtAngle(getMovingDirection(), 2); //move 2 pixels in the walk direction
        setProgressTowardsNext(getProgressTowardsNext()-2);
        set_ticks_to_move(get_ticks_to_move()-1);
        if (get_ticks_to_move() == 0)
            setWaitingStatus(true);
        return;
    }
}

Yoshi::Yoshi(StudentWorld* worldPtr, int startX, int startY)
: Player(worldPtr, IID_YOSHI, startX, startY)
{
}

void Yoshi::doSomething() {
    Player::doSomething(2);
}

Peach::Peach(StudentWorld* worldPtr, int startX, int startY)
: Player(worldPtr, IID_PEACH, startX, startY)
{
}

void Peach::doSomething() {
    Player::doSomething(1);
}






Baddie::Baddie(StudentWorld* worldPtr, int imageID, int startX, int startY, int maxMoveAmount)
: Entity(worldPtr, imageID, startX, startY, 0, 0), pauseTime(180), maxMoveAmount(maxMoveAmount), didDifferentiatedAction(false)
{
    setHittable(true); //make it so it can be hit by a vector (applicable to all baddies)
    moveTo(getX(), getY()); //IDK why, but this is the only way to start off the boos covering their eyes
}

int Baddie::getPauseTime() {
    return pauseTime;
}

void Baddie::setPauseTime(int time) {
    pauseTime = time;
}

bool Baddie::getDidDifferentiatedAction() {
    return didDifferentiatedAction;
}

void Baddie::setDidDifferentiatedAction(bool did) {
    didDifferentiatedAction = did;
}

void Baddie::doSomething() {
    if (amIImpacted() == true) { //got hit by a vortex
        int newX, newY;
        getPointerToWorld()->getRandomSquare(newX, newY);
        moveTo(newX, newY);
        setMovingDirection(right);
        setDirection(0);
        setWaitingStatus(true); //put it in paused state
        setPauseTime(180);
        setImpacted(false); //remove impacted status
    }
    
    if (getProgressTowardsNext() == 0) {
        if (getRollNumber() > 0) //after each successfully traversed square, reduce roll number by 1
            setRollNumber(getRollNumber()-1);
        setProgressTowardsNext(16);
    }
    
    if (getWaitingStatus() == true) { //its in paused state
        doDifferentiatedAction(); //for boo, swap stars/coins, and for bowser take away everything
        
        setPauseTime(getPauseTime()-1); //decrement pause counter
        
        if (getPauseTime() == 0) {
            srand((unsigned) time(NULL));
            int squares_to_move = 1 + rand() % maxMoveAmount; //choose from 1 to maxmoveamount how many squares to move; max = 3 for boo, = 10 for bowser
            set_ticks_to_move(squares_to_move*8);
            setRollNumber(squares_to_move);
            
            directionToMoveIn(getX(), getY(), false);
            setWaitingStatus(false); //set to walking/active state
        }
    }
    else if (getWaitingStatus() == false) {//boo is in walking state
        if (getPointerToWorld()->isEntityAtFork(this) == true) {//boo is atop square and he's at fork
            srand((unsigned) time(NULL));
            int moveDir = getMovingDirection();
            for (;;) {
                int randDir = rand() % 4;
                if (randDir == 0) {
                    if (getPointerToWorld()->isMovable(getX(), getY()+getProgressTowardsNext()) == true && moveDir != down) {
                        setMovingDirection(up);
                        setDirection(right);
                        break;
                    }
                }
                else if (randDir == 1) {
                    if (getPointerToWorld()->isMovable(getX(), getY()-getProgressTowardsNext()) == true && moveDir != up) {
                        setMovingDirection(down);
                        setDirection(right);
                        break;
                    }
                }
                else if (randDir == 2) {
                    if (getPointerToWorld()->isMovable(getX()-getProgressTowardsNext(), getY()) == true && moveDir != right) {
                        setMovingDirection(left);
                        setDirection(left);
                        break;
                    }
                }
                else if (randDir == 3) {
                    if (getPointerToWorld()->isMovable(getX()+getProgressTowardsNext(), getY()) == true && moveDir != left) {
                        setMovingDirection(right);
                        setDirection(right);
                        break;
                    }
                }
            }
            
        }
        else
            directionToMoveIn(getX(), getY(), true); //boo is at a turning point and must go perpendicular
        
        moveAtAngle(getMovingDirection(), 2); //move 2 pixels in walk direction
        setProgressTowardsNext(getProgressTowardsNext()-2);
        set_ticks_to_move(get_ticks_to_move()-1);
        if (get_ticks_to_move() == 0) {
            setWaitingStatus(true);
            setPauseTime(180);
            doSpecialPower();
        }
    }
}


Boo::Boo(StudentWorld* worldPtr, int startX, int startY)
:  Baddie(worldPtr, IID_BOO, startX, startY, 3) {
}

void Boo::doDifferentiatedAction() {
    if ((getPointerToWorld()->getPtrToPlayer(1)->getX() == this->getX() && getPointerToWorld()->getPtrToPlayer(1)->getY() == this->getY() && getPointerToWorld()->getPtrToPlayer(1)->getWaitingStatus() == true) || (getPointerToWorld()->getPtrToPlayer(2)->getX() == this->getX() && getPointerToWorld()->getPtrToPlayer(2)->getY() == this->getY() && getPointerToWorld()->getPtrToPlayer(2)->getWaitingStatus() == true)) { //if boo is on same square as player
        
        if (getDidDifferentiatedAction() == true)
            return;
        
        srand((unsigned) time(NULL));
        int die_roll = rand() % 2;
        if (die_roll == 0) { //swap players' coins
            int swap = getPointerToWorld()->getPtrToPlayer(1)->getCoins();
            getPointerToWorld()->getPtrToPlayer(1)->setCoins(getPointerToWorld()->getPtrToPlayer(2)->getCoins());
            getPointerToWorld()->getPtrToPlayer(2)->setCoins(swap);
        }
        else { //swap players' stars
            int swap = getPointerToWorld()->getPtrToPlayer(1)->getStars();
            getPointerToWorld()->getPtrToPlayer(1)->setStars(getPointerToWorld()->getPtrToPlayer(2)->getStars());
            getPointerToWorld()->getPtrToPlayer(2)->setStars(swap);
        }
        getPointerToWorld()->playSound(SOUND_BOO_ACTIVATE);
        setDidDifferentiatedAction(true); //set that boo already activated so dont activate again
    }
    else
        setDidDifferentiatedAction(false);
}

void Boo::doSpecialPower() {
}


Bowser::Bowser(StudentWorld* worldPtr, int startX, int startY)
:  Baddie(worldPtr, IID_BOWSER, startX, startY, 10) {
}

void Bowser::doDifferentiatedAction() {
    if (getWaitingStatus() == true) { //its in paused state
        if (getPointerToWorld()->getPtrToPlayer(1)->getX() == this->getX() && getPointerToWorld()->getPtrToPlayer(1)->getY() == this->getY() && getPointerToWorld()->getPtrToPlayer(1)->getWaitingStatus() == true) { //if bowser is on same square as peach
            
            if (getDidDifferentiatedAction() == true)
                return;
            
            srand((unsigned) time(NULL));
            int die_roll = rand() % 2;
            if (die_roll == 0) { //take away player stars and coins
                getPointerToWorld()->getPtrToPlayer(1)->setCoins(0);
                getPointerToWorld()->getPtrToPlayer(1)->setStars(0);
                getPointerToWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            }
            setDidDifferentiatedAction(true); //set that boo already activated so dont activate again

        } else if (getPointerToWorld()->getPtrToPlayer(2)->getX() == this->getX() && getPointerToWorld()->getPtrToPlayer(2)->getY() == this->getY() && getPointerToWorld()->getPtrToPlayer(2)->getWaitingStatus() == true) { //if bowser is on same square as yoshi
            
            if (getDidDifferentiatedAction() == true)
                return;
            
            srand((unsigned) time(NULL));
            int die_roll = rand() % 2;
            if (die_roll == 0) { //take away player stars and coins
                getPointerToWorld()->getPtrToPlayer(2)->setCoins(0);
                getPointerToWorld()->getPtrToPlayer(2)->setStars(0);
                getPointerToWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            }
            
            setDidDifferentiatedAction(true); //set that boo already activated so dont activate again

        }
    }
    else
        setDidDifferentiatedAction(false);
}

void Bowser::doSpecialPower() {
    srand((unsigned) time(NULL));
    int die_roll = rand() % 4;
    if (die_roll == 0) { //25% chance (1/4)
        getPointerToWorld()->placeDroppingSquare(getX(), getY());
        getPointerToWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
}


    



Vortex::Vortex(StudentWorld* worldPtr, int startX, int startY, int startDir)
: Entity(worldPtr, IID_VORTEX, startX, startY, 0, 0)
{
    setAliveStatus(true);
    moveTo(startX, startY);
    setMovingDirection(startDir);
}

void Vortex::doSomething() {
    if (amIAlive() == false) //immediately return if inactive
        return;
    
    moveAtAngle(getMovingDirection(), 2); //move 2 pixels in the walk direction

    if ((getX() < 0 || getX() >= VIEW_WIDTH) || (getY() < 0 || getY() >= VIEW_HEIGHT)) //if it leaves the screen
        setAliveStatus(false); //set it to dead/inactive
    
    Actor* collidee = nullptr;
    if (getPointerToWorld()->isThereAnActorToCollideWith(collidee, getX(), getY()) == true) { //if vortex can collide with a baddie
        collidee->setImpacted(true); //inform the other object it got impacted
        setAliveStatus(false); //set to dead/inactive
        getPointerToWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}








Square::Square(StudentWorld* worldPtr, int imageID, int startX, int startY, int startDir)
: Actor(worldPtr, imageID, startX, startY, startDir, 1, 1), peachIsHere(false), yoshiIsHere(false)
{
    moveTo(startX*16, startY*16);
    setOccupability(true);
}

void Square::doSomething() {
    if (amIAlive() == false)
        return;
    
    switch (whoIsHere()) {
        case -1:
            break;
        case 1:
                differentiatedSquareAction(1);
            break;
        case 2:
                differentiatedSquareAction(2);
            break;
        case 3:
                differentiatedSquareAction(1);
                differentiatedSquareAction(2);
            break;
    }
    
    setPlayersHere(whoIsHere()); //set square's data members to show that a player is currently on it, so it can't receive effects from the square in the next tick
}

int Square::whoIsHere() {
    int returnVal = 0;
    if (getPointerToWorld()->getPtrToPlayer(1)->getX() == this->getX() && getPointerToWorld()->getPtrToPlayer(1)->getY() == this->getY())
        returnVal += 1;
    if (getPointerToWorld()->getPtrToPlayer(2)->getX() == this->getX() && getPointerToWorld()->getPtrToPlayer(2)->getY() == this->getY())
        returnVal += 2;
    
    if (returnVal == 0)
        return -1;
    else
        return returnVal;
}

bool Square::isPlayerStillHere(int player) {
    if (player == 1)
        return peachIsHere;
    else if (player == 2)
        return yoshiIsHere;
    return -1;
}

void Square::setPlayersHere(int here) {
    if (here == -1) {
        peachIsHere = false;
        yoshiIsHere = false;
    } else if (here == 1){
        peachIsHere = true;
        yoshiIsHere = false;
    } else if (here == 2) {
        peachIsHere = false;
        yoshiIsHere = true;
    } else if (here == 3) {
        peachIsHere = true;
        yoshiIsHere = true;
    }
}

CoinSquare::CoinSquare(StudentWorld* worldPtr, int imageID, int startX, int startY)
: Square(worldPtr, imageID, startX, startY, 0)
{
}

BlueCoinSquare::BlueCoinSquare(StudentWorld* worldPtr, int startX, int startY, bool isPlayerStartingSquare)
: CoinSquare(worldPtr, IID_BLUE_COIN_SQUARE, startX, startY)
{
    if (isPlayerStartingSquare == true) //this prevents the players from starting off with 3 coins
        setPlayersHere(3);
}

void BlueCoinSquare::differentiatedSquareAction(int player) {
    if (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) {
        getPointerToWorld()->getPtrToPlayer(player)->setCoins(getPointerToWorld()->getPtrToPlayer(player)->getCoins() + 3);
        getPointerToWorld()->playSound(SOUND_GIVE_COIN);
    }
}

void RedCoinSquare::differentiatedSquareAction(int player) {
    if (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) {
        if (getPointerToWorld()->getPtrToPlayer(player)->getCoins() < 3) //if they have less than 3 coins
            getPointerToWorld()->getPtrToPlayer(player)->setCoins(0); //reduce their coins to 0
        else
            getPointerToWorld()->getPtrToPlayer(player)->setCoins(getPointerToWorld()->getPtrToPlayer(player)->getCoins() - 3);
        getPointerToWorld()->playSound(SOUND_TAKE_COIN);
    }
}

RedCoinSquare::RedCoinSquare(StudentWorld* worldPtr, int startX, int startY)
: CoinSquare(worldPtr, IID_RED_COIN_SQUARE, startX, startY)
{
}

DirectionSquare::DirectionSquare(StudentWorld* worldPtr, int startX, int startY, int startDir)
: Square(worldPtr, IID_DIR_SQUARE, startX, startY, startDir)
{
}

void DirectionSquare::differentiatedSquareAction(int player) {
    if ((getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) || (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == false)) { //EITHER landed on this square OR is passing by
        getPointerToWorld()->getPtrToPlayer(player)->setDirectionSquareForced(getDirection());
        getPointerToWorld()->getPtrToPlayer(player)->setForcing(true);
//        getPointerToWorld()->getPtrToPlayer(player)->setMovingDirection(getDirection());
//        getPointerToWorld()->getPtrToPlayer(player)->setDirection(getDirection() == left ? left : right);
    }
}


EventSquare::EventSquare(StudentWorld* worldPtr, int startX, int startY)
: Square(worldPtr, IID_EVENT_SQUARE, startX, startY, 0)
{
}

void EventSquare::differentiatedSquareAction(int player) {
    if (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) {
        srand((unsigned) time(NULL));
        int die_roll = 1 + rand() % 3;
                
        if (die_roll == 1) { // teleport player to random square
            int teleportX, teleportY;
            getPointerToWorld()->getRandomSquare(teleportX, teleportY);
            getPointerToWorld()->getPtrToPlayer(player)->moveTo(teleportX, teleportY);
            
            getPointerToWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else if (die_roll == 2) { //switch player's locations
            int swapX = getPointerToWorld()->getPtrToPlayer(player)->getX();
            int swapY = getPointerToWorld()->getPtrToPlayer(player)->getY();
            int swapDir = getPointerToWorld()->getPtrToPlayer(player)->getDirection();
            int swapMovingDir = getPointerToWorld()->getPtrToPlayer(player)->getMovingDirection();
            int swapTicks = getPointerToWorld()->getPtrToPlayer(player)->get_ticks_to_move();
            bool swapWaiting = getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus();
            int swapProgress = getPointerToWorld()->getPtrToPlayer(player)->getProgressTowardsNext();
            
            int otherPlayer = (player == 1 ? 2 : 1);
            
            //set this player's values to the other's
            getPointerToWorld()->getPtrToPlayer(player)->moveTo(getPointerToWorld()->getPtrToPlayer(otherPlayer)->getX(), getPointerToWorld()->getPtrToPlayer(otherPlayer)->getY());
            getPointerToWorld()->getPtrToPlayer(player)->set_ticks_to_move(getPointerToWorld()->getPtrToPlayer(otherPlayer)->get_ticks_to_move());
            getPointerToWorld()->getPtrToPlayer(player)->setMovingDirection(getPointerToWorld()->getPtrToPlayer(otherPlayer)->getMovingDirection());
            getPointerToWorld()->getPtrToPlayer(player)->setDirection(getPointerToWorld()->getPtrToPlayer(otherPlayer)->getDirection());
            getPointerToWorld()->getPtrToPlayer(player)->setWaitingStatus(getPointerToWorld()->getPtrToPlayer(otherPlayer)->getWaitingStatus());
            getPointerToWorld()->getPtrToPlayer(player)->setProgressTowardsNext(getPointerToWorld()->getPtrToPlayer(otherPlayer)->getProgressTowardsNext());
            
            //set the other player's values to this one's (swap's)
            getPointerToWorld()->getPtrToPlayer(otherPlayer)->moveTo(swapX, swapY);
            getPointerToWorld()->getPtrToPlayer(otherPlayer)->set_ticks_to_move(swapTicks);
            getPointerToWorld()->getPtrToPlayer(otherPlayer)->setMovingDirection(swapMovingDir);
            getPointerToWorld()->getPtrToPlayer(otherPlayer)->setDirection(swapDir);
            getPointerToWorld()->getPtrToPlayer(otherPlayer)->setWaitingStatus(swapWaiting);
            getPointerToWorld()->getPtrToPlayer(otherPlayer)->setProgressTowardsNext(swapProgress);
            this->setPlayersHere(otherPlayer); //prevent the other player from reactivating the event square
            
            getPointerToWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else { //it rolled 3
            if (getPointerToWorld()->getPtrToPlayer(player)->vortexCount() == 0) //they don't have a vortex
                getPointerToWorld()->getPtrToPlayer(player)->setVortexCount(1); //only if so, then give them a vortex
            getPointerToWorld()->playSound(SOUND_GIVE_VORTEX);
        }
    }
}

BankSquare::BankSquare(StudentWorld* worldPtr, int startX, int startY)
: Square(worldPtr, IID_BANK_SQUARE, startX, startY, 0)
{
}

void BankSquare::differentiatedSquareAction(int player) {
    if (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) { //LANDED on this square (cash out)
        getPointerToWorld()->getPtrToPlayer(player)->setCoins(getPointerToWorld()->getPtrToPlayer(player)->getCoins() + getPointerToWorld()->getBankAmount());
        getPointerToWorld()->setBankAmount(0); //empty the bank
        getPointerToWorld()->playSound(SOUND_WITHDRAW_BANK);
    }
    else if (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == false) { //just passing by (deduct up to 5 coins from player)
        if (getPointerToWorld()->getPtrToPlayer(player)->getCoins() < 5) {//if player has less than 5 coins
            int amtToAddToBank = getPointerToWorld()->getPtrToPlayer(player)->getCoins();
            getPointerToWorld()->getPtrToPlayer(player)->setCoins(0); //reduce their coins to 0
            getPointerToWorld()->setBankAmount(getPointerToWorld()->getBankAmount() + amtToAddToBank); //add what was taken from player to bank
        }
        else {
            getPointerToWorld()->getPtrToPlayer(player)->setCoins(getPointerToWorld()->getPtrToPlayer(player)->getCoins() - 5);
            getPointerToWorld()->setBankAmount(getPointerToWorld()->getBankAmount() + 5); //add 5 coins to bank
        }
        getPointerToWorld()->playSound(SOUND_DEPOSIT_BANK);
    }
}

StarSquare::StarSquare(StudentWorld* worldPtr, int startX, int startY)
: Square(worldPtr, IID_STAR_SQUARE, startX, startY, 0)
{
}

void StarSquare::differentiatedSquareAction(int player) {
    if ((getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) || (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == false)) { //EITHER landed on this square OR is passing by
        if (getPointerToWorld()->getPtrToPlayer(player)->getCoins() < 20)
            return;
        else {
            getPointerToWorld()->getPtrToPlayer(player)->setCoins(getPointerToWorld()->getPtrToPlayer(player)->getCoins()-20);
            getPointerToWorld()->getPtrToPlayer(player)->setStars(getPointerToWorld()->getPtrToPlayer(player)->getStars()+1);
            getPointerToWorld()->playSound(SOUND_GIVE_STAR);
        }
    }
}

DroppingSquare::DroppingSquare(StudentWorld* worldPtr, int startX, int startY)
: Square(worldPtr, IID_DROPPING_SQUARE, startX, startY, 0)
{
}

void DroppingSquare::differentiatedSquareAction(int player) {
    if (getPointerToWorld()->getPtrToPlayer(player)->getWaitingStatus() == true && isPlayerStillHere(player) == false) { //LANDED on this square
        srand((unsigned) time(NULL));
        int die_roll = rand() % 2;
        
        if (die_roll == 1) {
            if (getPointerToWorld()->getPtrToPlayer(player)->getCoins() < 10) //if they have less than 3 coins
                getPointerToWorld()->getPtrToPlayer(player)->setCoins(0); //reduce their coins to 0
            else
                getPointerToWorld()->getPtrToPlayer(player)->setCoins(getPointerToWorld()->getPtrToPlayer(player)->getCoins() - 10);
        }
        else { //die_roll == 2
            if (getPointerToWorld()->getPtrToPlayer(player)->getStars() > 0) //if they have at least 1 star
                getPointerToWorld()->getPtrToPlayer(player)->setStars(getPointerToWorld()->getPtrToPlayer(player)->getStars() - 1); //subtract 1 star
        }
        getPointerToWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
    }
}

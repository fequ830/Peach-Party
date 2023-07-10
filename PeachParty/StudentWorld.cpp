#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

//void setGameStatText(string text);
//bool getAction(int playerNum);
//void playSound(int soundID);
//int getBoardNumber() const;
//void setFinalScore(int stars, int coins);
//void startCountDownTimer(int numSeconds);
//int timeRemaining() const;
//string assetPath() const;


//GraphObject(int imageID, double startX, double startY,
//int startDirection = right, int depth = 0, double size = 1.0);
//int getX() const; // in pixels (0-255)
//int getY() const; // in pixels (0-255)
//void moveTo(int x, int y); // in pixels (0-255)
//int getDirection() const; // sprite angle in degrees (0-359)
//void setDirection(Direction d); // sprite angle in degrees (0-359)
//void getPositionInThisDirection(int angle, int distance, int& newX, int& newY) const;
//void moveAtAngle(int angle, int distance);


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), bankAmount(0)
{
}

int StudentWorld::init()
{
    Board bd;
    
    string boardFile = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = bd.loadBoard(boardFile);
    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        
        Board::GridEntry ge;
        
        for (int i = 0; i < 16; i++) {
            for (int k = 0; k < 16; k++) {
                ge = bd.getContentsOf(i, k);

                switch (ge) {
                    case Board::empty:
                        break;
                    case Board::boo:
                        actorVect.push_back(new Boo(this, i, k));
                        actorVect.push_back(new BlueCoinSquare(this, i, k, false));
                        break;
                    case Board::bowser:
                        actorVect.push_back(new Bowser(this, i, k));
                        actorVect.push_back(new BlueCoinSquare(this, i, k, false));
                        break;
                    case Board::player:
                        ptrToPeach = new Peach(this, i, k);
                        ptrToYoshi = new Yoshi(this, i, k);
                        actorVect.push_back(new BlueCoinSquare(this, i, k, true));
                        break;
                    case Board::red_coin_square:
                        actorVect.push_back(new RedCoinSquare(this, i, k));
                        break;
                    case Board::blue_coin_square:
                        actorVect.push_back(new BlueCoinSquare(this, i, k, false));
                        break;
                    case Board::up_dir_square:
                        actorVect.push_back(new DirectionSquare(this, i, k, Actor::up));
                        break;
                    case Board::down_dir_square:
                        actorVect.push_back(new DirectionSquare(this, i, k, Actor::down));
                        break;
                    case Board::left_dir_square:
                        actorVect.push_back(new DirectionSquare(this, i, k, Actor::left));
                        break;
                    case Board::right_dir_square:
                        actorVect.push_back(new DirectionSquare(this, i, k, Actor::right));
                        break;
                    case Board::event_square:
                        actorVect.push_back(new EventSquare(this, i, k));
                        break;
                    case Board::bank_square:
                        actorVect.push_back(new BankSquare(this, i, k));
                        break;
                    case Board::star_square:
                        actorVect.push_back(new StarSquare(this, i, k));
                        break;
                }
                
            }
        }
    }
    
	startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

//    // The term "actors" refers to all actors, e.g., Peach, Yoshi,
//    // baddies, squares, vortexes, etc.
//    // Give each actor a chance to do something, incl. Peach and Yoshi
    ptrToPeach->doSomething(1);
    ptrToYoshi->doSomething(2);
    for (vector<Actor*>::iterator iter = actorVect.begin(); iter != actorVect.end(); iter++) {
        if ((*iter)->amIAlive())
            (*iter)->doSomething();
    }

    //remove inactive/dead game objects
    for (vector<Actor*>::iterator iter = actorVect.begin(); iter != actorVect.end(); iter++) { //FIXME: I FEEL LIKE THIS IS IMPLEMENTED WRONG
        if ((*iter)->amIAlive() == false) {
            delete *iter;
            iter = actorVect.erase(iter);
            iter--;
        }
    }


    // Update the Game Status Line
    ostringstream oss;
    oss << "P1 Roll: " << to_string(ptrToPeach->getRollNumber()) << " Stars: " << to_string(ptrToPeach->getStars()) << " $$: " << to_string(ptrToPeach->getCoins());
    if (ptrToPeach->vortexCount() > 0)
        oss <<  " VOR";
    oss << " | Time: " << to_string(timeRemaining()) << " | Bank: " << to_string(bankAmount) << " | ";
    oss << "P2 Roll: " << to_string(ptrToYoshi->getRollNumber()) << " Stars: " << to_string(ptrToYoshi->getStars()) << " $$: " << to_string(ptrToYoshi->getCoins());
    if (ptrToYoshi->vortexCount() > 0)
        oss <<  " VOR";
    setGameStatText(oss.str()); // update the coins/stars stats text at screen top
    
    
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED); //play end of game sound
        
        if (ptrToYoshi->getStars() > ptrToPeach->getStars()) { //yoshi has more stars
            setFinalScore(ptrToYoshi->getStars(), ptrToYoshi->getCoins());
            return GWSTATUS_YOSHI_WON;
        }
        else if (ptrToPeach->getStars() > ptrToYoshi->getStars()) {// peach has more stars
            setFinalScore(ptrToPeach->getStars(), ptrToPeach->getCoins());
            return GWSTATUS_PEACH_WON;
        }
        else if (ptrToYoshi->getCoins() > ptrToPeach->getCoins()) { //yoshi has more coins
            setFinalScore(ptrToYoshi->getStars(), ptrToYoshi->getCoins());
            return GWSTATUS_YOSHI_WON;
        }
        else if (ptrToPeach->getCoins() > ptrToYoshi->getCoins()) { //peach has more coins
            setFinalScore(ptrToPeach->getStars(), ptrToPeach->getCoins());
            return GWSTATUS_PEACH_WON;
        } else { //both are tied, so choose random winner
            srand((unsigned) time(NULL));
            int random = rand() % 2; //FIXME: THIS MAY BE WRONG
            if (random == 1) {
                setFinalScore(ptrToPeach->getStars(), ptrToPeach->getCoins());
                return GWSTATUS_PEACH_WON;
            } else {
                setFinalScore(ptrToYoshi->getStars(), ptrToYoshi->getCoins());
                return GWSTATUS_YOSHI_WON;
            }
        }
    }
    
    // the game isn't over yet so continue playing
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (vector<Actor*>::iterator iter = actorVect.begin(); iter != actorVect.end(); iter++) {
        delete *iter;
    }
    actorVect.clear();
    
    delete ptrToYoshi;
    delete ptrToPeach;
}

bool StudentWorld::isMovable(int x, int y) {
    
    for (vector<Actor*>::iterator iter = actorVect.begin(); iter != actorVect.end(); iter++) {
        if ((*iter)->getY() == y && (*iter)->getX() == x && (*iter)->canBeOccupied() == true)
            return true; //there exists a square that can be moved onto in the specified direction
    }
    return false;
}

Player* StudentWorld::getPtrToPlayer(int player) {
    if (player == 1) //peach
        return ptrToPeach;
    else if (player == 2) //yoshi
        return ptrToYoshi;
    else
        return nullptr;
}


//void StudentWorld::directionToMoveIn(Actor* ptrToActor, int x, int y, bool onlyPerpendicular) {
//    int checkX, checkY;
//    ptrToActor->getPositionInThisDirection(ptrToActor->getMovingDirection(), ptrToActor->getProgressTowardsNext(), checkX, checkY); //see if it's possible to move 1 square in the current facing direction
//    if (isMovable(checkX, checkY) == false) { //if the player CANNOT move in the current direction
//        bool firstPossible = false, secondPossible = false;
//        int chosenDir = 0;
//        int firstPerp = 0, secondPerp = 0;
//        switch (ptrToActor->getMovingDirection()) {
//            case Actor::up:
//                firstPerp = Actor::right;
//                secondPerp = Actor::left;
//                break;
//            case Actor::down:
//                firstPerp = Actor::right;
//                secondPerp = Actor::left;
//                break;
//            case Actor::left:
//                firstPerp = Actor::up;
//                secondPerp = Actor::down;
//                break;
//            case Actor::right:
//                firstPerp = Actor::up;
//                secondPerp = Actor::down;
//                break;
//        }
//
//        ptrToActor->getPositionInThisDirection(firstPerp, ptrToActor->getProgressTowardsNext(), checkX, checkY); //check one perpendicular direction
//        if (isMovable(checkX, checkY) == true) {
//            firstPossible = true;
//        }
//
//        ptrToActor->getPositionInThisDirection(secondPerp, ptrToActor->getProgressTowardsNext(), checkX, checkY); //and check the other perpendicular direction
//        if (isMovable(checkX, checkY) == true) {
//            secondPossible = true;
//        }
//
//        if (firstPossible == true)
//            chosenDir = firstPerp;
//        else if (secondPossible == true)
//            chosenDir = secondPerp;
//        else if (onlyPerpendicular == false)
//            chosenDir = abs(ptrToActor->getMovingDirection()-180); //if we're allowed to go in the opposite direction we came from, then do so
//
//        ptrToActor->setMovingDirection(chosenDir);
//        ptrToActor->setDirection(chosenDir == Actor::left ? Actor::left : Actor::right); //if it will face left then set its direction to left; otherwise set to right
//    }
//}


void StudentWorld::introduceVortex(StudentWorld* worldPtr, int startX, int startY, int startDir) {
    actorVect.push_back(new Vortex(worldPtr, startX, startY, startDir));
}

//StudentWorld::~StudentWorld() {
//    cleanUp();
//}


bool StudentWorld::isThereAnActorToCollideWith(Actor*& collideActor, int x, int y) {
    for (vector<Actor*>::iterator iter = actorVect.begin(); iter != actorVect.end(); iter++) {
        if ((*iter)->isHittableByVector() == true) { //is a baddie
            if (abs((*iter)->getY() - y) < 16 && abs((*iter)->getX() - x) < 16) { //if overlap
                collideActor = *iter;
                return true;
            }
        }
    }
    return false;
}


int StudentWorld::getBankAmount() {
    return bankAmount;
}

void StudentWorld::setBankAmount(int amt) {
    bankAmount = amt;
}

void StudentWorld::getRandomSquare(int& x, int& y) {
    srand((unsigned) time(NULL));
    for (;;) {
        int randIndex = rand() % actorVect.size();
        if (actorVect[randIndex]->canBeOccupied() == true) { //if this actor is a square
            x = actorVect[randIndex]->getX();
            y = actorVect[randIndex]->getY();
            return;
        }
    }
}

bool StudentWorld::isEntityAtFork(Entity* entity) {
    int newX, newY;
    int movingDir = entity->getMovingDirection();
    int firstPerp, secondPerp; //the two perpendicular directions
        
    switch (movingDir) { //sets the perpendicular directions of the entity's MOVING direction
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
    
    int validPaths = 0;
    entity->getPositionInThisDirection((movingDir == 90 ? 270 : abs(movingDir-180)), 16, newX, newY); //the opposite direction
    if (isMovable(newX, newY) == true)
        validPaths++;
    entity->getPositionInThisDirection(movingDir, 16, newX, newY);
    if (isMovable(newX, newY) == true)
        validPaths++;
    entity->getPositionInThisDirection(firstPerp, 16, newX, newY);
    if (isMovable(newX, newY) == true)
        validPaths++;
    entity->getPositionInThisDirection(secondPerp, 16, newX, newY);
    if (isMovable(newX, newY) == true)
        validPaths++;
    if (validPaths > 2)
        return true;
    return false;
}

void StudentWorld::placeDroppingSquare(int x, int y) {
    for (vector<Actor*>::iterator iter = actorVect.begin(); iter != actorVect.end(); iter++) {
        if ((*iter)->canBeOccupied() == true && (*iter)->getX() == x && (*iter)->getY() == y) {
            delete *iter; //delete the square that bowser is currently on
            actorVect.erase(iter);
            actorVect.push_back(new DroppingSquare(this, x/16, y/16)); //add dropping square
            return;
        }
    }
}

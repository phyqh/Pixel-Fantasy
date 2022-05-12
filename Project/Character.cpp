#include "Character.h"
const float PLAYER_VELOCITY(1.0f);
#define PI 3.14159265
Character::Character(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName) :
    Sprite(position, size, rotation, textureName),
    walkCount(0), idleCount(0), attackCount(0), blockCount(0), dizzyCount(0),
    health(100), attack(100), defense(0.2), lastFaceIndex(0)
{
}



void Character::Idle(float dt, std::vector<std::string> idleSequence) {
    idleCount += 8 * dt;
    this->setTextureName(idleSequence[int(idleCount) % idleSequence.size()]);
}

void Character::Attack(float frame, std::vector<std::string> attackSequence) {
    attackCount = frame;
    this->setTextureName(attackSequence[int(attackCount) % attackSequence.size()]);
}

void Character::Block(float dt, std::vector<std::string> blockSequence) {
    blockCount += 8 * dt;
    this->setTextureName(blockSequence[int(blockCount) % blockSequence.size()]);
}

void Character::Dizzy(float dt, std::vector<std::string> dizzySequence) {
    blockCount += 8 * dt;
    this->setTextureName(dizzySequence[int(dizzyCount) % dizzySequence.size()]);
}

void Character::Walk(sf::Keyboard::Key e, float dt, float camZ, float camX, std::vector<std::string> walkSequence, int isAnimation) {

    float velocity = 0;


    if (isAnimation == 0) {
        velocity = PLAYER_VELOCITY * dt;
        walkCount += 10 * dt;
    }
    else if (isAnimation == 1) {
        velocity = PLAYER_VELOCITY * 0.03;
        walkCount = dt;
    }
    else if (isAnimation == 2) {
        velocity = 0;
        walkCount += 10 * dt; // only animation, no translation, easier controlling of AI
    }
    else if (isAnimation == 3) {
        velocity = 0.1;
        walkCount += 5 * dt; // only animation, no translation, easier controlling of AI
    }
    else if (isAnimation == 4) { // only set orientation
        velocity = 0;
    }

   

    float posX, posY, posZ;
    posX = getPosition().x;
    posY = getPosition().y;
    posZ = getPosition().z;
    
    this->setTextureName(walkSequence[int(walkCount) % walkSequence.size()]);
    
    /*double direction = getDirection();*/

    double direction = atan(((camX - posX)) / ((camZ - posZ))) * 180.0 / PI;

    int UpDownSignZ = 1;
    int UpDownSignX = 1;
    if (camZ < posZ)
        UpDownSignZ = -1;
    if (camX < posX)
        UpDownSignX = -1;

    int RightLeftSign = 1;

    if ((camZ > posZ && direction < 45) || (camZ < posZ && direction > 45))
        RightLeftSign = -1;
    

    if (RightLeftSign == -1) {
        switch (e) {
        case sf::Keyboard::Up:
            if (direction > -45 && direction < 45) {
                posZ -= UpDownSignZ * velocity;
            }
            // > 45 || < -45
            else
            {
                posX -= UpDownSignX * velocity;
            }
            break;
        case sf::Keyboard::Down:
            if (direction > -45 && direction < 45) {
                posZ += UpDownSignZ * velocity;

            }
            else {
                posX += UpDownSignX * velocity;
            }
            break;
        case sf::Keyboard::Left:
            if (direction > -45 && direction < 45) {
                posX -= velocity;
                setRotation(glm::radians(direction + 180.0));
            }
            else if (direction > 45) {
                posZ -= velocity;
                setRotation(glm::radians(direction + 0.0));
            }
            else {
                posZ -= velocity;
                setRotation(glm::radians(direction + 180.0));
            }
            break;
        case sf::Keyboard::Right:
            if (direction > -45 && direction < 45) {
                posX += velocity;
                setRotation(glm::radians(direction + 0.0));
            }
            else if (direction > 45) {
                posZ += velocity;
                setRotation(glm::radians(direction - 180.0));
            }
            else {
                posZ += velocity;
                setRotation(glm::radians(direction - 0.0));
            }
            break;
        default:
            break;
        }
    }
    else {
        switch (e) {
        case sf::Keyboard::Up:
            if (direction > -45 && direction < 45) {
                posZ -= UpDownSignZ * velocity;
            }
            // > 45 || < -45
            else
            {
                posX -= UpDownSignX * velocity;
            }
            break;
        case sf::Keyboard::Down:
            if (direction > -45 && direction < 45) {
                posZ += UpDownSignZ * velocity;

            }
            else {
                posX += UpDownSignX * velocity;
            }
            break;
        case sf::Keyboard::Right:
            if (direction > -45 && direction < 45) {
                posX -= velocity;
                setRotation(glm::radians(direction + 180.0));
            }
            else if (direction > 45) {
                posZ -= velocity;
                setRotation(glm::radians(direction + 0.0));
            }
            else {
                posZ -= velocity;
                setRotation(glm::radians(direction + 180.0));
            }
            break;
        case sf::Keyboard::Left:
            if (direction > -45 && direction < 45) {
                posX += velocity;
                setRotation(glm::radians(direction + 0.0));
            }
            else if (direction > 45) {
                posZ += velocity;
                setRotation(glm::radians(direction - 180.0));
            }
            else {
                posZ += velocity;
                setRotation(glm::radians(direction - 0.0));
            }
            break;
        default:
            break;
        }
    }
   
    
    float groundTruth = PhysicsEngine::computeGroundHeight(glm::vec3(posX, posY + 1, posZ), lastFaceIndex);
    glm::vec3 newPosition = glm::vec3(posX, groundTruth + (this->getSize()[0] / 2.08) - 0.027, posZ);
    if (PhysicsEngine::checkCollision(newPosition)) return;
    setPosition(newPosition);
};

void Character::AIWalk(sf::Keyboard::Key e, float dt, float camZ, float camX, std::vector<std::string> walkSequence, int isAnimation) {
    float velocity = 0;


    if (isAnimation == 0) {
        velocity = PLAYER_VELOCITY * dt;
        walkCount += 10 * dt;
    }
    else if (isAnimation == 1) {
        velocity = PLAYER_VELOCITY * 0.03;
        walkCount = dt;
    }
    else if (isAnimation == 2) {
        velocity = 0;
        walkCount += 10 * dt; // only animation, no translation, easier controlling of AI
    }
    else if (isAnimation == 3) {
        velocity = PLAYER_VELOCITY * 0.1;
        walkCount = 10 * dt;
    }
    else if (isAnimation == 4) {
        velocity = PLAYER_VELOCITY * 0.05;
        walkCount = dt;
    }


    float posX, posY, posZ;
    posX = getPosition().x;
    posY = getPosition().y;
    posZ = getPosition().z;

    this->setTextureName(walkSequence[int(walkCount) % walkSequence.size()]);

    /*double direction = getDirection();*/

    double direction = atan(((camX - posX)) / ((camZ - posZ))) * 180.0 / PI;

    switch (e) {
        case sf::Keyboard::Up:
            posZ -= velocity;
            break;
        case sf::Keyboard::Down:
            posZ += velocity;
            break;
        case sf::Keyboard::Left:
                posX -= velocity;
                setRotation(glm::radians(getDirection() + 180.0));
            break;
        case sf::Keyboard::Right:
                posX += velocity;
                setRotation(glm::radians(getDirection() + 0.0));
            break;
        default:
            break;
    }

    float groundTruth = PhysicsEngine::computeGroundHeight(glm::vec3(posX, posY + 1, posZ), lastFaceIndex);
    glm::vec3 newPosition = glm::vec3(posX, groundTruth + (this->getSize()[0] / 2) - 0.047, posZ);
    if (PhysicsEngine::checkCollision(newPosition)) return;
    setPosition(newPosition);
}

void Character::setHealth(double newHealth) {
    health = newHealth;
}
double const Character::getHealth() {
    return health;
}


void Character::setStamina(double newStamina) {
    if (newStamina <= 100 && newStamina >= 0)
        stamina = newStamina;
    else if (newStamina > 100)
        stamina = 100;
    else
        stamina = 0;
}

bool const Character::Alive() {
    return (health > 0);
}

// Manhattan distance
double const Character::getDistance(Character* anotherChar) {
    return (abs(this->getPosition().x - anotherChar->getPosition().x) + abs(this->getPosition().z - anotherChar->getPosition().z));
}
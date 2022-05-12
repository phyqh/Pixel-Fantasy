#ifndef CHARACTER_HEADER
#define CHARACTER_HEADER

#include "Libs.h"
#include "Sprite.h"
#include "PhysicsEngine.h"

class Character : public Sprite
{
private:
	double walkCount;
	double idleCount;
	double attackCount;
	double blockCount;
	double dizzyCount;
	double stamina = 100;
	int staminaState = 1;
	double health; // 0 ~ 100
	double attack; // 0 ~ 100
	double defense; // 0.0 ~ 1.0

	int id; // 0 for player, 1 for ordinary monster, 2 for boss

	unsigned int lastFaceIndex;

public:
	// constructor
	Character(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName);
	
	// Animation
	void Walk(sf::Keyboard::Key e, float dt, float camX, float camZ, std::vector<std::string> walkSequence, int isAnimation = 0);
	void AIWalk(sf::Keyboard::Key e, float dt, float camX, float camZ, std::vector<std::string> walkSequence, int isAnimation = 0);
	void Idle(float dt, std::vector<std::string> idleSequence);
	void Attack(float frame, std::vector<std::string> attackSequence);
	void Block(float dt, std::vector<std::string> blockSequence);
	void Dizzy(float dt, std::vector<std::string> dizzySequence);
	
	// Accessors
	bool const Alive();
	double const getHealth();
	double const getDistance(Character* anotherChar);
	double const getStamina() { return stamina; }
	int getStaminaState() { return staminaState; }
	unsigned int getlastFaceIndex() { return lastFaceIndex; }
	int const getid() { return id; }

	// Mutators
	void setIdleCount(int newIdleCount) { idleCount = newIdleCount; }
	void setHealth(double newHealth);
	void setStamina(double newStamina);
	void setid(int newID) { id = newID; }
	void setStaminaState(int newStaminaState) { staminaState = newStaminaState; }
	std::vector<std::string> walkSequence2 = { "Dwalk1", "Dwalk2", "Dwalk3", "Dwalk4", "Dwalk5", "Dwalk6" };
	std::vector<std::string> idleSequence2 = { "Didle1", "Didle2", "Didle3", "Didle4", "Didle5", "Didle6" };
	std::vector<std::string> attackSequence2 = { "Dattack1", "Dattack2", "Dattack3" };
	std::vector<std::string> dizzySequence2 = { "Ddizzy1", "Ddizzy2", "Ddizzy3" };
	std::vector<std::string> strikeSequence2 = { "Dstrike1", "Dstrike2", "Dstrike3" };
	std::vector<std::string> dieSequence2 = { "Ddie1", "Ddie2", "Ddie3", "Ddie4", "Ddie5","Ddie6", "Ddie7","Ddie8", "Ddie9", "Ddie10" };
	std::vector<std::string> boomSequence2 = { "Boom1", "Boom2", "Boom3","Boom4","Boom5" };
	std::vector<std::string> winSequence2 = { "Dwin1", "Dwin2" };
};

#endif
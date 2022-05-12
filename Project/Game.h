#pragma once

#include "Libs.h"
#include "RenderMaster.h"
#include "Character.h"
class Game
{
private:
	// variables
	Character* character;
	Character* dcharacter;
	Character* ncharacter;
	// delta time
	sf::Clock dtClock;
	float dt;

	// Music
	sf::Music* bossMusic;
	sf::Music* peaceMusic;
	sf::Music* youDie;
	sf::Music* bossDie;

	// Sound
	sf::Sound* hitSound;
	sf::Sound* swordSound;
	sf::Sound* bossSkill;

	// key time
	float keyTime;
	float keyTimeMax;
	float keyTimeIncrement;

	// window
	const unsigned windowWidth;
	const unsigned windowHeight;
	unsigned int windowFrameLimit;
	sf::ContextSettings windowContextSettings;
	sf::Event windowEvent;
	sf::Window* window;
	// renderer
	RenderMaster* renderMaster;
	Camera camera;

	// control
	bool drag;
	float lastX, lastY;

	// static variables
	int BGM = 0;
	int state = 0; // 0 when noraml, 1 when met boss dragon, 2 when dragon defeated
	int ninjaState = 0;
	int healthUI = 10;
	int staminaUI = 10;

	// private functions
	void initVariables();
	void initWindow();
	void initRenderer();
	void initialize();

public:
	// constructors and destructors
	Game();
	virtual ~Game();

	// accessors
	int getWindowWidth() const { return windowWidth; }
	int getWindowHeight() const { return windowHeight; }
	int getBGM() const { return BGM; }
	int getHealthUI() const { return character->getHealth() / 10; }
	int getStaminaUI() const { return character->getStamina() / 10; }
	int getBossHealthUI() const { return dcharacter->getHealth() / 10; }
	int getNinjaHealthUI() const { return ncharacter->getHealth() / 10; }
	int getState() const { return state; }
	int getNinjaState() const { return ninjaState; }
	// mutators
	void setBGM(int newBGM) { BGM = newBGM; }
	// functions
	bool windowIsOpen();

	// update
	void updateDT();
	void updateKeyTime();
	void updateEvents();
	void updateKeyboardInput();
	void updateFirstPlayerInput();
	void updateSecondPlayerInput();
	bool shiftDash(sf::Keyboard::Key e);
	void update();
	void updateAIMove(Character* enemy);
	void updateAIMove2(Character* enemy, std::vector<std::string> walk, std::vector<std::string> attack);
	void updateWorldObjects();
	void setUI() { staminaUI = character->getStamina() / 10; healthUI = character->getHealth() / 10; };
	void setGameState(int newGameState) { state = newGameState; }
	

	bool CheckPlayerAttack();

	// render
	void render();
	void Animation(float keyTime);
	// static functions

	// Respective sequence of textures for animation of character's movements
	std::vector<std::string> walkSequence = { "walk1", "walk2", "walk3", "walk4", "walk5", "walk6" };
	std::vector<std::string> idleSequence = { "idle1", "idle2", "idle3", "idle4", "idle5", "idle6" };
	std::vector<std::string> attackSequence = { "attack1", "attack2", "attack3", "attack4", "attack5","attack6" };
	std::vector<std::string> blockSequence = { "block1", "block2",  "block3", "block4" };
	std::vector<std::string> dizzySequence = { "hurt1", "hurt2" };
	std::vector<std::string> dashSequence = { "dash2", "dash3" };
	std::vector<std::string> winSequence = { "win1", "win2" };
	std::vector<std::string> dieSequence = { "die1","die2","die3","die4","die5","die6","die7","die8" };

	std::vector<std::string> walkSequence2 = { "Dwalk1", "Dwalk2", "Dwalk3", "Dwalk4", "Dwalk5", "Dwalk6" };
	std::vector<std::string> idleSequence2 = { "Didle1", "Didle2", "Didle3", "Didle4", "Didle5", "Didle6" };
	std::vector<std::string> attackSequence2 = { "Dattack1", "Dattack2", "Dattack3" };
	std::vector<std::string> dizzySequence2 = { "Ddizzy1", "Ddizzy2", "Ddizzy3" };
	std::vector<std::string> strikeSequence2 = { "Dstrike1", "Dstrike2", "Dstrike3" };
	std::vector<std::string> dieSequence2 = { "Ddie1", "Ddie2", "Ddie3", "Ddie4", "Ddie5","Ddie6", "Ddie7","Ddie8", "Ddie9", "Ddie10"};
	std::vector<std::string> boomSequence2 = { "Boom1", "Boom2", "Boom3","Boom4","Boom5"};
	std::vector<std::string> pulseSequence2 = { "Pulse1","Pulse2","Pulse3","Pulse4"};
	std::vector<std::string> winSequence2 = { "Dwin1", "Dwin2" };
	std::vector<std::string> crouchSequence2 = { "Dcrouch2", "Dcrouch3" };

	std::vector<std::string> dizzySequenceN = { "Nidle1","Nidle2","Nidle3" };
	std::vector<std::string> walkSequenceN = {"Nrun1","Nrun2","Nrun3","Nrun4" ,"Nrun5" ,"Nrun6" ,"Nrun7" ,"Nrun8","Nrun9","Nrun10" };
	std::vector<std::string> attackSequenceN = { "Nidle1","Nidle2","Nidle3" };
};


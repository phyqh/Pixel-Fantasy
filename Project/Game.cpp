#include "Game.h"

#define PI 3.14159265


// private functions
void Game::initialize()
{
	// variables
	this->initVariables();

	// window
	this->initWindow();

	// renderer
	this->initRenderer();

	// character
	character = ResourceManager::getCharacter("character1");
	character->Walk(sf::Keyboard::Left, dt, camera.getPosition().z, camera.getPosition().x, walkSequence);

	dcharacter = ResourceManager::getCharacter("character2");
	dcharacter->setid(2);

	ncharacter = ResourceManager::getCharacter("character3");
	ncharacter->Walk(sf::Keyboard::Left, dt, camera.getPosition().z, camera.getPosition().x, walkSequenceN);

	glm::vec3 pos = dcharacter->getPosition();

	ResourceManager::getCharacter("Pulse")->setPosition(glm::vec3(pos.x, pos.y - dcharacter->getSize().x * 0.35, pos.z + 0.5));

	//camera
	camera.focusTranslate(character->getPosition(), 0, 3);

	// Music
	bossMusic = ResourceManager::getMusic("fatalBattle");
	peaceMusic = ResourceManager::getMusic("Peace");
	youDie = ResourceManager::getMusic("youDie");
	bossDie = ResourceManager::getMusic("bossDie");

	bossMusic->setLoop(true);
	peaceMusic->setLoop(true);

	peaceMusic->play();

	// Sound
	hitSound = ResourceManager::getSound("hitted");
	swordSound = ResourceManager::getSound("swordSound");
	bossSkill = ResourceManager::getSound("bossSkill");
}

void Game::initVariables()
{
	// delta Time
	this->dt = 0.f;
	this->dtClock.restart();

	// key Time
	this->keyTime = 0.f;
	this->keyTimeMax = 10000.f;
	this->keyTimeIncrement = 10.f;

	// window
	this->window = nullptr;
	this->windowFrameLimit = 120;

	// control
	this->drag = false;
	this->lastX = windowWidth / 2.0f;
	this->lastY = windowHeight / 2.0f;
}

void Game::initWindow()
{
	windowContextSettings.depthBits = 24;
	windowContextSettings.stencilBits = 8;
	windowContextSettings.antialiasingLevel = 4;
	windowContextSettings.majorVersion = 3;
	windowContextSettings.minorVersion = 0;

	// create window
	this->window = new sf::Window(sf::VideoMode(1920, 1080), "PixelFantasy", sf::Style::Default, windowContextSettings);

	// window Settings
	this->window->setFramerateLimit(this->windowFrameLimit);
	this->window->setMouseCursorVisible(true);
	this->window->setVerticalSyncEnabled(true);

	auto image = sf::Image();
	if (!image.loadFromFile("../Resources/Icons/sword_silver.png"))
		std::cout << "oh no! fail to load icon" << std::endl;
	window->setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}

void Game::initRenderer() {
	this->renderMaster = new RenderMaster(this);

}

// constructors / destructors
Game::Game(): windowWidth(1920), windowHeight(1080)
{
	// init game
	this->initialize();
}

Game::~Game()
{
	// Character resources released as game ends
	delete character;

	if (renderMaster) delete renderMaster;
	renderMaster = nullptr;
}

// public functions
bool Game::windowIsOpen()
{
	return this->window->isOpen();
}

// update
void Game::updateDT()
{
	this->dt = this->dtClock.restart().asSeconds();
#ifdef DEBUG
	std::cout << this->dt << "\n";
#endif
}

void Game::updateKeyTime()
{
	if (this->keyTime < this->keyTimeMax)
		this->keyTime += this->keyTimeIncrement * 0.01;
	else
		this->keyTime = 0.0;
}

void Game::updateEvents()
{
	
	while (this->window->pollEvent(this->windowEvent))
	{	
		if (this->windowEvent.type == sf::Event::Closed) this->window->close();
		else if (this->windowEvent.type == sf::Event::MouseWheelScrolled) camera.processMouseScroll(this->windowEvent.mouseWheelScroll.delta);
		else if (this->windowEvent.type == sf::Event::MouseButtonPressed) {
			if (this->windowEvent.mouseButton.button == sf::Mouse::Middle) {
				drag = true;
				lastX = this->windowEvent.mouseButton.x;
				lastY = this->windowEvent.mouseButton.y;
			}
		}
		else if (this->windowEvent.type == sf::Event::MouseMoved) {
			if (drag) {
				float xoffset = this->windowEvent.mouseMove.x - lastX;
				float yoffset = this->windowEvent.mouseMove.y - lastY;
				lastX = this->windowEvent.mouseMove.x;
				lastY = this->windowEvent.mouseMove.y;

				camera.processMouseMovement(xoffset, yoffset);
			}
		}
		else if (this->windowEvent.type == sf::Event::MouseButtonReleased) {
			if (this->windowEvent.mouseButton.button == sf::Mouse::Middle) {
				drag = false;
			}
		}

	}
}



void Game::updateKeyboardInput()
{
	int cameraMoved = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		this->window->close();

	// Camera Movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		camera.ProcessKeyboard(FORWARD, dt);
		cameraMoved = 1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		camera.ProcessKeyboard(BACKWARD, dt);
		cameraMoved = 1;
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		camera.ProcessKeyboard(LEFT, dt);
		cameraMoved = 1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		camera.ProcessKeyboard(RIGHT, dt);
		cameraMoved = 1;
	}


	// rotate all sprites to face camera
	if (cameraMoved == 1) {
		auto sprites = ResourceManager::getSprites();
		for (auto& pair : sprites) {
			Sprite* sprite = pair.second;
			double changedDirection = 0;
			double camX = camera.getPosition().x;
			double camZ = camera.getPosition().z;
			double spriteX = sprite->getPosition().x;
			double spriteZ = sprite->getPosition().z;
			if (abs(camZ - spriteZ) < 0.1)
				changedDirection = 90.0;
			else
				changedDirection = atan(((camX - spriteX)) / ((camZ - spriteZ))) * 180.0 / PI;
			sprite->setDirection(changedDirection);
			sprite->setRotation(glm::radians(changedDirection));
		}
	}
	camera.focusRotate(character->getPosition()); // Rotate Camera Properly to focus on character as it is hovering around (Pressing A or D)
}


bool Game::shiftDash(sf::Keyboard::Key e) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && character->getStamina() > 0 && character->getStaminaState() == 1) {
		double distX = camera.getPosition().x - character->getPosition().x;
		double distZ = camera.getPosition().z - character->getPosition().z;
		glm::vec3 pos = character->getPosition();
		switch (e) {
		case sf::Keyboard::Left:
			character->Walk(sf::Keyboard::Left, dt, camera.getPosition().z, camera.getPosition().x, dashSequence,3);
			break;
		case sf::Keyboard::Right:
			character->Walk(sf::Keyboard::Right, dt, camera.getPosition().z, camera.getPosition().x, dashSequence, 3);
			break;
		case sf::Keyboard::Up:
			character->Walk(sf::Keyboard::Up, dt, camera.getPosition().z, camera.getPosition().x, dashSequence, 3);
			break;
		case sf::Keyboard::Down:
			character->Walk(sf::Keyboard::Down, dt, camera.getPosition().z, camera.getPosition().x, dashSequence, 3);
			break;
		/*case sf::Keyboard::Left:
			character->setPosition(glm::vec3(pos.x - 0.15, pos.y, pos.z));
			break;
		case sf::Keyboard::Right:
			character->setPosition(glm::vec3(pos.x + 0.15, pos.y, pos.z));
			break;
		case sf::Keyboard::Up:
			character->setPosition(glm::vec3(pos.x, pos.y, pos.z - 0.15));
			break;
		case sf::Keyboard::Down:
			character->setPosition(glm::vec3(pos.x, pos.y, pos.z + 0.15));
			break;*/
		}/*
		character->Walk(e,0, camera.getPosition().z, camera.getPosition().x, walkSequence,2);*/

		/*for (int i = 0; i < 6; i++) {

			character->Walk(e, 0.05, camera.getPosition().z, camera.getPosition().x, walkSequence);

			camera.focusTranslate(character->getPosition(), distX, distZ);
			updateDT();

			updateSecondPlayerInput();

			updateWorldObjects();
			updateEvents();

			render();
		}
		updateAIMove(dcharacter);*/
		return true;
	}
	else {
		if (character->getStamina() < 20)
			character->setStaminaState(0);
		return false;
	}
}

void Game::updateFirstPlayerInput() {
	if (character->getRenderState() == 2) {
		int playerMoved = 0;
		double distX = camera.getPosition().x - character->getPosition().x;
		double distZ = camera.getPosition().z - character->getPosition().z;

		bool dashed = 0;

		// First Player Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			character->Walk(sf::Keyboard::Left, dt, camera.getPosition().z, camera.getPosition().x, walkSequence);
			playerMoved = 1;
			dashed = shiftDash(sf::Keyboard::Left);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			character->Walk(sf::Keyboard::Right, dt, camera.getPosition().z, camera.getPosition().x, walkSequence);
			playerMoved = 1;
			dashed = shiftDash(sf::Keyboard::Right);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			character->Walk(sf::Keyboard::Up, dt, camera.getPosition().z, camera.getPosition().x, walkSequence);
			playerMoved = 1;
			dashed = shiftDash(sf::Keyboard::Up);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			character->Walk(sf::Keyboard::Down, dt, camera.getPosition().z, camera.getPosition().x, walkSequence);
			playerMoved = 1;
			dashed = shiftDash(sf::Keyboard::Down);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
			character->Block(dt, blockSequence);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
			character->Attack(dt, attackSequence);
			if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
			std::cout << "End of GAme!" << std::endl;
		}
		else // No direction keys pressed, Second character stays idle
		{
			character->Idle(dt, idleSequence);
		}
		if (playerMoved == 1)
		{
			camera.focusTranslate(character->getPosition(), distX, distZ);
		} // Translate camera position as character moves


		if (dashed == 1)
		{
			character->setStamina(character->getStamina() - 3);
		}
		else {
			character->setStamina(character->getStamina() + 0.2);
			if (character->getStamina() > 40)
				character->setStaminaState(1);
		}
	}
}

bool Game::CheckPlayerAttack() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && character->getRenderState() == 2) {
		/*character->Attack(i/5, attackSequence);
		if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
		if (i > 15 && dcharacter->getDistance(character) < 0.5 && i < 25)
		{
			dcharacter->setHealth(dcharacter->getHealth() - 0.1);
			if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
		}*/
		return true;
	}
	return false;
}

void Game::updateSecondPlayerInput() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		dcharacter->Walk(sf::Keyboard::Left, dt, camera.getPosition().z, camera.getPosition().x, walkSequence2);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
	{
		dcharacter->Walk(sf::Keyboard::Right, dt, camera.getPosition().z, camera.getPosition().x, walkSequence2);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
	{
		dcharacter->Walk(sf::Keyboard::Up, dt, camera.getPosition().z, camera.getPosition().x, walkSequence2);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
	{
		dcharacter->Walk(sf::Keyboard::Down, dt, camera.getPosition().z, camera.getPosition().x, walkSequence2);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		dcharacter->Attack(dt, attackSequence2);
	}

	else // No TFGH (second player direction keys) pressed, Second character stays idle
	{
		dcharacter->Idle(dt, idleSequence2);
	}
}


void Game::updateAIMove(Character* enemy) {

	int Dist = 1;

	character->setke(0);
	bool PlayerAttack = 0; // To record if player attacks when enemy is acting
	bool PlayerHit = 0;

	if (enemy->getRenderState() != 0) {
		int state = 0;
		glm::vec3 AIPos = enemy->getPosition();
		glm::vec3 HumanPos = character->getPosition();

		if (enemy->getDistance(character) > 10)
			setGameState(0);

		// Change between state 0 and 1 (walking direction) every 100 keytime, this two states both makes the AI walk when player is not around
		if (int(keyTime / 100) % 2 != 0)
			state = 0;
		if (int(keyTime / 100) % 2 == 0)
			state = 1;
		if (character->getRenderState() == 2) {
			// Player is close to AI, change state to chasing the player
			if (enemy->getDistance(character) < 7)
				state = 2;
			// Player is next to AI, change state to still and prepare fighting
			if (enemy->getDistance(character) < (2/*+ enemy->getSize().x * 0.5*/))
				state = 3;
			if ((character->getHealth() < 50 || enemy->getHealth() < 50) && enemy->getStamina() > 1)
			{
				state = 4;
				ResourceManager::getCharacter("Effect1")->setRenderState(2);
				ResourceManager::getCharacter("Effect2")->setRenderState(2);
				ResourceManager::getCharacter("Effect3")->setRenderState(2);
				ResourceManager::getCharacter("Effect4")->setRenderState(2);
			}
			if ((enemy->getStamina() <= 5 || enemy->getHealth() < 30) && (int(keyTime) % 100) > 70)
			{
				state = 5;
				ResourceManager::getCharacter("Effect1")->setRenderState(0);
				ResourceManager::getCharacter("Effect2")->setRenderState(0);
				ResourceManager::getCharacter("Effect3")->setRenderState(0);
				ResourceManager::getCharacter("Effect4")->setRenderState(0);
			}
		}

		if (state >= 3 && getBGM() < 2) {
			peaceMusic->pause();
			setBGM(1);
		}

		if (state == 0) {
			enemy->Walk(sf::Keyboard::Down, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2);
		}
		if (state == 1) {
			enemy->Walk(sf::Keyboard::Up, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2);
		}

		glm::vec3 pos = enemy->getPosition();
		glm::vec3 newPositionZ1(pos.x, pos.y + 0.25,pos.z + 0.02);
		glm::vec3 newPositionZ2(pos.x, pos.y + 0.25, pos.z - 0.02);
		glm::vec3 newPositionZ3(pos.x - 0.05, pos.y + 0.25, pos.z - 0.02);
		glm::vec3 newPositionZ4(pos.x + 0.05, pos.y + 0.25, pos.z - 0.02);

		glm::vec3 newPositionX1(pos.x + 0.02, pos.y + 0.25, pos.z);
		glm::vec3 newPositionX2(pos.x - 0.02, pos.y + 0.25, pos.z);
		bool collisionZ = false;//((PhysicsEngine::checkCollision(newPositionZ1)) || (PhysicsEngine::checkCollision(newPositionZ2)) );
		bool collisionX = false; //(PhysicsEngine::checkCollision(newPositionX1)) || (PhysicsEngine::checkCollision(newPositionX2));
		/*glm::vec3 newPositionX1(pos.x + 0.1, pos.y, pos.z);
		glm::vec3 newPositionX2(pos.x - 0.1, pos.y, pos.z);*/

		if (state == 2) {
			if (/*abs(AIPos.x - HumanPos.x) < abs(AIPos.z - HumanPos.z) && */ (abs(AIPos.z - HumanPos.z) > abs(AIPos.x - HumanPos.x)) && !collisionZ) {
					if (AIPos.z < HumanPos.z) 
					{
						enemy->setPosition(glm::vec3(pos.x, pos.y, pos.z + dt / 2));
						enemy->Walk(sf::Keyboard::Down, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2, 2);
					}
					else 
					{
						enemy->setPosition(glm::vec3(pos.x, pos.y, pos.z - dt / 2));
						enemy->Walk(sf::Keyboard::Up, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2, 2);
					}
				
			}
			else {
				if (collisionZ)
				{
					enemy->setPosition(glm::vec3(pos.x + dt / 2, pos.y, pos.z));
					enemy->Walk(sf::Keyboard::Right, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2, 2);
				}
				else {
					if (AIPos.x < HumanPos.x) {
						enemy->setPosition(glm::vec3(pos.x + dt / 2, pos.y, pos.z));
						enemy->Walk(sf::Keyboard::Right, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2, 2);
					}
					else {
						enemy->setPosition(glm::vec3(pos.x - dt / 2, pos.y, pos.z));
						enemy->Walk(sf::Keyboard::Left, dt / 2, camera.getPosition().z, camera.getPosition().x, enemy->walkSequence2, 2);
					}
				}
			}

		}


		if (state == 3) {

			int hitted = 0;
			int blocked = 0;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && character->getStamina() > 50) {
				character->Block(0, blockSequence);
				blocked = 1;
			}
			
			if (int(keyTime) % 40 == 1) {
				/*dcharacter->Attack(int(keyTime) % 3, attackSequence2);*/
				for (int i = 0; i < 30; i++) {
					if (!PlayerAttack && CheckPlayerAttack())
						PlayerAttack = CheckPlayerAttack();
					if (HumanPos.x > AIPos.x)
						enemy->setRotation(glm::radians(enemy->getDirection() + 0.0));
					else
						enemy->setRotation(glm::radians(enemy->getDirection() + 180));
					enemy->Attack(i / 5, enemy->attackSequence2);
					
					if (PlayerAttack)
					{
						character->Attack(i / 5, attackSequence);
						if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
						if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
						{
							if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
							PlayerHit = 1;
						}
					}
					else
						updateFirstPlayerInput();

					updateDT();
					updateWorldObjects();
					render();
				}


				if (abs(AIPos.x - HumanPos.x) > abs(AIPos.z - HumanPos.z)) {
					if (AIPos.x < HumanPos.x) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Right, i / 5, camera.getPosition().z, camera.getPosition().x, enemy->strikeSequence2, 4);
							if (i > 20 && enemy->getDistance(character) < 1 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									PlayerAttack = 1;
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateDT();
							updateWorldObjects();
							render();
						}
					}

					else if (AIPos.x > HumanPos.x) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Left, i / 5, camera.getPosition().z, camera.getPosition().x, enemy->strikeSequence2, 4);
							if (i > 15 && dcharacter->getDistance(character) < 1 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateDT();
							updateWorldObjects();
							render();
						}
					}
				}
				else {
					if (AIPos.z > HumanPos.z) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Up, i / 5, camera.getPosition().z, camera.getPosition().x, enemy->strikeSequence2, 4);
							if (i > 15 && dcharacter->getDistance(character) < 1 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateDT();
							updateWorldObjects();
							render();
						}
					}

					else if (AIPos.z < HumanPos.z) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Down, i / 5, camera.getPosition().z, camera.getPosition().x, enemy->strikeSequence2, 4);
							if (i > 15 && dcharacter->getDistance(character) < 1 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateDT();
							updateWorldObjects();
							render();
						}
					}
				}

				if (blocked == 1 && hitted == 1) {
					character->setStamina(character->getStamina() - 80);
					if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
				}

				if (blocked == 0 && hitted == 1) {
					character->setHealth(character->getHealth() - 15);
					if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
				}
			}
			else {
				character->setke(0);
			}
		}


		//ResourceManager::getCharacter("Effect1")->Idle(dt, boomSequence2);
		//ResourceManager::getCharacter("Effect2")->Idle(dt, boomSequence2);
		if (state == 4) {
			glm::vec3 pos = enemy->getPosition();
			ResourceManager::getCharacter("Effect1")->setPosition(glm::vec3(pos.x,pos.y - enemy->getSize().x * 0.35, pos.z + 0.5));
			ResourceManager::getCharacter("Effect2")->setPosition(glm::vec3(pos.x, pos.y - enemy->getSize().x * 0.35, pos.z - 0.5));
			ResourceManager::getCharacter("Effect3")->setPosition(glm::vec3(pos.x + 0.5, pos.y - enemy->getSize().x * 0.35, pos.z));
			ResourceManager::getCharacter("Effect4")->setPosition(glm::vec3(pos.x - 0.5, pos.y - enemy->getSize().x * 0.35, pos.z));

			ResourceManager::getCharacter("Effect1")->Idle(dt, boomSequence2);
			ResourceManager::getCharacter("Effect2")->Idle(dt, boomSequence2);
			ResourceManager::getCharacter("Effect3")->Idle(dt, boomSequence2);
			ResourceManager::getCharacter("Effect4")->Idle(dt, boomSequence2);

			if (bossSkill->getStatus() != sf::Sound::Playing) bossSkill->play();

			enemy->Idle(dt/2, enemy->winSequence2);

			enemy->setStamina(enemy->getStamina() - 0.5);
			if (character->getDistance(enemy) <= 1) {
				character->Dizzy(0, dizzySequence);
				if (character->getke() == 0)
					character->setke(1);
				else
					character->setke(0);
				character->setHealth(character->getHealth() - 0.8);
			}
			if (enemy->getStamina() < 5 || (enemy->getHealth() <= 0) )
			{
				ResourceManager::getCharacter("Effect1")->setRenderState(0);
				ResourceManager::getCharacter("Effect2")->setRenderState(0);
				ResourceManager::getCharacter("Effect3")->setRenderState(0);
				ResourceManager::getCharacter("Effect4")->setRenderState(0);
			}
			character->setke(0);
		}

		if (state == 5) {
			if (bossSkill->getStatus() != sf::Sound::Playing) bossSkill->play();
			
			ResourceManager::getCharacter("Pulse")->setRenderState(1);
			ResourceManager::getCharacter("Pulse")->setPosition(enemy->getPosition());

			if (HumanPos.x > AIPos.x) {
				ResourceManager::getCharacter("Pulse")->setRotation(glm::radians(ResourceManager::getCharacter("Pulse")->getDirection() + 0.0));
				enemy->setRotation(glm::radians(ResourceManager::getCharacter("Pulse")->getDirection() + 0.0));
			}
			else {
				ResourceManager::getCharacter("Pulse")->setRotation(glm::radians(ResourceManager::getCharacter("Pulse")->getDirection() + 180));
				enemy->setRotation(glm::radians(ResourceManager::getCharacter("Pulse")->getDirection() + 180));
			}
			if (abs(HumanPos.x - AIPos.x) > abs(HumanPos.z - AIPos.z)) {
				if (HumanPos.x > AIPos.x) {
					for (int i = 0; i < 30; i++) {
						enemy->Attack(i / 5, crouchSequence2);
						ResourceManager::getCharacter("Pulse")->AIWalk(sf::Keyboard::Right, dt, camera.getPosition().z, camera.getPosition().x, pulseSequence2, 3);
						if (ResourceManager::getCharacter("Pulse")->getDistance(character) < 0.5)
						{
							character->setHealth(character->getHealth() - 2);
							character->Dizzy(0, dizzySequence);
							if (character->getke() == 0)
								character->setke(1);
							else
								character->setke(0);

						}
						else {
							if (!PlayerAttack && CheckPlayerAttack())
								PlayerAttack = CheckPlayerAttack();
							if (PlayerAttack)
							{
								character->Attack(i / 5, attackSequence);
								if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
								if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
								{
									if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
									PlayerHit = 1;
								}
							}
							else
								updateFirstPlayerInput();
						}

						if (enemy->getHealth() <= 0)
							break;

						updateDT();
						updateWorldObjects();
						render();
					}
				}
			
				else {
					for (int i = 0; i < 30; i++) {
						enemy->Attack(i / 5, crouchSequence2);
						ResourceManager::getCharacter("Pulse")->AIWalk(sf::Keyboard::Left, dt, camera.getPosition().z, camera.getPosition().x, pulseSequence2, 3);
						if (ResourceManager::getCharacter("Pulse")->getDistance(character) < 0.5)
						{
							character->setHealth(character->getHealth() - 2);
							character->Dizzy(0, dizzySequence);
							if (character->getke() == 0)
								character->setke(1);
							else
								character->setke(0);

						}
						else {
							if (!PlayerAttack && CheckPlayerAttack())
								PlayerAttack = CheckPlayerAttack();
							if (PlayerAttack)
							{
								character->Attack(i / 5, attackSequence);
								if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
								if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
								{
									if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
									PlayerHit = 1;
								}
							}
							else
								updateFirstPlayerInput();
						}

						if (enemy->getHealth() <= 0)
							break;

						updateDT();
						updateWorldObjects();
						render();
					}
				}
			}

			else {
				if (HumanPos.z > AIPos.z) {
					for (int i = 0; i < 30; i++) {
						enemy->Attack(i / 5, crouchSequence2);
						ResourceManager::getCharacter("Pulse")->AIWalk(sf::Keyboard::Down, dt, camera.getPosition().z, camera.getPosition().x, pulseSequence2, 3);
						if (ResourceManager::getCharacter("Pulse")->getDistance(character) < 0.5)
						{
							character->setHealth(character->getHealth() - 2);
							character->Dizzy(0, dizzySequence);
							if (character->getke() == 0)
								character->setke(1);
							else
								character->setke(0);

						}
						else {
							if (!PlayerAttack && CheckPlayerAttack())
								PlayerAttack = CheckPlayerAttack();
							if (PlayerAttack)
							{
								character->Attack(i / 5, attackSequence);
								if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
								if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
								{
									if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
									PlayerHit = 1;
								}
							}
							else
								updateFirstPlayerInput();
						}

						if (enemy->getHealth() <= 0)
							break;

						updateDT();
						updateWorldObjects();
						render();
					}
				}
				else {
					for (int i = 0; i < 30; i++) {
						enemy->Attack(i / 5, crouchSequence2);
						ResourceManager::getCharacter("Pulse")->AIWalk(sf::Keyboard::Up, dt, camera.getPosition().z, camera.getPosition().x, pulseSequence2, 3);
						if (ResourceManager::getCharacter("Pulse")->getDistance(character) < 0.5)
						{
							character->setHealth(character->getHealth() - 2);
							character->Dizzy(0, dizzySequence);
							if (character->getke() == 0)
								character->setke(1);
							else
								character->setke(0);

						}
						else {
							if (!PlayerAttack && CheckPlayerAttack())
								PlayerAttack = CheckPlayerAttack();
							if (PlayerAttack)
							{
								character->Attack(i / 5, attackSequence);
								if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
								if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
								{
									if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
									PlayerHit = 1;
								}
							}
							else
								updateFirstPlayerInput();
						}

						if (enemy->getHealth() <= 0)
							break;

						updateDT();
						updateWorldObjects();
						render();
					}
				}
			}

			// State 5 method:
			/*for (int i = 0; i < 30; i++) {
				enemy->Attack(i / 5, crouchSequence2);
				ResourceManager::getCharacter("Pulse")->AIWalk(sf::Keyboard::Up, dt, camera.getPosition().z, camera.getPosition().x, pulseSequence2, 3);
				if (ResourceManager::getCharacter("Pulse")->getDistance(character) < 0.5)
				{
					character->setHealth(character->getHealth() - 0.2);
					character->Dizzy(0, dizzySequence);
					if (character->getke() == 0)
						character->setke(1);
					else
						character->setke(0);

				}
				else {
					if (!PlayerAttack && CheckPlayerAttack())
						PlayerAttack = CheckPlayerAttack();
					if (PlayerAttack)
					{
						character->Attack(i / 5, attackSequence);
						if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
						if (i > 15 && enemy->getDistance(character) < 1 && i < 25)
						{
							if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
							PlayerHit = 1;
						}
					}
					else
						updateFirstPlayerInput();
				}

				if (enemy->getHealth() <= 0)
					break;

				updateDT();
				updateWorldObjects();
				render();
			}*/

			ResourceManager::getCharacter("Pulse")->setRenderState(0);
		}



		if (PlayerAttack && PlayerHit)
			enemy->setHealth(enemy->getHealth() - 10);
	}
}

void Game::updateAIMove2(Character* enemy, std::vector<std::string> walk, std::vector<std::string> attack) {

	int Dist = 1;

	character->setke(0);
	bool PlayerAttack = 0; // To record if player attacks when enemy is acting
	bool PlayerHit = 0;

	if (enemy->getRenderState() != 0) {
		int state = 0;
		glm::vec3 AIPos = enemy->getPosition();
		glm::vec3 HumanPos = character->getPosition();

		

		// Change between state 0 and 1 (walking direction) every 100 keytime, this two states both makes the AI walk when player is not around
		if (int(keyTime / 100) % 2 != 0)
			state = 0;
		if (int(keyTime / 100) % 2 == 0)
			state = 1;
		if (character->getRenderState() == 2) {
			// Player is close to AI, change state to chasing the player
			if (enemy->getDistance(character) < 4)
				state = 2;
			// Player is next to AI, change state to still and prepare fighting
			if (enemy->getDistance(character) < (2/*+ enemy->getSize().x * 0.5*/))
				state = 3;
		}

		if (state == 0) {
			enemy->Walk(sf::Keyboard::Down, dt / 2, camera.getPosition().z, camera.getPosition().x, walk);
			this->ninjaState = 0;
		}
		if (state == 1) {
			enemy->Walk(sf::Keyboard::Up, dt / 2, camera.getPosition().z, camera.getPosition().x, walk);
			this->ninjaState = 0;
		}

		glm::vec3 pos = enemy->getPosition();
		glm::vec3 newPositionZ1(pos.x, pos.y + 0.25, pos.z + 0.02);
		glm::vec3 newPositionZ2(pos.x, pos.y + 0.25, pos.z - 0.02);
		glm::vec3 newPositionZ3(pos.x - 0.05, pos.y + 0.25, pos.z - 0.02);
		glm::vec3 newPositionZ4(pos.x + 0.05, pos.y + 0.25, pos.z - 0.02);

		glm::vec3 newPositionX1(pos.x + 0.02, pos.y + 0.25, pos.z);
		glm::vec3 newPositionX2(pos.x - 0.02, pos.y + 0.25, pos.z);
		bool collisionZ = false;//((PhysicsEngine::checkCollision(newPositionZ1)) || (PhysicsEngine::checkCollision(newPositionZ2)) );
		bool collisionX = false; //(PhysicsEngine::checkCollision(newPositionX1)) || (PhysicsEngine::checkCollision(newPositionX2));
		/*glm::vec3 newPositionX1(pos.x + 0.1, pos.y, pos.z);
		glm::vec3 newPositionX2(pos.x - 0.1, pos.y, pos.z);*/

		if (state>= 2)
			this->ninjaState = 1;

		if (state == 2) {
			
			if (/*abs(AIPos.x - HumanPos.x) < abs(AIPos.z - HumanPos.z) && */ (abs(AIPos.z - HumanPos.z) > abs(AIPos.x - HumanPos.x)) && !collisionZ) {
				if (AIPos.z < HumanPos.z)
				{
					enemy->setPosition(glm::vec3(pos.x, pos.y, pos.z + dt / 2));
					enemy->Walk(sf::Keyboard::Down, dt / 2, camera.getPosition().z, camera.getPosition().x, walk, 2);
				}
				else
				{
					enemy->setPosition(glm::vec3(pos.x, pos.y, pos.z - dt / 2));
					enemy->Walk(sf::Keyboard::Up, dt / 2, camera.getPosition().z, camera.getPosition().x, walk, 2);
				}

			}
			else {
				if (collisionZ)
				{
					enemy->setPosition(glm::vec3(pos.x + dt / 2, pos.y, pos.z));
					enemy->Walk(sf::Keyboard::Right, dt / 2, camera.getPosition().z, camera.getPosition().x, walk, 2);
				}
				else {
					if (AIPos.x < HumanPos.x) {
						enemy->setPosition(glm::vec3(pos.x + dt / 2, pos.y, pos.z));
						enemy->Walk(sf::Keyboard::Right, dt / 2, camera.getPosition().z, camera.getPosition().x, walk, 2);
					}
					else {
						enemy->setPosition(glm::vec3(pos.x - dt / 2, pos.y, pos.z));
						enemy->Walk(sf::Keyboard::Left, dt / 2, camera.getPosition().z, camera.getPosition().x, walk, 2);
					}
				}
			}

		}


		if (state == 3) {

			int hitted = 0;
			int blocked = 0;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && character->getStamina() > 50) {
				character->Block(0, blockSequence);
				blocked = 1;
			}

			if (int(keyTime) % 40 == 1) {
				/*dcharacter->Attack(int(keyTime) % 3, attackSequence2);*/
				for (int i = 0; i < 30; i++) {
					if (!PlayerAttack && CheckPlayerAttack())
						PlayerAttack = CheckPlayerAttack();
					if (HumanPos.x > AIPos.x)
						enemy->setRotation(glm::radians(enemy->getDirection() + 0.0));
					else
						enemy->setRotation(glm::radians(enemy->getDirection() + 180));
					enemy->Attack(i / 5, walk);

					if (PlayerAttack)
					{
						character->Attack(i / 5, attackSequence);
						if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
						if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
						{
							if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
							PlayerHit = 1;
						}
					}
					else
						updateFirstPlayerInput();
					updateAIMove(dcharacter);
					updateDT();
					updateWorldObjects();
					render();
				}


				if (abs(AIPos.x - HumanPos.x) > abs(AIPos.z - HumanPos.z)) {
					if (AIPos.x < HumanPos.x) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Right, i / 5, camera.getPosition().z, camera.getPosition().x, attack, 4);
							if (i > 0 && enemy->getDistance(character) < 2 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									PlayerAttack = 1;
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateAIMove(dcharacter);
							updateDT();
							updateWorldObjects();
							render();
						}
					}

					else if (AIPos.x > HumanPos.x) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Left, i / 5, camera.getPosition().z, camera.getPosition().x, attack, 4);
							if (i > 0 && enemy->getDistance(character) < 2 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateAIMove(dcharacter);
							updateDT();
							updateWorldObjects();
							render();
						}
					}
				}
				else {
					if (AIPos.z > HumanPos.z) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Up, i / 5, camera.getPosition().z, camera.getPosition().x, attack, 4);
							if (i > 0 && enemy->getDistance(character) < 1 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateAIMove(dcharacter);
							updateDT();
							updateWorldObjects();
							render();
						}
					}

					else if (AIPos.z < HumanPos.z) {
						for (int i = 0; i < 30; i++) {
							enemy->AIWalk(sf::Keyboard::Down, i / 5, camera.getPosition().z, camera.getPosition().x, attack, 4);
							if (i > 0 && enemy->getDistance(character) < 1 && i < 25)
							{
								character->Dizzy(0, dizzySequence);
								if (character->getke() == 0)
									character->setke(1);
								else
									character->setke(0);
								hitted = 1;
							}
							else {
								if (!PlayerAttack && CheckPlayerAttack())
									PlayerAttack = CheckPlayerAttack();
								if (PlayerAttack)
								{
									character->Attack(i / 5, attackSequence);
									if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
									if (i > 15 && enemy->getDistance(character) < Dist && i < 25)
									{
										if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
										PlayerHit = 1;
									}
								}
								else
									updateFirstPlayerInput();
							}
							updateAIMove(dcharacter);
							updateDT();
							updateWorldObjects();
							render();
						}
					}
				}

				if (blocked == 1 && hitted == 1) {
					character->setStamina(character->getStamina() - 80);
					if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
				}

				if (blocked == 0 && hitted == 1) {
					character->setHealth(character->getHealth() - 5);
					if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
				}
			}
			else {
			enemy->Idle(dt, dizzySequenceN);
				character->setke(0);
			}
		}

		if (PlayerAttack && PlayerHit)
			enemy->setHealth(enemy->getHealth() - 10);
	}
}

void Game::updateWorldObjects() {
	int mode = 0;
	updateKeyTime();
	updateEvents();
	updateKeyboardInput();
	std::vector<Sprite*> LightPoints;
	LightPoints.push_back(ResourceManager::getSprite("light1"));
	LightPoints.push_back(ResourceManager::getSprite("light2"));
	LightPoints.push_back(ResourceManager::getSprite("light3"));
	LightPoints.push_back(ResourceManager::getSprite("light4"));
	LightPoints.push_back(ResourceManager::getSprite("light5"));
	LightPoints.push_back(ResourceManager::getSprite("light6"));

	double velocity = 0.15;
	std::vector<double> distances;
	distances.push_back(2);
	distances.push_back(5);

	double distance = 2;

	std::vector<int> directions;
	directions.push_back(1);
	directions.push_back(-1);
	double YAW = camera.getYaw();
	for (int i = 0; i < LightPoints.size(); i++)
	{
		double posX, posY, posZ;
		/*double posX = camera.getPosition().x - double(((i+1)%3)/2);
		double posY = camera.getPosition().y + double((i%3)/2);
		double posZ = camera.getPosition().z - 2;*/

		if (YAW < -90) {
			posX = camera.getPosition().x - (double(((i + 1) % 3) / 5) + distance) * cos((180 + YAW) * PI / 180);
			posY = camera.getPosition().y + double((i % 3) / 2) + i/2;
			posZ = camera.getPosition().z - distance * sin((180 + YAW) * PI / 180);
		}
		else if (YAW < 0) {
			// -90 < YAW < 0
			posX = camera.getPosition().x - (double(((i + 1) % 3) / 5) - distance) * cos((-YAW) * PI / 180);
			posY = camera.getPosition().y + double((i % 3) / 2) + i/2;
			posZ = camera.getPosition().z - distance * sin((-YAW) * PI / 180);
		}
		else if (YAW > 90) {
			posX = camera.getPosition().x - (double(((i + 1) % 3) / 5) + distance) * cos((180 - YAW) * PI / 180);
			posY = camera.getPosition().y + double((i % 3) / 2) + i/2;
			posZ = camera.getPosition().z + distance * sin((180 - YAW) * PI / 180);
		}
		else {
			// 90 > YAW > 0
			posX = camera.getPosition().x + (double(((i + 1) % 3) / 5) + distance) * cos((YAW)*PI / 180);
			posY = camera.getPosition().y + double((i % 3) / 2) + i/2;
			posZ = camera.getPosition().z + distance * sin((YAW)*PI / 180);
		}

		if ((int(keyTime) % 300) > 270 && LightPoints[i]->getSize().x < 0.03 * i) {
			LightPoints[i]->setRenderState(0);
		}
		else {
			LightPoints[i]->setPosition(glm::vec3(posX + velocity * cos(keyTime / 50 - 40 * i) + i, posY - velocity * cos(keyTime / 50 + 30 * i), posZ - velocity * sin(keyTime / 50 - 20 * i)));
			LightPoints[i]->setSize(glm::vec2(0.05 * abs(sin(keyTime / 50)), 0.05 * abs(sin(keyTime / 50))));
			LightPoints[i]->setRenderState(1);
		}

	}
}

void Game::Animation(float keytime) {
	int Dist = 1;

	updateAIMove(dcharacter);
	updateAIMove2(ncharacter,walkSequenceN,attackSequenceN);
	updateWorldObjects();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && character->getRenderState() == 2)
	{
		int hitted = 0;
		int hitted2 = 0;

		for (int i = 0; i < 48; i++) {
			character->Attack(i / 8, attackSequence);
			if (swordSound->getStatus() != sf::Sound::Playing) swordSound->play();
			
			if (i > 20 && dcharacter->getDistance(character) < Dist && i < 45)
			{
				dcharacter->Dizzy(0, dizzySequence2);
				hitted = 1;
			}
			
			else if (i > 0 && ncharacter->getDistance(character) < Dist && i < 45)
			{
				/*ncharacter->Dizzy(0, dizzySequenceN);*/
				hitted2 = 1;
			}

			else {
				updateSecondPlayerInput();
				updateAIMove(dcharacter);
				updateAIMove2(ncharacter, walkSequenceN, attackSequenceN);
			}
			updateDT();
			updateWorldObjects();
			render();
		}

		if (hitted == 1) {
			dcharacter->setHealth(dcharacter->getHealth() - 10);
			if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
		}

		if (hitted2 == 1) {
			ncharacter->setHealth(ncharacter->getHealth() - 10);
			if (hitSound->getStatus() != sf::Sound::Playing) hitSound->play();
		}

		
	}

	/*if (character->getRenderState() != 0 && this->getHealthUI() <= 0)
	{
		ResourceManager::getCharacter("Effect1")->setRenderState(0);
		ResourceManager::getCharacter("Effect2")->setRenderState(0);
		ResourceManager::getCharacter("Effect3")->setRenderState(0);
		ResourceManager::getCharacter("Effect4")->setRenderState(0);

		ResourceManager::getCharacter("Pulse")->setRenderState(0);

		if (youDie->getStatus() != sf::Sound::Playing) youDie->play();
		for (int i = 0; i < 80; i++) {
			character->Attack(i / 10, dieSequence);
			dcharacter->Idle(i / 5, winSequence2);
			
			updateDT();
			render();
		}
		state = 0;
		character->setRenderState(0);
		
	}

	if (dcharacter->getRenderState() != 0 && dcharacter->getHealth() <= 0) {
		ResourceManager::getCharacter("Effect1")->setRenderState(0);
		ResourceManager::getCharacter("Effect2")->setRenderState(0);
		ResourceManager::getCharacter("Effect3")->setRenderState(0);
		ResourceManager::getCharacter("Effect4")->setRenderState(0);

		ResourceManager::getCharacter("Pulse")->setRenderState(0);
		if (bossDie->getStatus() != sf::Sound::Playing) bossDie->play();
		for (int i = 0; i < 121; i++) {
			dcharacter->Attack(i / 12, dieSequence2);
			character->Idle(i / 12, winSequence);
			updateDT();
			render();
		}
		state = 0;
		dcharacter->setRenderState(0);
	}*/


}

void Game::update()
{
	this->updateKeyTime();
	//Delta Time
	this->updateDT();
	//Events
	this->updateEvents();

	// First Player Movement
	this->updateFirstPlayerInput();

	// Second Player Movement
	this->updateSecondPlayerInput();

	//Input
	this->updateKeyboardInput();

	this->setUI();

	//Animation
	this->Animation(keyTime);

	if (getBGM() == 0 && dcharacter->getRenderState() == 0)
		state = 2;
	else if (getBGM() == 1)
	{
		std::cout << "boss music!" << getBGM() << std::endl;
		bossMusic->play();
		setBGM(2);
		std::cout << getBGM() << std::endl;
		state = 1;
	}
	else if (getBGM() >= 2 && state == 0) {
		bossMusic->pause();
		peaceMusic->play();
		setBGM(0);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
		character->setRenderState(2);
		dcharacter->setRenderState(2);
		state = 0;
		character->setHealth(100);
		character->setStamina(100);
		dcharacter->setHealth(100);
		dcharacter->setStamina(100);
		
	}

	if (character->getRenderState() != 0 && this->getHealthUI() <= 0)
	{
		ResourceManager::getCharacter("Effect1")->setRenderState(0);
		ResourceManager::getCharacter("Effect2")->setRenderState(0);
		ResourceManager::getCharacter("Effect3")->setRenderState(0);
		ResourceManager::getCharacter("Effect4")->setRenderState(0);

		ResourceManager::getCharacter("Pulse")->setRenderState(0);

		if (youDie->getStatus() != sf::Sound::Playing) youDie->play();
		for (int i = 0; i < 80; i++) {
			character->Attack(i / 10, dieSequence);
			dcharacter->Idle(i / 5, winSequence2);

			updateDT();
			render();
		}
		state = 0;
		character->setRenderState(0);

	}

	if (dcharacter->getRenderState() != 0 && dcharacter->getHealth() <= 0) {
		ResourceManager::getCharacter("Effect1")->setRenderState(0);
		ResourceManager::getCharacter("Effect2")->setRenderState(0);
		ResourceManager::getCharacter("Effect3")->setRenderState(0);
		ResourceManager::getCharacter("Effect4")->setRenderState(0);

		ResourceManager::getCharacter("Pulse")->setRenderState(0);
		if (bossDie->getStatus() != sf::Sound::Playing) bossDie->play();
		for (int i = 0; i < 121; i++) {
			dcharacter->Attack(i / 12, dieSequence2);
			character->Idle(i / 12, winSequence);
			updateDT();
			render();
		}
		state = 0;
		dcharacter->setRenderState(0);
	}


	if (ncharacter->getRenderState() != 0 && ncharacter->getHealth() <= 0)
	{
		if (bossDie->getStatus() != sf::Sound::Playing) bossDie->play();
		
		for (int i = 0; i < 80; i++) {
			ncharacter->Attack(i / 20, dizzySequenceN);
			character->Idle(i / 5, winSequence);

			updateDT();
			render();
		}
		ncharacter->setRenderState(0);

	}
}



// render
void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	renderMaster->render(camera);

	this->window->display();
}


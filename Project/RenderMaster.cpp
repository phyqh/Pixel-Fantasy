#include "RenderMaster.h"
#include "Model.h"
#include "Character.h"
#include "Game.h"


void RenderMaster::initialize()
{
	// load glad
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initFramebuffer();
	initShader();
	initResources();
	initInstances();
}

void RenderMaster::initFramebuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &colorbuffer);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, game->getWindowWidth(), game->getWindowHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, game->getWindowWidth(),
		game->getWindowHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderMaster::initShader()
{
	shaderName = "screen";
	ResourceManager::loadShader("../Shaders/screen.vs", "../Shaders/screen.fs", nullptr, shaderName);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void RenderMaster::initResources()
{
	ResourceManager::loadTexture("../Resources/Textures/head.png", "head");
	auto landscape = ResourceManager::loadModel("../Resources/Models/landscape/landscape.obj", "landscape");
	landscape->setPixelization(4096);
	auto rest = ResourceManager::loadModel("../Resources/Models/landscape/rest.obj", "rest");
	auto water = ResourceManager::loadModel("../Resources/Models/landscape/water.obj", "water");
	water->setPixelization(4096);
	auto windmill = ResourceManager::loadModel("../Resources/Models/lowpoly-township-set/windmill.obj", "windmill");

	ResourceManager::loadMusic("../Resources/BGM/bg.wav", "Peace");
	ResourceManager::loadMusic("../Resources/BGM/dragon-armour.wav", "fatalBattle");
	ResourceManager::loadMusic("../Resources/BGM/die1.wav", "youDie");
	ResourceManager::loadMusic("../Resources/BGM/enemyFell.wav", "bossDie");

	ResourceManager::loadSound("../Resources/SoundEffects/hitted.wav", "hitted");
	ResourceManager::loadSound("../Resources/SoundEffects/sword.wav", "swordSound");
	ResourceManager::loadSound("../Resources/SoundEffects/BossSkill.wav", "bossSkill");

	ResourceManager::loadTexture("../Resources/Icons/heart.png", "heart");
	ResourceManager::loadTexture("../Resources/Icons/sp.png", "sp");
	ResourceManager::loadTexture("../Resources/Icons/you die.png", "you die");
	ResourceManager::loadTexture("../Resources/Icons/enemy.png","killer");
	ResourceManager::loadTexture("../Resources/Icons/enemyDie.png", "boss die");
	/*ResourceManager::loadTexture("");*/


	// load light ball
	ResourceManager::loadTexture("../Resources/Textures/light.png", "light");

	// load ninja idle (crouch)
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_crouch 1.png", "Nidle1");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_crouch 2.png", "Nidle2");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_crouch 3.png", "Nidle3");

	// loda ninja crouch dash
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_crouchdash 1.png", "Ndash1");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_crouchdash 2.png", "Ndash2");

	// load ninja walk
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 01.png", "Nrun1");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 02.png", "Nrun2");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 03.png", "Nrun3");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 04.png", "Nrun4");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 05.png", "Nrun5");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 06.png", "Nrun6");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 07.png", "Nrun7");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 08.png", "Nrun8");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 09.png", "Nrun9");
	ResourceManager::loadTexture("../Resources/Textures/Animation3/SNinja_nosword_run 10.png", "Nrun10");

	// load dragon walk sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/walk_01.png", "Dwalk1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/walk_02.png", "Dwalk2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/walk_03.png", "Dwalk3");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/walk_04.png", "Dwalk4");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/walk_05.png", "Dwalk5");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/walk_06.png", "Dwalk6");

	// load dragon idle sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/idle_01.png", "Didle1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/idle_02.png", "Didle2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/idle_03.png", "Didle3");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/idle_04.png", "Didle4");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/idle_05.png", "Didle5");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/idle_06.png", "Didle6");

	// load dragon attack sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/attack_01.png", "Dattack1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/attack_02.png", "Dattack2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/attack_03.png", "Dattack3");

	// load dragon dizzy sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/dizzy_01.png", "Ddizzy1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/dizzy_02.png", "Ddizzy2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/dizzy_03.png", "Ddizzy3");

	// load dragon strike sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/strike_01.png", "Dstrike1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/strike_02.png", "Dstrike2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/strike_03.png", "Dstrike3");

	// load dragon win sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/win_01.png", "Dwin1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/win_02.png", "Dwin2");

	// load dragon die sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_001.png", "Ddie1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_002.png", "Ddie2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_003.png", "Ddie3");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_004.png", "Ddie4");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_005.png", "Ddie5");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_006.png", "Ddie6");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_007.png", "Ddie7");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_008.png", "Ddie8");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_009.png", "Ddie9");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/die_010.png", "Ddie10");

	// load dragon crouch attack textures
	ResourceManager::loadTexture("../Resources/Textures/Animation2/crouch_ATK_01.png", "Dcrouch1");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/crouch_ATK_02.png", "Dcrouch2");
	ResourceManager::loadTexture("../Resources/Textures/Animation2/crouch_ATK_03.png", "Dcrouch3");

	// load SF textures
	ResourceManager::loadTexture("../Resources/Textures/VFX/Boom_01.png", "Boom1");
	ResourceManager::loadTexture("../Resources/Textures/VFX/Boom_02.png", "Boom2");
	ResourceManager::loadTexture("../Resources/Textures/VFX/Boom_03.png", "Boom3");
	ResourceManager::loadTexture("../Resources/Textures/VFX/Boom_04.png", "Boom4");
	ResourceManager::loadTexture("../Resources/Textures/VFX/Boom_05.png", "Boom5");

	ResourceManager::loadTexture("../Resources/Textures/VFX/pulse1.png", "Pulse1");
	ResourceManager::loadTexture("../Resources/Textures/VFX/pulse2.png", "Pulse2");
	ResourceManager::loadTexture("../Resources/Textures/VFX/pulse3.png", "Pulse3");
	ResourceManager::loadTexture("../Resources/Textures/VFX/pulse4.png", "Pulse4");

	// load monsters
	ResourceManager::loadTexture("../Resources/Textures/Monsters/cerberus.png", "cerberus");
	ResourceManager::loadTexture("../Resources/Textures/Monsters/worm.png", "worm");
	ResourceManager::loadTexture("../Resources/Textures/Monsters/wolf.png", "wolf");

	// load plants
	ResourceManager::loadTexture("../Resources/Textures/Plants/flower1.png", "flower1");
	ResourceManager::loadTexture("../Resources/Textures/Plants/flower2.png", "flower2");
	ResourceManager::loadTexture("../Resources/Textures/Plants/flower4.png", "flower3");
	ResourceManager::loadTexture("../Resources/Textures/Plants/flower5.png", "flower4");
	ResourceManager::loadTexture("../Resources/Textures/Plants/shrub3.png", "shrub");
	ResourceManager::loadTexture("../Resources/Textures/Plants/tree2_dark_autumn.png", "tree1");
	ResourceManager::loadTexture("../Resources/Textures/Plants/tree6_dark_autumn.png", "tree2");
	ResourceManager::loadTexture("../Resources/Textures/Plants/tree8_dark_autumn.png", "tree3");
	ResourceManager::loadTexture("../Resources/Textures/Plants/tree9_dark_autumn.png", "tree4");


	// load walk sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_walk_01.png", "walk1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_walk_02.png", "walk2");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_walk_03.png", "walk3");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_walk_04.png", "walk4");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_walk_05.png", "walk5");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_walk_06.png", "walk6");

	// load idle sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_idle_01.png", "idle1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_idle_02.png", "idle2");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_idle_03.png", "idle3");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_idle_04.png", "idle4");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_idle_05.png", "idle5");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_idle_06.png", "idle6");

	// load attack sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_attack_01.png", "attack1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_attack_02.png", "attack2");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_attack_03.png", "attack3");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_attack_04.png", "attack4");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_attack_05.png", "attack5");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_attack_06.png", "attack6");

	// load block sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_block_01.png", "block1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_block_02.png", "block2");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_block_03.png", "block3");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_block_04.png", "block4");

	// load hurt sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_hurt_01.png", "hurt1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_hurt_02.png", "hurt2");

	// load dash sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_dash_01.png", "dash1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_dash_02.png", "dash2");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_dash_03.png", "dash3");

	// load dash sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_win_01.png", "win1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_win_02.png", "win2");

	// load die sequence textures
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_01.png", "die1");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_02.png", "die2");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_03.png", "die3");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_04.png", "die4");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_05.png", "die5");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_06.png", "die6");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_07.png", "die7");
	ResourceManager::loadTexture("../Resources/Textures/Animation/Knight_die_08.png", "die8");


	// load character
	ResourceManager::createCharacter(glm::vec3(-4.0f, -10.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::radians(0.0), "idle1", "character1");
	ResourceManager::createCharacter(glm::vec3(-5.0f, 0.48f, 50.0f), glm::vec2(3.0f, 3.0f), glm::radians(0.0), "Didle1", "character2");
	ResourceManager::createCharacter(glm::vec3(0.0f, -10.0f, -20.0f), glm::vec2(1.0f, 1.0f), glm::radians(0.0), "Nidle1", "character3");

	Sprite* light1 = ResourceManager::createSprite(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(0.1f, 0.1f), glm::radians(0.0), "light", "light1");
	Sprite* light2 = ResourceManager::createSprite(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(0.1f, 0.1f), glm::radians(0.0), "light", "light2");
	Sprite* light3 = ResourceManager::createSprite(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(0.1f, 0.1f), glm::radians(0.0), "light", "light3");
	Sprite* light4 = ResourceManager::createSprite(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(0.1f, 0.1f), glm::radians(0.0), "light", "light4");
	Sprite* light5 = ResourceManager::createSprite(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(0.1f, 0.1f), glm::radians(0.0), "light", "light5");
	Sprite* light6= ResourceManager::createSprite(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(0.1f, 0.1f), glm::radians(0.0), "light", "light6");

	Character* Boom1 = ResourceManager::createCharacter(glm::vec3(-5.0f, 0.48f, -3.0f), glm::vec2(1,1), glm::radians(0.0), "Boom1", "Effect1");
	Character* Boom2 = ResourceManager::createCharacter(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(1, 1), glm::radians(0.0), "Boom2", "Effect2");
	Character* Boom3 = ResourceManager::createCharacter(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(1, 1), glm::radians(0.0), "Boom3", "Effect3");
	Character* Boom4 = ResourceManager::createCharacter(glm::vec3(-4.0f, 2.48f, 0.0f), glm::vec2(1, 1), glm::radians(0.0), "Boom4", "Effect4");

	Character* Pulse = ResourceManager::createCharacter(glm::vec3(-5.0f, 0.48f, -3.0f), glm::vec2(1, 1), glm::radians(0.0), "Pulse1", "Pulse");

	Boom1->setIdleCount(0);
	Boom2->setIdleCount(1);
	Boom3->setIdleCount(2);
	Boom4->setIdleCount(3);

	Boom1->setRenderState(0);
	Boom2->setRenderState(0);
	Boom3->setRenderState(0);
	Boom4->setRenderState(0);
	
	Pulse->setRenderState(0);

	light1->setRenderState(1);
	light1->setke(1.0);
	light2->setRenderState(1);
	light2->setke(1.0);
	light3->setRenderState(1);
	light3->setke(1.0);
	light4->setRenderState(1);
	light4->setke(1.0);
	light5->setRenderState(1);
	light5->setke(1.0);
	light6->setRenderState(1);
	light6->setke(1.0);
}

void RenderMaster::initInstances()
{
	const unsigned int num = 100 * 60;
	glm::vec3* translations1 = new glm::vec3[num];
	glm::vec3* translations2 = new glm::vec3[num];
	glm::vec3* translations3 = new glm::vec3[num];
	glm::vec3* translations4 = new glm::vec3[num];
	//glm::vec3 translations4[num];
	unsigned int fake = 0;
	for (int i = 0; i < 100; i += 2) {
		for (int j = 0; j < 60; j += 2) {
			float x1 = glm::clamp(j - 30 + 3 * (rand() / double(RAND_MAX)), -30.0, 30.0);
			float z1 = glm::clamp(2 * i - 100 + 3 * (rand() / double(RAND_MAX)), -100.0, 100.0);;
			translations1[60 * i + j] = glm::vec3(x1, 2.0, z1);
			translations1[60 * i + j].y = PhysicsEngine::computeGroundHeight(translations1[60 * i + j], fake) + 0.06;
			float x2 = glm::clamp(j - 30 + 3 * (rand() / double(RAND_MAX)), -30.0, 30.0);
			float z2 = glm::clamp(2 * i - 100 + 3 * (rand() / double(RAND_MAX)), -100.0, 100.0);;
			translations2[60 * i + j] = glm::vec3(x2, 2.0, z2);
			translations2[60 * i + j].y = PhysicsEngine::computeGroundHeight(translations2[60 * i + j], fake) + 0.06;
			float x3 = glm::clamp(j - 30 + 3 * (rand() / double(RAND_MAX)), -30.0, 30.0);
			float z3 = glm::clamp(2 * i - 100 + 3 * (rand() / double(RAND_MAX)), -100.0, 100.0);;
			translations3[60 * i + j] = glm::vec3(x3, 2.0, z3);
			translations3[60 * i + j].y = PhysicsEngine::computeGroundHeight(translations3[60 * i + j], fake) + 0.06;
			float x4 = glm::clamp(j - 30 + 3 * (rand() / double(RAND_MAX)), -30.0, 30.0);
			float z4 = glm::clamp(2 * i - 100 + 3 * (rand() / double(RAND_MAX)), -100.0, 100.0);;
			translations4[60 * i + j] = glm::vec3(x4, 2.0, z4);
			translations4[60 * i + j].y = PhysicsEngine::computeGroundHeight(translations4[60 * i + j], fake) + 0.06;
		}
	}
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * num, &translations1[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ResourceManager::createInstances("flower1", instanceVBO, num, "flower1s");
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * num, &translations2[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ResourceManager::createInstances("flower2", instanceVBO, num, "flower2s");
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * num, &translations3[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ResourceManager::createInstances("flower3", instanceVBO, num, "flower3s");
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * num, &translations4[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ResourceManager::createInstances("flower4", instanceVBO, num, "flower4s");

	delete[] translations1;
	delete[] translations2;
	delete[] translations3;
	delete[] translations4;
}

RenderMaster::RenderMaster(Game* game)
{
	this->game = game;
	this->initialize();
	shadowRenderer = new ShadowRenderer();
	skyboxRenderer = new SkyboxRenderer();
	modelRenderer = new ModelRenderer(shadowRenderer);
	spriteRenderer = new SpriteRenderer();
	debugRenderer = new DebugRenderer(shadowRenderer);
	instanceRenderer = new InstanceRenderer();
	UiRenderer = new uiRenderer();
}

RenderMaster::~RenderMaster() {
	if (skyboxRenderer) delete skyboxRenderer;
	if (modelRenderer) delete modelRenderer;
	if (spriteRenderer) delete spriteRenderer;
	if (shadowRenderer) delete shadowRenderer;
	if (debugRenderer) delete debugRenderer;
	if (instanceRenderer) delete instanceRenderer;
	if (UiRenderer) delete UiRenderer;
	skyboxRenderer = nullptr;
	modelRenderer = nullptr;
	spriteRenderer = nullptr;
	shadowRenderer = nullptr;
	debugRenderer = nullptr;
	instanceRenderer = nullptr;
	UiRenderer = nullptr;
}

void RenderMaster::render(const Camera& camera) const
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// render
	shadowRenderer->render();
	// reset viewport
	glViewport(0, 0, game->getWindowWidth(), game->getWindowHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	skyboxRenderer->render(camera);
	modelRenderer->render(camera);
	spriteRenderer->render(camera);
	instanceRenderer->render(camera);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderFramebuffer();
	UiRenderer->render(game->getHealthUI(),game->getStaminaUI(), game->getBossHealthUI(), game->getState(), game->getState(), game->getNinjaState(),game->getNinjaHealthUI());
	//debugRenderer->render();
}

void RenderMaster::renderFramebuffer() const
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	Shader& screenShader = ResourceManager::getShader(shaderName);
	screenShader.use();
	screenShader.setInteger("colorTexture", 0);
	screenShader.setInteger("depthTexture", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

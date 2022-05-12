#pragma once

#include "Texture.h"
#include "Shader.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.

class Model;
class Sprite;
class Character;
class SpriteInstances;

class ResourceManager
{
private:
	// variables
	
	// static variables
	// Resource storage
	static std::map<std::string, Shader>  shaders;
	static std::map<std::string, Texture> textures;
	static std::map<std::string, Model*> models;
	static std::map<std::string, Sprite*> sprites;
	static std::map<std::string, Character*> characters;
	static std::map<std::string, SpriteInstances*> allInstances;
	static std::map<std::string, sf::Sound*> sounds;
	static std::map<std::string, sf::SoundBuffer*> soundbuffers;
	static std::map<std::string, sf::Music*> musics;

	// private functions
	// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
	ResourceManager() { }

	// Loads and generates a shader from file
	static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile = nullptr);

	// Loads a single texture from file
	static Texture loadTextureFromFile(const GLchar* file);

	// Loads a single model and generate mesh object
	static Model* loadModelFromFile(const GLchar* file);

	// Loads a single sound from file
	static sf::Sound* loadSoundFromFile(const GLchar* file, std::string name);

public:
	// accessors
	// Retrieves a stored shader
	static Shader& getShader(std::string name);
	// Retrieves a stored texture
	static Texture& getTexture(std::string name);
	// Retrieves a stored model
	static Model* getModel(std::string name);
	// Retrieves a stored sprite
	static Sprite* getSprite(std::string name);
	// Retrieves a stored character
	static Character* getCharacter(std::string name);
	// Retrieves a stored sound
	static sf::Sound* getSound(std::string name);
	// Retrieves a music
	static sf::Music* getMusic(std::string name);
	// Get all sprites
	static std::map<std::string, Sprite*>& getSprites();
	// Get all characters
	static const std::map<std::string, Character*>& getCharacters();
	// Get all models
	static const std::map<std::string, Model*>& getModels();
	// Get all instances
	static const std::map<std::string, SpriteInstances*>& getAllInstances();
	// mutators
	// public functions

	// static functions
	// Check if a shader exists
	static bool findShader(std::string shaderName);
	// Check if a texture exists
	static bool findTexture(std::string textureName);
	// Check if a model exists
	static bool findModel(std::string modelName);
	// Check if a sprite exists
	static bool findSprite(std::string spriteName);
	// Check if a character exists
	static bool findCharacter(std::string characterName);
	// Check if an instances exists
	static bool findInstances(std::string instancesName);
	// Check if a sound exists
	static bool findSound(std::string soundName);
	// Check if a music exists
	static bool findMusic(std::string musicName);

	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader& loadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name);
	// Loads (and generates) a texture from file
	static Texture& loadTexture(const GLchar* file, std::string name);
	// Loads (and generate) a model with mesh and textures from file
	static Model* loadModel(const GLchar* file, std::string name);
	// Loads (and generate) a sound with soundbuffer from file
	static sf::Sound* loadSound(const GLchar* file, std::string name);
	// Loads (and generate) a music
	static sf::Music* loadMusic(const GLchar* file, std::string name);
	// Creates a sprite with texture
	static Sprite* createSprite(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName, std::string name);
	// Creates a character with texture
	static Character* createCharacter(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName, std::string name);
	// Creates a instances with texture
	static SpriteInstances* createInstances(std::string textureName, GLuint vbo, int n, std::string name);
	// Draw all the models within the scene
	static void drawModels(const Shader& shader);
	// Perform shadow mapping
	static void shadowTest(const Shader& shader);
	// Properly de-allocates all loaded resources
	static void clear();
};


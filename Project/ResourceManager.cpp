#include "ResourceManager.h"
#include "Model.h"
#include "Sprite.h"
#include "Character.h"
#include "SpriteInstances.h"

// Instantiate static variables
std::map<std::string, Texture>    ResourceManager::textures;
std::map<std::string, Shader>       ResourceManager::shaders;
std::map<std::string, Model*>       ResourceManager::models;
std::map<std::string, Sprite*>       ResourceManager::sprites;
std::map<std::string, Character*>   ResourceManager::characters;
std::map<std::string, SpriteInstances*> ResourceManager::allInstances;
std::map<std::string, sf::Sound*> ResourceManager::sounds;
std::map<std::string, sf::SoundBuffer*> ResourceManager::soundbuffers;
std::map<std::string, sf::Music*> ResourceManager::musics;

bool ResourceManager::findShader(std::string shaderName)
{
    return (shaders.find(shaderName) != shaders.end());
}

bool ResourceManager::findTexture(std::string textureName)
{
    return (textures.find(textureName) != textures.end());
}

bool ResourceManager::findModel(std::string modelName)
{
    return (models.find(modelName) != models.end());
}

bool ResourceManager::findSprite(std::string spriteName)
{
    return (sprites.find(spriteName) != sprites.end());
}

bool ResourceManager::findCharacter(std::string characterName)
{
    return (characters.find(characterName) != characters.end());
}

bool ResourceManager::findInstances(std::string instancesName)
{
    return (allInstances.find(instancesName) != allInstances.end());
}

bool ResourceManager::findSound(std::string soundName)
{
    return (sounds.find(soundName) != sounds.end());
}

bool ResourceManager::findMusic(std::string musicName)
{
    return (musics.find(musicName) != musics.end());
}

Shader& ResourceManager::loadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name)
{
    if (findShader(name)) return shaders[name];
    std::cout << "Load shaders " << vShaderFile << " and " << fShaderFile << std::endl;
    shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return shaders[name];
}

Texture& ResourceManager::loadTexture(const GLchar* file, std::string name)
{
    if (findTexture(name)) return textures[name];
    std::cout << "Load texture " << file << std::endl;
    textures[name] = loadTextureFromFile(file);
    return textures[name];
}

Model* ResourceManager::loadModel(const GLchar* file, std::string name)
{
    if (findModel(name)) return models[name];
    std::cout << "Load model " << file << std::endl;
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);
    models[name] = loadModelFromFile(file);
    //stbi_set_flip_vertically_on_load(false);
    return models[name];
}

sf::Sound* ResourceManager::loadSound(const GLchar* file, std::string name)
{
    if (findSound(name)) return sounds[name];
    std::cout << "Load sound " << file << std::endl;
    sounds[name] = loadSoundFromFile(file, name);
    return sounds[name];
}

sf::Music* ResourceManager::loadMusic(const GLchar* file, std::string name) {
    if (findMusic(name)) return musics[name];
    sf::Music* music = new sf::Music;
    if (!music->openFromFile(file)) {
        std::cout << "Failed to load sound " << file << std::endl;
        abort();
        return nullptr;
    }
    musics[name] = music;
    return music;
}

Sprite* ResourceManager::createSprite(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName, std::string name)
{
    if (findSprite(name)) return sprites[name];
    if (!findTexture(textureName)) {
        cout << "Texture: " << textureName << " not found, fail to create sprite: " << name << endl;
        return nullptr;
    }
    sprites[name] = new Sprite(position, size, rotation, textureName);
    return sprites[name];
}

Character* ResourceManager::createCharacter(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName, std::string name)
{
    if (findCharacter(name)) return characters[name];
    if (!findTexture(textureName)) {
        cout << "Texture: " << textureName << " not found, fail to create character: " << name << endl;
        return nullptr;
    }
    characters[name] = new Character(position, size, rotation, textureName);
    sprites[name] = characters[name];
    return characters[name];
}

SpriteInstances* ResourceManager::createInstances(std::string textureName, GLuint vbo, int n, std::string name)
{
    if (findInstances(name)) return allInstances[name];
    if (!findTexture(textureName)) {
        cout << "Texture: " << textureName << " not found, fail to create instances: " << name << endl;
        return nullptr;
    }
    allInstances[name] = new SpriteInstances(textureName, vbo, n);
    return allInstances[name];
}

void ResourceManager::drawModels(const Shader& shader)
{
    for (const auto& pair : models) {
        pair.second->draw(shader);
    }
}

void ResourceManager::shadowTest(const Shader& shader)
{
    for (const auto& pair : models) {
        pair.second->shadowTest(shader);
    }
}

Shader& ResourceManager::getShader(std::string name)
{
    if (!findShader(name)) std::cerr << "Shader " << name << " not found" << std::flush;
    return shaders[name];
}

Texture& ResourceManager::getTexture(std::string name)
{
    if (!findTexture(name)) std::cerr << "Texture " << name << " not found" << std::flush;
    return textures[name];
}

Model* ResourceManager::getModel(std::string name)
{
    if (!findModel(name)) std::cerr << "Model " << name << " not found" << std::flush;
    return models[name];
}

Sprite* ResourceManager::getSprite(std::string name)
{
    if (!findSprite(name)) std::cerr << "Sprite " << name << " not found" << std::flush;
    return sprites[name];
}

Character* ResourceManager::getCharacter(std::string name)
{
    if (!findCharacter(name)) std::cerr << "Character " << name << " not found" << std::flush;
    return characters[name];
}

sf::Sound* ResourceManager::getSound(std::string name)
{

    if (!findSound(name)) std::cerr << "Sound " << name << " not found" << std::flush;
    return sounds[name];
}


std::map<std::string, Sprite*>& ResourceManager::getSprites()
{
    return sprites;
}


sf::Music* ResourceManager::getMusic(std::string name)
{
    if (!findMusic(name)) std::cerr << "Music " << name << " not found" << std::flush;
    return musics[name];
}

const std::map<std::string, Character*>& ResourceManager::getCharacters()
{
    return characters;
}

const std::map<std::string, Model*>& ResourceManager::getModels()
{
    return models;
}

const std::map<std::string, SpriteInstances*>& ResourceManager::getAllInstances()
{
    return allInstances;
}

void ResourceManager::clear()
{
    // (Properly) delete all shaders	
    for (auto iter : shaders)
        glDeleteProgram(iter.second.getID());
    // (Properly) delete all textures
    for (auto iter : textures)
        glDeleteTextures(1, &iter.second.getID());
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // Open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // If geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();
    const GLchar* gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code
    Shader shader;
    shader.compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture ResourceManager::loadTextureFromFile(const GLchar* file)
{
    
    // Create Texture object
    Texture texture;
    // Load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        
        texture.setInternalFormat(format);
        texture.setImageFormat(format);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture " << file << std::endl;
    }

    texture.generate(width, height, data);
    stbi_image_free(data);

    return texture;
}

Model* ResourceManager::loadModelFromFile(const GLchar* file)
{
    return new Model(file);
}

sf::Sound* ResourceManager::loadSoundFromFile(const GLchar* file, std::string name)
{
    sf::SoundBuffer* buffer = new sf::SoundBuffer;
    if (!buffer->loadFromFile(file)) {
        std::cout << "Failed to load sound " << file << std::endl;
        abort();
    }
    soundbuffers[name] = buffer;
    sf::Sound* sound = new sf::Sound;
    sound->setBuffer(*buffer);
    return sound;
}

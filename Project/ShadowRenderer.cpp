#include "ShadowRenderer.h"
#include "Sprite.h"
#include <GL/Gl.h>
#include"Character.h"

static const char* vertexShaderPath = "../Shaders/spriteShadow.vs";
static const char* fragmentShaderPath = "../Shaders/spriteShadow.fs";
const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

ShadowRenderer::ShadowRenderer()
{
	modelShadowShaderName = "modelShadowShader";
	spriteShadowShaderName = "spriteShadowShader";
	ResourceManager::loadShader("../Shaders/shadow.vs", "../Shaders/shadow.fs", nullptr, modelShadowShaderName);
	ResourceManager::loadShader("../Shaders/spriteShadow.vs", "../Shaders/spriteShadow.fs", nullptr, spriteShadowShaderName);

	float quadVertices[] = {
		0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // top left
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top left
	};
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	update();
}

void ShadowRenderer::update()
{
	glm::vec3 characterPosition = ResourceManager::getCharacter("character1")->getPosition();
	lightPos = glm::vec3(characterPosition.x-2.0f, characterPosition.y+3.0f, characterPosition.z-2.0f);

	glm::mat4 lightProjection, lightView;
	float nearPlane = 1.0f, farPlane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	lightView = glm::lookAt(lightPos, characterPosition, glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
}

void ShadowRenderer::render()
{
	update();
	glCullFace(GL_FRONT);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	mapModelShadow();
	mapSpriteShadow();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
}

void ShadowRenderer::mapModelShadow() const
{
	Shader& shadowShader = ResourceManager::getShader(modelShadowShaderName);
	shadowShader.use();
	shadowShader.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	ResourceManager::shadowTest(shadowShader);
}

void ShadowRenderer::mapSpriteShadow() const
{
	Shader& shadowShader = ResourceManager::getShader(spriteShadowShaderName);
	shadowShader.use();
	shadowShader.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
	shadowShader.setInteger("texture1", 0);
	const std::map<std::string, Sprite*>& sprites = ResourceManager::getSprites();
	for (const auto& pair : sprites) {
		const Sprite* sprite = pair.second;
		if (sprite->getRenderState() > 1) {
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::vec3 position = sprite->getPosition();
			position.x -= (sprite->getSize().y / 2.0) * 0.1;
			model = glm::translate(model, position);
			glm::vec2 size = sprite->getSize();
			model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
			model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
			model = glm::scale(model, glm::vec3(size, 1.0f));
			float rotation = sprite->getRotation().y;
			model = glm::rotate(model, rotation, glm::vec3(0, 1, 0));
			//model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
			//model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
			//model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
			shadowShader.setMatrix4("model", model);
			//render quads
			glBindVertexArray(quadVAO);
			// bind textures on corresponding texture units
			glActiveTexture(GL_TEXTURE0);
			ResourceManager::getTexture(sprite->getTextureName()).bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}




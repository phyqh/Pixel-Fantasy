#include "ModelRenderer.h"
#include "Model.h"
#include "PhysicsEngine.h"

static const char* vertexShaderPath = "../Shaders/main.vs";
static const char* fragmentShaderPath = "../Shaders/main.fs";

using namespace std;

ModelRenderer::ModelRenderer(ShadowRenderer* shadowRenderer)
{
	this->shadowRenderer = shadowRenderer;
	shaderName = "modelShader";
	ResourceManager::loadShader(vertexShaderPath, fragmentShaderPath, nullptr, shaderName);
}

void ModelRenderer::render(const Camera& camera) const
{
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	Shader& modelShader = ResourceManager::getShader(shaderName);
	modelShader.use();
	modelShader.setInteger("textureDiffuse1", 0);
	modelShader.setInteger("shadowMap", 1);;

	// modelShader.setVector3f("lightColor", 1.0f, 1.0f, 1.0f);
	// modelShader.setVector3f("lightDir", -1.0f, -1.0f, -1.0f);
	modelShader.setVector3f("lightPos", glm::vec3(-50.0, 50.0, -50.0));
	modelShader.setVector3f("viewPos", camera.getPosition());

	// view/projection transformations
	glm::mat4 projection = camera.GetProjectionMatrix();
	glm::mat4 view = camera.GetViewMatrix();
	modelShader.setMatrix4("projection", projection);
	modelShader.setMatrix4("view", view);
	modelShader.setMatrix4("lightSpaceMatrix", shadowRenderer->getLightSpaceMatrix());
	modelShader.setFloat("fogDensity", 0.1);
	modelShader.setFloat("fogGradient", 0.5);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowRenderer->getShadowMap());

	ResourceManager::drawModels(modelShader);
}



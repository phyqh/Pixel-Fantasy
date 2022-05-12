#include "uiRenderer.h"

static const char* vertexShaderPath = "../Shaders/icon.vs";
static const char* fragmentShaderPath = "../Shaders/icon.fs";

uiRenderer::uiRenderer()
{
    shaderName = "uiShader";
    float quadVertices[] = {
        0.1f,  0.15f, 0.0f, 1.0f, 1.0f,  // top right
        0.1f, -0.15f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.1f, 0.15f, 0.0f, 0.0f, 1.0f, // top left
        0.1f, -0.15f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.1f, -0.15f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.1f,  0.15f, 0.0f, 0.0f, 1.0f   // top left
    };

    // character VAO, VBO
    ResourceManager::loadShader(vertexShaderPath, fragmentShaderPath, nullptr, shaderName);

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
}

uiRenderer::~uiRenderer()
{
}

void uiRenderer::render(int lifeCount, int StaminaCount, int BosslifeCount, int gameState, int bossDieRenderState, int ninjaState, int ninjalifeCount) const
{
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    // bind textures on corresponding texture units
    Shader& uiShader = ResourceManager::getShader(shaderName);
    uiShader.use();
    uiShader.setInteger("texture1", 0);
    uiShader.setFloat("pixelization", 16.0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("heart").getID());
    for (int i = 0; i < lifeCount; i++) {
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::vec2 size = glm::vec2(0.2, 0.2);
        model = glm::translate(model, glm::vec3(-0.97 + 0.04 * i, 0.78, 0));
        model = glm::scale(model, glm::vec3(size, 1.0f));
        uiShader.setMatrix4("model", model);
        //render quads
        glBindVertexArray(quadVAO);
        // bind textures on corresponding texture units
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("sp").getID());
    for (int i = 0; i < StaminaCount; i++) {
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::vec2 size = glm::vec2(0.2, 0.2);
        model = glm::translate(model, glm::vec3(-0.97 + 0.04 * i, 0.7, 0));
        model = glm::scale(model, glm::vec3(size, 1.0f));
        uiShader.setMatrix4("model", model);
        //render quads
        glBindVertexArray(quadVAO);
        // bind textures on corresponding texture units
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    if (lifeCount <= 0) {
        uiShader.setFloat("pixelization", 128);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("you die").getID());
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::vec2 size = glm::vec2(2.5, 2.0);
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(size, 1.0f));
        uiShader.setMatrix4("model", model);
        //render quads
        glBindVertexArray(quadVAO);
        // bind textures on corresponding texture units
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    if (BosslifeCount <= 0 && gameState != 2) {
        uiShader.setFloat("pixelization", 128);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("boss die").getID());
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::vec2 size = glm::vec2(2.5, 2.0);
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(size, 1.0f));
        uiShader.setMatrix4("model", model);
        //render quads
        glBindVertexArray(quadVAO);
        // bind textures on corresponding texture units
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Monster
    if (gameState > 0) {
        if (BosslifeCount > 0) {
            uiShader.setFloat("pixelization", 128);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("killer").getID());
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::vec2 size = glm::vec2(3.5, 0.8);
            model = glm::translate(model, glm::vec3(0.0, -0.6, 0.0));
            model = glm::scale(model, glm::vec3(size, 1.0f));
            uiShader.setMatrix4("model", model);
            //render quads
            glBindVertexArray(quadVAO);
            // bind textures on corresponding texture units
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("heart").getID());
        uiShader.setFloat("pixelization", 16.0);
        for (int i = 0; i < BosslifeCount; i++) {
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::vec2 size = glm::vec2(0.2, 0.2);
            model = glm::translate(model, glm::vec3(0.04 * i - 0.15, -0.75, 0));
            model = glm::scale(model, glm::vec3(size, 1.0f));
            uiShader.setMatrix4("model", model);
            //render quads
            glBindVertexArray(quadVAO);
            // bind textures on corresponding texture units
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }


    // Ninja health
    if (ninjaState > 0) {
        uiShader.setFloat("pixelization", 128);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("killer").getID());
        //glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //glm::vec2 size = glm::vec2(3.5, 0.8);
        //model = glm::translate(model, glm::vec3(0.0, -0.6, 0.0));
        //model = glm::scale(model, glm::vec3(size, 1.0f));
        //uiShader.setMatrix4("model", model);
        ////render quads
        //glBindVertexArray(quadVAO);
        //// bind textures on corresponding texture units
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ResourceManager::getTexture("heart").getID());
        uiShader.setFloat("pixelization", 16.0);
        for (int i = 0; i < ninjalifeCount; i++) {
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::vec2 size = glm::vec2(0.2, 0.2);
            model = glm::translate(model, glm::vec3(0.04 * i - 0.15, -0.75, 0));
            model = glm::scale(model, glm::vec3(size, 1.0f));
            uiShader.setMatrix4("model", model);
            //render quads
            glBindVertexArray(quadVAO);
            // bind textures on corresponding texture units
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }


}

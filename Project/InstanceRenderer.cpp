#include "InstanceRenderer.h"
#include "SpriteInstances.h"

static const char* vertexShaderPath = "../Shaders/instance.vs";
static const char* fragmentShaderPath = "../Shaders/instance.fs";

InstanceRenderer::InstanceRenderer() {
    shaderName = "instanceShader";
    ResourceManager::loadShader(vertexShaderPath, fragmentShaderPath, nullptr, shaderName);

    // generate a list of 100 quad locations/translation-vectors
    // ---------------------------------------------------------
    /*int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }*/


    float quadVertices[] = {
        0.1f,  0.1f, 0.0f, 1.0f, 1.0f,  // top right
        0.1f, -0.1f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.1f, 0.1f, 0.0f, 0.0f, 1.0f, // top left
        0.1f, -0.1f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.1f, -0.1f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.1f,  0.1f, 0.0f, 0.0f, 1.0f   // top left
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

InstanceRenderer::~InstanceRenderer()
{
}

void InstanceRenderer::render(const Camera& camera)
{
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    // bind textures on corresponding texture units
    Shader& instanceShader = ResourceManager::getShader(shaderName);
    instanceShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    instanceShader.setMatrix4("scale", model);
    instanceShader.setMatrix4("view", view);
    instanceShader.setMatrix4("projection", projection);
    instanceShader.setFloat("fogDensity", 0.1);
    instanceShader.setFloat("fogGradient", 1.5);
    //glBindVertexArray(quadVAO);

    //ResourceManager::getTexture("head").bind();
    //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 2); // 100 triangles of 6 vertices each
    //glBindVertexArray(0);
    const auto& allInstances = ResourceManager::getAllInstances();
    for (const auto& pair : allInstances) {
        //std::cout << "Rendering instances: " << pair.first << std::endl;
        const SpriteInstances& instances = *pair.second;
        glActiveTexture(GL_TEXTURE0);
        ResourceManager::getTexture(instances.getTextureName()).bind();
        glBindVertexArray(quadVAO);
        // also set instance data
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, instances.getInstanceVBO()); // this attribute comes from a different vertex buffer

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.s
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.getNum()); // 100 triangles of 6 vertices each
        glBindVertexArray(0);
        glActiveTexture(0);
    }
}

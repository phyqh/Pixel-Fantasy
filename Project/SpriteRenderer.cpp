#include "SpriteRenderer.h"

static const char* vertexShaderPath = "../Shaders/sprite.vs";
static const char* fragmentShaderPath = "../Shaders/sprite.fs";


SpriteRenderer::SpriteRenderer()
{
    shaderName = "spriteShader";
    float quadVertices[] = {
        0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // top left
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top left
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

SpriteRenderer::~SpriteRenderer()
{
}



void SpriteRenderer::render(const Camera& camera) const
{
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    // bind textures on corresponding texture units
    Shader& spirteShader = ResourceManager::getShader(shaderName);
    spirteShader.use();

    const auto sprites = ResourceManager::getSprites();
    for (const auto& pair : sprites) {
        
        const Sprite sprite = *pair.second;
        if (sprite.getRenderState() != 0) {
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, sprite.getPosition());
            glm::vec2 size = sprite.getSize();
            model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
            model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
            glm::vec3 rotation = sprite.getRotation();
            model = glm::rotate(model, (rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, (rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, (rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, glm::vec3(size, 1.0f));


            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = camera.GetProjectionMatrix();

            spirteShader.setMatrix4("model", model);
            spirteShader.setMatrix4("view", view);
            spirteShader.setMatrix4("projection", projection);
            spirteShader.setFloat("fogDensity", 0.1);
            spirteShader.setFloat("fogGradient", 0.5);
            spirteShader.setFloat("ke", sprite.getke());

            //render quads
            glBindVertexArray(quadVAO);
            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0);
            ResourceManager::getTexture(sprite.getTextureName()).bind();

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

#include "DebugRenderer.h"

DebugRenderer::DebugRenderer(ShadowRenderer* shadowRenderer)
{
    this->shadowRenderer = shadowRenderer;
    shaderName = "debugShader";
    Shader debugDepthQuad = ResourceManager::loadShader("../Shaders/debug.vs", "../Shaders/debug.fs", nullptr, shaderName);
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    debugDepthQuad.use();
    debugDepthQuad.setInteger("depthMap", 0);
}

void DebugRenderer::render() const
{
    // render Depth map to quad for visual debugging
    Shader debugDepthQuad = ResourceManager::getShader(shaderName);
    debugDepthQuad.use();
    float near_plane = 1.0f, far_plane = 7.5f;
    debugDepthQuad.setFloat("near_plane", near_plane);
    debugDepthQuad.setFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowRenderer->getShadowMap());
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void DebugRenderer::render(const GLuint& texture) const
{
    // render Depth map to quad for visual debugging
    Shader debugDepthQuad = ResourceManager::getShader(shaderName);
    debugDepthQuad.use();
    float near_plane = 1.0f, far_plane = 7.5f;
    debugDepthQuad.setFloat("near_plane", near_plane);
    debugDepthQuad.setFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

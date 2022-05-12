#include "Mesh.h"

extern inline glm::vec3 minimum(const glm::vec3& a, const glm::vec3& b)
{
    return glm::vec3(min(a[0], b[0]), min(a[1], b[1]), min(a[2], b[2]));
}

extern inline glm::vec3 maximum(const glm::vec3& a, const glm::vec3& b)
{
    return glm::vec3(max(a[0], b[0]), max(a[1], b[1]), max(a[2], b[2]));
}

const BoundingBox& Mesh::computeBoundingBox(glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale)
{
    const BoundingBox& localBounds = localBoundingBox;

    glm::vec3 localMin = localBounds.min;
    glm::vec3 localMax = localBounds.max;

    glm::vec3 v, newMax, newMin;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translate);
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    v = model * glm::vec4(localMin[0], localMin[1], localMin[2], 1);
    newMax = v;
    newMin = v;
    v = model * glm::vec4(localMax[0], localMin[1], localMin[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = model * glm::vec4(localMin[0], localMax[1], localMin[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = model * glm::vec4(localMax[0], localMax[1], localMin[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = model * glm::vec4(localMin[0], localMin[1], localMax[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = model * glm::vec4(localMax[0], localMin[1], localMax[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = model * glm::vec4(localMin[0], localMax[1], localMax[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = model * glm::vec4(localMax[0], localMax[1], localMax[2], 1);
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);

    this->aabb.max = newMax;
    this->aabb.min = newMin;

    BoundingBox bb = aabb;
    std::cout << "new boundingbox:" << "(" << bb.min[0] << ", " << bb.min[1] << ", " << bb.min[2] << ") "
        << "(" << bb.max[0] << ", " << bb.max[1] << ", " << bb.max[2] << ")" << endl;

    return aabb;
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);

    glBindVertexArray(meshVAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glBindVertexArray(0);
}

Mesh::Mesh()
{
}

Mesh::Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<string>&& textureNames, BoundingBox&& bb)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textureNames = textureNames;
    this->localBoundingBox = bb;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

void Mesh::draw(const Shader& shader) const
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textureNames.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        Texture& texture = ResourceManager::getTexture(textureNames[i]);
        TextureType type = texture.getTextureType();
        string name;
        if (type == TextureType::TEXTURE_DIFFUSE) name = "textureDiffuse" + std::to_string(diffuseNr++);
        else if (type == TextureType::TEXTURE_SPECULAR) name = "textureSpecular" + std::to_string(specularNr++);
        else if (type == TextureType::TEXTURE_NORMAL) name = "textureNormal" + std::to_string(normalNr++);
        else if (type == TextureType::TEXTURE_HEIGHT) name = "textureHeight" + std::to_string(heightNr++);
        else std::cout << "Unknown texture type!" << std::endl;

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.getID(), name.c_str()), i);
        // and finally bind the texture
        texture.bind();
    }

    // draw mesh
    glBindVertexArray(meshVAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::draw() const
{
    // draw mesh
    glBindVertexArray(meshVAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



#pragma once

#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "BoundingBox.h"
#include "Libs.h"

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // texCoords
    glm::vec2 texCoords;
    // tangent
    glm::vec3 tangent;
    // bitangent
    glm::vec3 bitangent;
    //bone indexes which will influence this vertex
    int boneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float weights[MAX_BONE_INFLUENCE];
};

class Mesh {
private:
    // variables
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<string>      textureNames;
    // render data 
    unsigned int meshVBO, meshVAO, meshEBO;
    // bounding box
    BoundingBox localBoundingBox, aabb;

    // static variables
    // private 
    // initializes all the buffer objects/arrays
    void setupMesh();
public:
    // constructors and destructors
    Mesh();
    Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<string>&& textureNames, BoundingBox&& bb);
    // accessors
    const BoundingBox& getLocalBoundingBox() const { return this->localBoundingBox; }
    const BoundingBox& getBoundingBox() const { return this->aabb; }
    const vector<Vertex>& getVertices() const { return this->vertices; }
    const vector<unsigned int>& getIndices() const { return this->indices; }
    // mutators
    // public functions
    void draw(const Shader& shader) const;
    void draw() const;
    // compute local bounding box
    const BoundingBox& computeBoundingBox(glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale);
    // static functions
};
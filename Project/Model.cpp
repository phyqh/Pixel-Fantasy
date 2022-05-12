#include "Model.h"
#include "Character.h"


void Model::loadModel(string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<string> textureNames;
    constexpr float inf = numeric_limits<float>::max();
    glm::vec3 aabbMax(-inf, -inf, -inf), aabbMin(inf, inf, inf);

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // bounding box
        aabbMax.x = max(vector.x, aabbMax.x);
        aabbMax.y = max(vector.y, aabbMax.y);
        aabbMax.z = max(vector.z, aabbMax.z);
        aabbMin.x = min(vector.x, aabbMin.x);
        aabbMin.y = min(vector.y, aabbMin.y);
        aabbMin.z = min(vector.z, aabbMin.z);

        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'textureDiffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: textureDiffuseN
    // specular: textureSpecularN
    // normal: textureNormalN
    // height: textureHeightN

    // 1. diffuse maps
    vector<string> diffuseMapNames = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::TEXTURE_DIFFUSE);
    textureNames.insert(textureNames.end(), diffuseMapNames.begin(), diffuseMapNames.end());
    // 2. specular maps
    vector<string> specularMapNames = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::TEXTURE_SPECULAR);
    textureNames.insert(textureNames.end(), specularMapNames.begin(), specularMapNames.end());
    // 3. normal maps
    vector<string> normalMapNames = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::TEXTURE_NORMAL);
    textureNames.insert(textureNames.end(), normalMapNames.begin(), normalMapNames.end());
    // 4. height maps
    vector<string> heightMapNames = loadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::TEXTURE_HEIGHT);
    textureNames.insert(textureNames.end(), heightMapNames.begin(), heightMapNames.end());

    // return a mesh object created from the extracted mesh data
    BoundingBox aabb;
    aabb.max = aabbMax;
    aabb.min = aabbMin;
    return Mesh(std::move(vertices), std::move(indices), std::move(textureNames), std::move(aabb));
}

vector<string> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType)
{
    vector<string> textureNames;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        string fileName = string(str.C_Str());
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        if (ResourceManager::findTexture(fileName)) {
            textureNames.push_back(fileName);
            continue; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
        }
        else {
            // if texture hasn't been loaded already, load it
            string filePath = directory + '/' + fileName;
            Texture& texture = ResourceManager::loadTexture(filePath.c_str(), fileName);
            texture.setTextureType(textureType);
            textureNames.push_back(fileName);
        }
    }

    return textureNames;
}

const BoundingBox& Model::computeBoundingBox()
{
    BoundingBox localBounds = computeLocalBoundingBox();

    glm::vec3 localMin = localBounds.min;
    glm::vec3 localMax = localBounds.max;

    glm::vec3 v, newMax, newMin;

    v = localToGlobal(glm::vec3(localMin[0], localMin[1], localMin[2]));
    newMax = v;
    newMin = v;
    v = localToGlobal(glm::vec3(localMax[0], localMin[1], localMin[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = localToGlobal(glm::vec3(localMin[0], localMax[1], localMin[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = localToGlobal(glm::vec3(localMax[0], localMax[1], localMin[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = localToGlobal(glm::vec3(localMin[0], localMin[1], localMax[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = localToGlobal(glm::vec3(localMax[0], localMin[1], localMax[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = localToGlobal(glm::vec3(localMin[0], localMax[1], localMax[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);
    v = localToGlobal(glm::vec3(localMax[0], localMax[1], localMax[2]));
    newMax = maximum(newMax, v);
    newMin = minimum(newMin, v);

    this->aabb.max = newMax;
    this->aabb.min = newMin;

    for (Mesh& mesh : meshes) mesh.computeBoundingBox(translate, rotate, scale);

    BoundingBox bb = aabb;
    std::cout << "Final boundingbox:" << "(" << bb.min[0] << ", " << bb.min[1] << ", " << bb.min[2] << ") "
        << "(" << bb.max[0] << ", " << bb.max[1] << ", " << bb.max[2] << ")" << std::endl;

    return aabb;
}

glm::vec3 Model::localToGlobal(glm::vec3 point) const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translate);
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model * glm::vec4(point, 1);
}

BoundingBox Model::computeLocalBoundingBox()
{
    constexpr float inf = numeric_limits<float>::max();
    glm::vec3 aabbMax(-inf, -inf, -inf), aabbMin(inf, inf, inf);
    for (const auto& mesh : meshes) {
        const BoundingBox& localBoundingBox = mesh.getLocalBoundingBox();
        aabbMax = maximum(aabbMax, localBoundingBox.max);
        aabbMin = minimum(aabbMin, localBoundingBox.min);
    }

    BoundingBox localBoundingBox;
    localBoundingBox.max = aabbMax;
    localBoundingBox.min = aabbMin;

    return localBoundingBox;
}


Model::Model(string const& path, bool gamma) : gammaCorrection(gamma), pixelization(128), root(nullptr)
{
    loadModel(path);
    translate = glm::vec3(0.0f);
    rotate = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    computeBoundingBox();

    vector<Mesh*> m;
    for (auto& mesh : meshes) m.push_back(&mesh);
    cout << "# of meshes:" << m.size() << endl;
    root = new BVHNode(m);
}

Model::~Model()
{
    if (root) delete root;
}

glm::mat4 Model::getTransform() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translate); // translate it down so it's at the center of the scene
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);	// it's a bit too big for our scene, so scale it down

    return model;
}

void Model::draw(const Shader& shader) const {
    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translate); // translate it down so it's at the center of the scene
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);	// it's a bit too big for our scene, so scale it down
    shader.setMatrix4("model", model);
    shader.setFloat("pixelization", pixelization);
    glm::vec3 characterPosition = ResourceManager::getCharacter("character1")->getPosition();
    for (unsigned int i = 0; i < meshes.size(); i++) {
        const BoundingBox& aabb = meshes[i].getBoundingBox();
        glm::vec3 centroid = aabb.centroid();
        glm::vec3 diff = centroid - characterPosition;
        if (!aabb.intersects(characterPosition) && glm::dot(diff, diff) > 5000.0 && aabb.size() < 100.0) continue;
        meshes[i].draw(shader);
    }
}

void Model::shadowTest(const Shader& shader) const
{
    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translate); // translate it down so it's at the center of the scene
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);	// it's a bit too big for our scene, so scale it down
    shader.setMatrix4("model", model);

    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw();
}

bool Model::BVHSearch(glm::vec3 position) const
{
    return root->search(position);
}

BVHNode::BVHNode(std::vector<Mesh*> m): meshes(m), left(nullptr), right(nullptr), aabb()
{
    for (auto* mesh : meshes) {
        const BoundingBox& bb = mesh->getBoundingBox();
        aabb.max = maximum(aabb.max, bb.max);
        aabb.min = minimum(aabb.min, bb.min);
    }
    
    
    if (meshes.size() > 3) {
        std::cout << "Recursively build BVH!" << std::endl;
        int axis = 0;
        glm::vec3 diff = aabb.max - aabb.min;
        if (diff.y > diff.x && diff.y > diff.z) axis = 1;
        if (diff.z > diff.y && diff.z > diff.x) axis = 2;
        
        if (axis == 0) sort(meshes.begin(), meshes.end(), [](Mesh* a, Mesh* b)
            { return a->getBoundingBox().centroid().x > b->getBoundingBox().centroid().x; });
        else if (axis == 1) sort(meshes.begin(), meshes.end(), [](Mesh* a, Mesh* b)
            { return a->getBoundingBox().centroid().y > b->getBoundingBox().centroid().y; });
        else if (axis == 2) sort(meshes.begin(), meshes.end(), [](Mesh* a, Mesh* b)
            { return a->getBoundingBox().centroid().z > b->getBoundingBox().centroid().z; });

        int half = meshes.size() / 2;
        vector<Mesh*> l, r;
        for (int i = 0; i < half; i++) l.push_back(meshes[i]);
        for (int i = half; i < meshes.size(); i++) r.push_back(meshes[i]);

        left = new BVHNode(l);
        right = new BVHNode(r);
    }
}

BVHNode::~BVHNode()
{
    if (left) delete left;
    if (right) delete right;
}

bool BVHNode::search(glm::vec3 position)
{

    if (left == nullptr && right == nullptr) {
        for (auto* mesh : meshes) {
            if (mesh->getBoundingBox().intersects(position)) return true;
        }
        return false;
    }
    //return left->search(position) || right->search(position);
    if (aabb.intersects(position)) return left->search(position) || right->search(position);

    return false;
}

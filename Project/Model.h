#pragma once

#include "Mesh.h"
#include <algorithm>

inline glm::vec3 minimum(const glm::vec3& a, const glm::vec3& b);
inline glm::vec3 maximum(const glm::vec3& a, const glm::vec3& b);

class BVHNode {
private:
	std::vector<Mesh*> meshes;
	BoundingBox aabb;
	BVHNode* left;
	BVHNode* right;
public:
	BVHNode(std::vector<Mesh*> m);
	~BVHNode();
	std::vector<Mesh*>& getMeshes() { return this->meshes; }
	bool search(glm::vec3 position);
};

class Model {
private:
	// variables
	std::vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;
	glm::vec3 translate;
	glm::vec3 rotate;
	glm::vec3 scale;
	BoundingBox aabb;
	int pixelization;
	BVHNode* root;
	// static variables
	// private functions
	
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const& path);
	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<string> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);
	// compute local bounding box
	BoundingBox computeLocalBoundingBox();
	// compute global bounding box
	const BoundingBox& computeBoundingBox();
	// transform local coords to global space
	glm::vec3 localToGlobal(glm::vec3 point) const;

public:
	 // constructors and destructorsss
	 Model(const string& path, bool gamma = false);
	 ~Model();
	 // accessors
	 const BoundingBox& getBoundingBox() const { return this->aabb; };
	 const std::vector<Mesh>& getMeshes() const { return this->meshes; }
	 glm::mat4 getTransform() const;
	 int getPixelization() const { return this->pixelization; }
	 // mutators
	 void setTranslation(glm::vec3 t) { this->translate = t; computeBoundingBox(); }
	 void setRotation(glm::vec3 r) { this->rotate = r; computeBoundingBox(); }
	 void setScale(glm::vec3 s) { this->scale = s; computeBoundingBox();}
	 void setPixelization(int p) { this->pixelization = p; }
	 // public functions
	 void draw(const Shader& shader) const;
	 void shadowTest(const Shader& shader) const;
	 bool BVHSearch(glm::vec3 position) const;
	 // static fucnctions
};
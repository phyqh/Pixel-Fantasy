#include "PhysicsEngine.h"
#include "Model.h"

const float kEpsilon = 1e-8;

extern inline bool insideTriangle(const glm::vec3& O, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
    glm::vec2 v0(C.x - A.x, C.z - A.z);
    glm::vec2 v1(B.x - A.x, B.z - A.z);
    glm::vec2 v2(O.x - A.x, O.z - A.z);

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
    if (u < 0 || u > 1) // if u out of range, return directly
    {
        return false;
    }

    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
    if (v < 0 || v > 1) // if v out of range, return directly
    {
        return false;
    }

    return u + v <= 1;
}

extern inline bool rayTriangleIntersect(const glm::vec3& O, const glm::vec3& D, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float& t)
{
    if (!insideTriangle(O, A, B, C)) return false;
    glm::vec3 E1 = B - A;
    glm::vec3 E2 = C - A;
    glm::vec3 P = glm::cross(D, E2);
    float det = glm::dot(E1, P);
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false;
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;
    float invDet = 1 / det;

    glm::vec3 T = O - A;
    float u = glm::dot(T, P) * invDet;
    if (u < 0 || u > 1) return false;

    glm::vec3 Q = glm::cross(T, E1);
    float v = glm::dot(D, Q) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = glm::dot(E2, Q) * invDet;

    return true;
}


double PhysicsEngine::computeGroundHeight(glm::vec3 position, unsigned int& lastFaceIndex)
{
    Model* landscape = ResourceManager::getModel("landscape");
    glm::mat4 model = landscape->getTransform();
    const vector<Mesh>& meshes = landscape->getMeshes();
    float tMin = std::numeric_limits<float>::max();
    //cout << "# of meshes: " << meshes.size() << endl;
    for (const auto& mesh : meshes) {
        if (mesh.getBoundingBox().includes(position)) {
            const vector<Vertex>& vertices = mesh.getVertices();
            const vector<unsigned int>& indices = mesh.getIndices();
            //cout << "Mesh # of vertices:" << vertices.size() << " # of indices: " << indices.size() << " # of faces: " << indices.size() / 3 << endl;
            float t;
            bool found = false;
            unsigned int lower = (lastFaceIndex - 27 < 0) ? 0 : lastFaceIndex - 27,
                upper = (lastFaceIndex + 27 >= indices.size()) ? indices.size() - 1 : lastFaceIndex + 27;
            for (unsigned long i = lower; i < upper; i += 3) {
                if (rayTriangleIntersect(position, glm::vec3(0, -1, 0),
                    model * glm::vec4(vertices[indices[i]].position, 1),
                    model * glm::vec4(vertices[indices[i + 1]].position, 1),
                    model * glm::vec4(vertices[indices[i + 2]].position, 1), t)) {
                    tMin = min(tMin, t);
                    lastFaceIndex = i;
                    found = true;
                    //cout << "Fast!" << endl;
                    break;
                }
            }
            if (!found) {
                for (unsigned long long i = 0; i < indices.size(); i += 3) {
                    if (rayTriangleIntersect(position, glm::vec3(0, -1, 0),
                        model * glm::vec4(vertices[indices[i]].position, 1),
                        model * glm::vec4(vertices[indices[i + 1]].position, 1),
                        model * glm::vec4(vertices[indices[i + 2]].position, 1), t)) {
                        tMin = min(tMin, t);
                        lastFaceIndex = i;
                        found = true;
                        //cout << "slow!" << endl;
                        break;
                    }
                }
            }
        }
        else {
            /*glm::vec3 point = position;
            BoundingBox bb = landscape->getBoundingBox();
            std::cout << "boundingbox:" << "(" << bb.min[0] << ", " << bb.min[1] << ", " << bb.min[2] << ") "
                << "(" << bb.max[0] << ", " << bb.max[1] << ", " << bb.max[2] << ")"
                << " not include point: (" << point[0] << ", " << point[1] << " ," << point[2] << ")" << std::endl;*/
        }
    }

    return position.y - tMin;
}

bool PhysicsEngine::checkCollision(glm::vec3 position)
{
    auto& models = ResourceManager::getModels();
    for (auto& pair : models) {
        if (pair.first == "landscape" || pair.first == "water") continue;
        auto model = pair.second;
        return model->BVHSearch(position);
        /*const BoundingBox& aabb = model->getBoundingBox();
        if (aabb.intersects(position)) {
            auto meshes = model->getMeshes();
            for (auto& mesh : meshes) {
                if (mesh.getBoundingBox().intersects(position)) {
                    return true;
                }
            }
        }*/
    }
    return false;
}

#include "Camera.h"
#define PI 3.14159265
// constructors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float aspect) : speed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->front = glm::vec3(0.9f, -0.5f, 0.0f);
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->aspect = aspect;
    pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float aspect) : speed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->front = glm::vec3(0.9f, -0.5f, 0.0f);
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    this->aspect = aspect;
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = speed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
    // make sure the user stays at the ground level
    
    // position.y = POSITION.y; // <-- this one-liner keeps the user at the ground level (xz plane)
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::focusTranslate(glm::vec3 pos, double distX, double distZ) {
    double distanceX = abs(pos.x - position.x);
    double distanceZ = abs(pos.z - position.z);
    double cosTheta = distanceZ / sqrt(distanceX * distanceX + distanceZ * distanceZ);
    double sinTheta = distanceX / sqrt(distanceX * distanceX + distanceZ * distanceZ);

    this->position = glm::vec3(pos.x + distX, pos.y, pos.z + distZ);
}

void Camera::focusRotate(glm::vec3 pos) {/*
    this->position = glm::vec3(pos.x, pos.y, pos.z + 2);*/
    double distanceX = abs(pos.x - position.x);
    double distanceZ = abs(pos.z - position.z);
    this->yaw = -90;
    double cosTheta = distanceZ / sqrt(distanceX * distanceX + distanceZ * distanceZ);
    double sinTheta = distanceX / sqrt(distanceX * distanceX + distanceZ * distanceZ);

    double distance = 3;
    this->position.y = std::max((double)pos.y, PhysicsEngine::computeGroundHeight(this->position, lastFaceIndex) + 1.0);
    
    if (distanceZ < 0.001)
    {
        if (pos.x < position.x)
            this->yaw = -180;
        else
            this->yaw = 0;
    }
    else
    {
        if (pos.z < position.z && pos.x < position.x) {
            this->yaw = -90 - atan(distanceX / distanceZ) * 180 / PI;
            this->position.x = pos.x + distance * sinTheta;
            this->position.z = pos.z + distance * cosTheta;
        }
        else if (pos.z > position.z && pos.x < position.x) {
            this->yaw = 90 + atan(distanceX / distanceZ) * 180 / PI;
            this->position.x = pos.x + distance * sinTheta;
            this->position.z = pos.z - distance * cosTheta;
        }
        else if (pos.z > position.z && pos.x > position.x) {
            this->yaw = 90 - atan(distanceX / distanceZ) * 180 / PI;
            this->position.x = pos.x - distance * sinTheta;
            this->position.z = pos.z - distance * cosTheta;
        }
        else if (pos.z < position.z && pos.x > position.x) {
            this->yaw = -90 + atan(distanceX / distanceZ) * 180 / PI;
            this->position.x = pos.x - distance * sinTheta;
            this->position.z = pos.z + distance * cosTheta;
        }
    }
    if (abs(this->position.y - pos.y) < 0.01)
        this->pitch = 0;
    else
        this->pitch = -atan((position.y-pos.y)/3) * 180 / PI;
    updateCameraVectors();
}
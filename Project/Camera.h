#pragma once

#include "Libs.h"
#include "PhysicsEngine.h"
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const glm::vec3 POSITION = glm::vec3(-4.0f, 0.48f, 3.0f);
const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float ASPECT = 1.77777778;
const float YAW = -90.0f;
const float PITCH = -30.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
private:
    // variables
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float aspect;
    // euler Angles
    float yaw;
    float pitch = 0; // IMPORTANT: Initial value for pitch when the scene is rendered first time
    // camera options
    float speed;
    float mouseSensitivity;
    float zoom;

    // static variables
    unsigned int lastFaceIndex;
    // private functions
    void updateCameraVectors();

public:
    // constructors and destructors
    // constructor with vectors
    Camera(glm::vec3 position = POSITION, glm::vec3 up = UP, float yaw = YAW, float pitch = PITCH, float aspect = ASPECT);
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float aspect);

    // accessors
    glm::vec3 getPosition() const { return this->position; }
    double getPitch() const { return pitch; }
    double getYaw() const { return yaw; }
    // mutators
    void focusTranslate(glm::vec3 pos, double distX, double distZ);
    void focusRotate(glm::vec3 pos);
    
    // functions
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    inline glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    // returns the perspective projection matrix
    inline glm::mat4 GetProjectionMatrix() const
    {
        return glm::perspective(glm::radians(zoom), aspect, 0.1f, 100.0f);
    }
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

    // static functions
};


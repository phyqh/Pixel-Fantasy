#include "Sprite.h"

Sprite::Sprite(glm::vec3 position, glm::vec2 size, GLfloat rotation, std::string textureName): position(position), size(size), rotation(rotation), textureName(textureName), 
currentDirection(0), renderState(2), ke(0.0), instanceVBO(-1)
{

}


void Sprite::setDirection(float direction) {
    currentDirection = direction;
}

double Sprite::getDirection() const {
    return currentDirection;
}

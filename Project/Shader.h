#pragma once

#include "Libs.h"

class Shader
{
private:
	// variables
    // State
    GLuint ID;

	// static variables

	// private functions
	// utility function for checking shader compilation/linking errors.
	void checkCompileErrors(GLuint shader, std::string type) const;
public:
	// constructors and destructors
    Shader() { }
	// accessors
    GLuint& getID() { return this->ID; }
    GLuint getID() const { return this->ID; }
	// mutators
	
	// public functions 
    
    // Compiles the shader from given source code
    void    compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr); // Note: geometry source code is optional 
   // Sets the current shader as active
    void use()const;
    // Utility functions
    void    setFloat(const GLchar* name, GLfloat value, GLboolean useShader = false) const;
    void    setInteger(const GLchar* name, GLint value, GLboolean useShader = false) const;
    void    setVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader = false) const;
    void    setVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader = false) const;
    void    setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false) const;
    void    setVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader = false) const;
    void    setVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false) const;
    void    setVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader = false) const;
    void    setMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean useShader = false) const;
};


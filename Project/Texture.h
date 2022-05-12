#pragma once

#include "Libs.h"
#include "stb_image.h"

enum class TextureType { TEXTURE_DIFFUSE, TEXTURE_SPECULAR, TEXTURE_NORMAL, TEXTURE_HEIGHT };

class Texture
{
private:
	// variables
	// // Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
	GLuint ID;
	// Texture image dimensions
	GLuint width, height; // Width and height of loaded image in pixels
	// Texture Format
	GLuint internalFormat; // Format of texture object
	GLuint imageFormat; // Format of loaded image
	// Texture configuration
	GLuint wrapS; // Wrapping mode on S axis
	GLuint wrapT; // Wrapping mode on T axis
	GLuint filterMin; // Filtering mode if texture pixels < screen pixels
	GLuint filterMax; // Filtering mode if texture pixels > screen pixels
	TextureType textureType;

	// static variables
	// private functions

public:
	// constructors and destructors
	// // Constructor (sets default texture modes)
	Texture();

	// accessors
	GLuint& getID() { return this->ID; }
	TextureType getTextureType() const { return this->textureType; }
	// mutators
	void setInternalFormat(GLuint format) { this->internalFormat = format; }
	void setImageFormat(GLuint format) { this->imageFormat = format; }
	void setTextureType(TextureType textureType) { this->textureType = textureType; }
	
	// public functions
	
	// // Generates texture from image data
	void generate(GLuint width, GLuint height, unsigned char* data);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void bind() const;

	// static functions
};


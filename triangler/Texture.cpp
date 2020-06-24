#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

#include "Texture.hpp"

Texture::Texture(const std::string path, const GLenum target) : target_(target), path_(path)
{
	// Generate new texture, bind it
	glGenTextures(1, &texture_);
	glBindTexture(target_, texture_);

	// Filtering
	glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Wrapping
	glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load image file
	stbi_set_flip_vertically_on_load(true);
	unsigned char* img = stbi_load(path_.c_str(), &width_, &height_, &channels_, 0);

	if (img)
	{
		glTexImage2D(target_, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(target_);
	}
	else
	{
		std::cout << "Texture init: Failed to load image file '" << path_ << "'" << std::endl;
	}

	stbi_image_free(img);
}

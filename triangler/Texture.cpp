#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

#include "Texture.hpp"

Texture::Texture(const std::string path, const GLenum target) : target_(target), path_(path)
{
	// Generate new texture, bind it
	glGenTextures(1, &texture_);
	//Bind();
	glBindTexture(target_, texture_);

	// Filtering
	glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Wrapping
	glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load image file
	stbi_set_flip_vertically_on_load(true);
	unsigned char* img = stbi_load(path_.c_str(), &width_, &height_, &channels_, 0);

	if (img)
	{
		glTexImage2D(target_, 0, GL_RGB, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(target_);
	}
	else
	{
		std::cout << "Texture init: Failed to load image file '" << path_ << "'" << std::endl;
	}

	stbi_image_free(img);
	std::cout << "Succesfully loaded " << width_ << "x" << height_ << " (" << channels_ << " channels) texture from '" << path_ << "'" << std::endl;
}

void Texture::Bind() const
{
	glBindTexture(target_, texture_);
}

void Texture::AttachNoReciprocation(Object3D* obj)
{
	objects_.emplace_back(obj);
}

void Texture::Attach(Object3D* obj)
{
	AttachNoReciprocation(obj);
	obj->AttachNoReciprocation(this);
}

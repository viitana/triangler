#pragma once
#include <string>

#include <GL/glew.h>

class Texture
{
public:
	Texture(const std::string path, const GLenum target = GL_TEXTURE_2D);
	//void Bind() const;
private:
	const GLenum target_;
	const std::string path_;

	GLuint texture_;
	int width_, height_, channels_;
};


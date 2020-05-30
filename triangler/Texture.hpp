#pragma once
#include <string>

#include <GL/glew.h>

class Texture
{
public:
	Texture(const GLenum target, const std::string path);
	//void Bind() const;
private:
	const GLenum target_;
	const std::string path_;

	GLuint texture_;
	int width_, height_, channels_;
};


#pragma once
#include <string>

#include <GL/glew.h>

#include "mutuallyattachable.hpp"
#include "object3d.hpp"

class Object3D;

class Texture
	: public MutuallyAttachableTo<Object3D>
{
public:
	Texture(const std::string path, const GLenum target = GL_TEXTURE_2D);

	// Inherited: MutuallyAttachable<Texture>
	void AttachNoReciprocation(Object3D* obj) override;
	void Attach(Object3D* obj) override;
	const GLuint GetTexture() const { return texture_; }
	void Bind() const;
private:
	const GLenum target_;
	const std::string path_;

	std::vector<Object3D*> objects_;
	GLuint texture_;
	int width_, height_, channels_;
};


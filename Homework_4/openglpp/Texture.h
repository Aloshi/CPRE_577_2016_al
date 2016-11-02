#pragma once

#include <gl/glew.h>  // for GL* types

class Texture
{
public:
	Texture();
	virtual ~Texture();

	void fromFile(const char* path);
	void setData(const unsigned char* data, unsigned int width, unsigned int height,
		GLenum in_fmt = GL_RGB, bool generateMipmaps = true);

	void setWrapMode(GLenum s, GLenum t);
	void setFilters(GLenum min, GLenum mag);

	void bind();
	void unbind();

private:
	GLuint mID;
};
#pragma once

#include <memory>  // std::shared_ptr

#include <GL/glew.h>  // for GL* types

class Texture
{
public:
	Texture();
	virtual ~Texture();

	static std::shared_ptr<Texture> fromFile(const char* path);
	void setData(const unsigned char* data, unsigned int width, unsigned int height,
		GLenum in_fmt = GL_RGB, bool generateMipmaps = true);

	void setWrapMode(GLenum s, GLenum t);
	void setFilters(GLenum min, GLenum mag);

	void bind();
	void unbind();

	inline unsigned int width() const { return mWidth; }
	inline unsigned int height() const { return mHeight; }

private:
	GLuint mID;

	unsigned int mWidth;
	unsigned int mHeight;
};

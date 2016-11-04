#include <openglpp/Texture.h>

#include <openglpp/LoadBMP.h>
#include <openglpp/GLException.h>

Texture::Texture()
	: mID(0), mWidth(0), mHeight(0)
{
	glGenTextures(1, &mID);

	if (mID == 0)
		throw GLException() << "Unable to generate new texture";

	bind();
	setWrapMode(GL_CLAMP, GL_CLAMP);
	setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	unbind();
}

Texture::~Texture()
{
	if (mID)
		glDeleteTextures(1, &mID);
}

std::shared_ptr<Texture> Texture::fromFile(const char* path)
{
	unsigned int width, height;
	std::vector<unsigned char> data = loadBMP(path, &width, &height);

	auto tex = std::make_shared<Texture>();
	tex->setData(data.data(), width, height, GL_BGR, true);
	return tex;
}

void Texture::setData(const unsigned char* data, unsigned int width, unsigned int height, GLenum in_fmt, bool generateMipmaps)
{
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, in_fmt, GL_UNSIGNED_BYTE, data);

	if (generateMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	unbind();

	mWidth = width;
	mHeight = height;
}

void Texture::setWrapMode(GLenum s, GLenum t)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
}

void Texture::setFilters(GLenum min, GLenum mag)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, mID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
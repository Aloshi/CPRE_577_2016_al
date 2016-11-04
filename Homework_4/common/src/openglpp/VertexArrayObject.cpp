#include <openglpp/VertexArrayObject.h>

#include <openglpp/VertexBufferObject.h>

#include <assert.h>
#include <iostream>

VertexArrayObject::VertexArrayObject()
	: mID(0)
{
	glGenVertexArrays(1, &mID);
}

VertexArrayObject::~VertexArrayObject()
{
	if (mID) {
		glDeleteVertexArrays(1, &mID);
	}

	std::cout << "VAO deleted\n";
}

void VertexArrayObject::bindVertexAttrib(const std::shared_ptr<VertexBufferObject>& vbo,
	GLuint attrib_idx, GLint size,
	GLenum type, GLboolean normalized,
	GLsizei stride, const GLvoid* offset)
{
	bind();
	vbo->bind();
	glVertexAttribPointer(attrib_idx, size, type, normalized, stride, offset);
	glEnableVertexAttribArray(attrib_idx);

	mVBOs.push_back(vbo);
}

void VertexArrayObject::bind()
{
	assert(mID != 0);
	glBindVertexArray(mID);
}
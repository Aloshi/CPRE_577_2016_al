#include <openglpp/VertexBufferObject.h>

#include <assert.h>
#include <iostream>

VertexBufferObject::VertexBufferObject()
	: mID(0)
{
	glGenBuffers(1, &mID);
}

VertexBufferObject::~VertexBufferObject()
{
	if (mID) {
		glDeleteBuffers(1, &mID);
	}
	std::cout << "VBO deleted\n";
}

void VertexBufferObject::bind()
{
	assert(mID != 0);
	glBindBuffer(GL_ARRAY_BUFFER, mID);
}

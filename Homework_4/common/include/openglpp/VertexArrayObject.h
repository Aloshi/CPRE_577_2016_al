#pragma once

#include <vector>
#include <memory>

#include <gl/glew.h>  // for GL* types

class VertexBufferObject;

class VertexArrayObject {
public:
	VertexArrayObject();
	virtual ~VertexArrayObject();

	VertexArrayObject(const VertexArrayObject&) = delete;
	VertexArrayObject& operator=(const VertexArrayObject&) = delete;

	// bind this vao, bind vbo, call glVertexAttribPointer with given args,
	// and enable vertex attrib array attrib_idx, add vbo to mVBOs to prevent cleanup
	void bindVertexAttrib(const std::shared_ptr<VertexBufferObject>& vbo,
		GLuint attrib_idx, GLint size,
		GLenum type, GLboolean normalized = GL_FALSE,
		GLsizei stride = 0, const GLvoid* offset = 0);

	void bind();

private:
	// keep a reference to VBOs so they don't get destroyed
	GLuint mID;
	std::vector< std::shared_ptr<VertexBufferObject> > mVBOs;
};
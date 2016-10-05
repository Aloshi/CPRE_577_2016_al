#pragma once

#include <gl/glew.h>  // for GL* types

class VertexBufferObject {
public:
	VertexBufferObject();
	virtual ~VertexBufferObject();

	VertexBufferObject(const VertexBufferObject&) = delete;
	VertexBufferObject& operator=(const VertexBufferObject&) = delete;

	void bind();

	// Will bind this VBO.
	// NOTE: size argument is number of elements of type T, *not* bytes like the OpenGL API!
	template <typename T>
	inline void bufferData(size_t n_elements, const T* data, GLenum usage) {
		bind();

		const GLsizeiptr n_bytes = n_elements * sizeof(T);
		glBufferData(GL_ARRAY_BUFFER, n_bytes, data, usage);
	}

private:
	GLuint mID;
};
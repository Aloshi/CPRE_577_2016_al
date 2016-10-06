#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <gl/glew.h>  // GLenum

#include "VertexBufferObject.h"

// manages VBOs for typical vertex attributes
class Mesh {
public:
	Mesh();
	Mesh(const Mesh& rhs) = delete;
	Mesh& operator=(const Mesh& rhs) = delete;

	void setVertices(const std::vector<glm::vec3>& vertices, GLenum primitive);
	void setNormals(const std::vector<glm::vec3>& normals);
	void setColors(const std::vector<glm::vec3>& colors);

	inline GLenum primitiveType() const { return mPrimitiveType; }
	inline GLuint numPrimitives() const { return mCount; }

	inline const std::shared_ptr<VertexBufferObject>& verticesVBO() const { assert(mHaveVertices); return mVertices; }
	inline const std::shared_ptr<VertexBufferObject>& normalsVBO() const { assert(mHaveNormals); return mNormals; }
	inline const std::shared_ptr<VertexBufferObject>& colorsVBO() const { assert(mHaveColors);  return mColors; }

private:
	std::shared_ptr<VertexBufferObject> mVertices;
	std::shared_ptr<VertexBufferObject> mNormals;
	std::shared_ptr<VertexBufferObject> mColors;

	GLenum mPrimitiveType;
	GLuint mCount;

	bool mHaveVertices;
	bool mHaveNormals;
	bool mHaveColors;
};
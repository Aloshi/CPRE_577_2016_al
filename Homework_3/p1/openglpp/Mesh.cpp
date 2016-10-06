#include "Mesh.h"

Mesh::Mesh()
	: mVertices(std::make_shared<VertexBufferObject>()),
	mNormals(std::make_shared<VertexBufferObject>()),
	mColors(std::make_shared<VertexBufferObject>()),
	mHaveVertices(false), mHaveNormals(false), mHaveColors(false)
{
}

void Mesh::setVertices(const std::vector<glm::vec3>& vertices, GLenum primitive)
{
	mVertices->bufferData(vertices.size(), vertices.data(), GL_STATIC_DRAW);

	mPrimitiveType = primitive;
	mCount = vertices.size();
	mHaveVertices = (vertices.size() > 0);
}

void Mesh::setNormals(const std::vector<glm::vec3>& normals)
{
	mNormals->bufferData(normals.size(), normals.data(), GL_STATIC_DRAW);
	mHaveNormals = (normals.size() > 0);
}

void Mesh::setColors(const std::vector<glm::vec3>& colors)
{
	mColors->bufferData(colors.size(), colors.data(), GL_STATIC_DRAW);
	mHaveColors = (colors.size() > 0);
}
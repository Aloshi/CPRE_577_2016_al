#include <openglpp/Mesh.h>

Mesh::Mesh()
	: mVertices(std::make_shared<VertexBufferObject>()),
	mNormals(std::make_shared<VertexBufferObject>()),
	mColors(std::make_shared<VertexBufferObject>()),
	mTexCoords(std::make_shared<VertexBufferObject>()),
	mTangents(std::make_shared<VertexBufferObject>()),
	mBitangents(std::make_shared<VertexBufferObject>()),
	mHaveVertices(false), mHaveNormals(false),
	mHaveColors(false), mHaveTexCoords(false)
{
}

void Mesh::setVertices(const std::vector<glm::vec3>& vertices, GLenum primitive)
{
	mVertexData = vertices;  // for tangents

	mVertices->bufferData(vertices.size(), vertices.data(), GL_STATIC_DRAW);

	mPrimitiveType = primitive;
	mCount = vertices.size();
	mHaveVertices = (vertices.size() > 0);

	if (mHaveVertices && mHaveTexCoords)
		rebuildTangents();
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

void Mesh::setTexCoords(const std::vector<glm::vec2>& texCoords)
{
	mTexCoordData = texCoords;  // for tangents

	mTexCoords->bufferData(texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
	mHaveTexCoords = (texCoords.size() > 0);

	if (mHaveVertices && mHaveTexCoords)
		rebuildTangents();
}

void Mesh::rebuildTangents()
{
	assert(mPrimitiveType == GL_TRIANGLES);

	std::vector<glm::vec3> tangents(mVertexData.size()), bitangents(mVertexData.size());
	for (unsigned int i = 0; i < mVertexData.size(); i += 3) {
		const glm::vec3* v = mVertexData.data() + i;
		const glm::vec2* uv = mTexCoordData.data() + i;

		glm::vec3 dp1 = v[1] - v[0];
		glm::vec3 dp2 = v[2] - v[0];

		glm::vec2 duv1 = uv[1] - uv[0];
		glm::vec2 duv2 = uv[2] - uv[0];

		float r = 1.0f / (duv1.x * duv2.y - duv1.y * duv2.x);
		glm::vec3 tangent = (dp1 * duv2.y - dp2 * duv1.y) * r;
		glm::vec3 bitangent = (dp2 * duv1.x - dp1 * duv2.x) * r;

		for (unsigned int j = 0; j < 3; j++) {
			tangents[i + j] = tangent;
			bitangents[i + j] = bitangent;
		}
	}

	mTangents->bufferData(tangents.size(), tangents.data(), GL_STATIC_DRAW);
	mBitangents->bufferData(bitangents.size(), bitangents.data(), GL_STATIC_DRAW);
}
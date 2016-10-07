#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Transform.h"
#include "VertexArrayObject.h"

// ties together a mesh and a material
struct Object {
public:
	Transform transform;
	Material material;

	inline void setMesh(const std::shared_ptr<Mesh>& mesh) {
		mMesh = mesh;
		mVAO.reset();
	}

	// apply our material, set the model matrix, and render the mesh
	inline void Render() {
		if (mMesh == nullptr)
			return;

		if (mVAO == nullptr)
			rebuildVAO();

		material.shader()->use();
		mVAO->bind();
		material.activate();
		material.shader()->setUniform("modelMatrix", transform.matrix());
		glDrawArrays(mMesh->primitiveType(), 0, mMesh->numPrimitives());
		glBindVertexArray(NULL);  // unbind VAO
	}

	inline void rebuildVAO() {
		mVAO = std::make_shared<VertexArrayObject>();

		if (Shader::POSITION_NAME)
			mVAO->bindVertexAttrib(mMesh->verticesVBO(), material.shader()->attrib(Shader::POSITION_NAME), 3, GL_FLOAT);
		if (Shader::NORMAL_NAME)
			mVAO->bindVertexAttrib(mMesh->normalsVBO(), material.shader()->attrib(Shader::NORMAL_NAME), 3, GL_FLOAT, GL_TRUE);
		if (Shader::COLOR_NAME)
			mVAO->bindVertexAttrib(mMesh->colorsVBO(), material.shader()->attrib(Shader::COLOR_NAME), 3, GL_FLOAT);
	}

private:
	std::shared_ptr<Mesh> mMesh;
	std::shared_ptr<VertexArrayObject> mVAO;
};

#pragma once

#include <memory>
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
	inline void render(const glm::mat4& parentTransform = glm::mat4(1.0f)) {
		if (!mChildren.empty())
			renderChildren(parentTransform);

		if (mMesh == nullptr)
			return;

		if (mVAO == nullptr)
			rebuildVAO();

		material.shader()->use();
		mVAO->bind();
		material.activate();
		material.shader()->setUniform("modelMatrix", parentTransform * transform.matrix());

		glDrawArrays(mMesh->primitiveType(), 0, mMesh->numPrimitives());
		glBindVertexArray(0);  // unbind VAO
		//glBindTexture(GL_TEXTURE_2D, 0);  // unbind textures
	}

	inline void rebuildVAO() {
		mVAO = std::unique_ptr<VertexArrayObject>(new VertexArrayObject());

		if (Shader::POSITION_NAME)
			mVAO->bindVertexAttrib(mMesh->verticesVBO(), material.shader()->attrib(Shader::POSITION_NAME), 3, GL_FLOAT);
		if (Shader::NORMAL_NAME)
			mVAO->bindVertexAttrib(mMesh->normalsVBO(), material.shader()->attrib(Shader::NORMAL_NAME), 3, GL_FLOAT, GL_TRUE);
		if (Shader::COLOR_NAME)
			mVAO->bindVertexAttrib(mMesh->colorsVBO(), material.shader()->attrib(Shader::COLOR_NAME), 3, GL_FLOAT);
		if (Shader::TEXCOORDS_NAME)
			mVAO->bindVertexAttrib(mMesh->texCoordsVBO(), material.shader()->attrib(Shader::TEXCOORDS_NAME), 2, GL_FLOAT);
		if (Shader::TANGENT_NAME)
			mVAO->bindVertexAttrib(mMesh->tangentsVBO(), material.shader()->attrib(Shader::TANGENT_NAME), 3, GL_FLOAT);
		if (Shader::BITANGENT_NAME)
			mVAO->bindVertexAttrib(mMesh->bitangentsVBO(), material.shader()->attrib(Shader::BITANGENT_NAME), 3, GL_FLOAT);
	}

	void addChild(const std::shared_ptr<Object>& child) {
		mChildren.push_back(child);
	}

	void removeChild(const std::shared_ptr<Object>& child) {
                for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
                    if (*it == child) {
                        mChildren.erase(it);
                        return;
                    }
                }
	}

	virtual std::shared_ptr<Object> clone() const
	{
		auto obj = std::make_shared<Object>();
		obj->transform = transform;
		obj->material = material;
		obj->mMesh = mMesh;

		obj->mChildren.reserve(mChildren.size());
		for (unsigned int i = 0; i < mChildren.size(); i++) {
			obj->addChild(mChildren[i]->clone());
		}

		return obj;
	}

protected:
	inline void renderChildren(const glm::mat4& parentTransform) {
		const glm::mat4 mat = parentTransform * transform.matrix();
		for (unsigned int i = 0; i < mChildren.size(); i++) {
			mChildren[i]->render(mat);
		}
	}

private:
	std::shared_ptr<Mesh> mMesh;
	std::unique_ptr<VertexArrayObject> mVAO;
	std::vector< std::shared_ptr<Object> > mChildren;
};

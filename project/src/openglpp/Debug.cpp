#pragma once

#include <openglpp/Debug.h>
#include <openglpp/Mesh.h>
#include <openglpp/VertexArrayObject.h>
#include <openglpp/Shader.h>

namespace Debug
{

struct DebugObject {
	glm::vec3 color;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<VertexArrayObject> vao;
};

bool gInitialized = false;
std::shared_ptr<Shader> gShader;
std::vector<DebugObject> gMeshes;

void init()
{
	gShader = Shader::fromFile("../shaders/debug_vs.glsl", "../shaders/debug_fs.glsl");
	gInitialized = true;
}

void drawLineLoop(const std::vector<glm::vec3>& pts, glm::vec3 color)
{
	if (!gInitialized)
		init();

	auto mesh = std::make_shared<Mesh>();
	mesh->setVertices(pts, GL_LINE_LOOP);

	auto vao = std::make_shared<VertexArrayObject>();
	vao->bindVertexAttrib(mesh->verticesVBO(), gShader->attrib("in_Position"), 3, GL_FLOAT);

	DebugObject obj;
	obj.color = color;
	obj.mesh = mesh;
	obj.vao = vao;

	gMeshes.push_back(obj);
}

void draw(const Camera& camera)
{
	if (!gInitialized)
		init();

	gShader->use();
	gShader->setUniform("projectionMatrix", camera.projection());
	gShader->setUniform("viewMatrix", camera.world());
	gShader->setUniform("modelMatrix", glm::mat4(1.0f));
	glDisable(GL_DEPTH_TEST);

	for (auto it = gMeshes.begin(); it != gMeshes.end(); it++) {
		it->vao->bind();
		gShader->setUniform("debugDrawColor", it->color);
		glDrawArrays(it->mesh->primitiveType(), 0, it->mesh->numPrimitives());
	}

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(0);
}

void clear()
{

}

}
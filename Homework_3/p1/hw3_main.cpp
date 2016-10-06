#include <iostream>
#include "openglpp/Window.h"
#include "openglpp/Shader.h"

#include "openglpp/VertexArrayObject.h"
#include "openglpp/VertexBufferObject.h"

#include "openglpp/Camera.h"

#include "openglpp/generators/SphereGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "openglpp/Object.h"

std::shared_ptr<Mesh> buildSphereMesh()
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	generateSphere(10, 10.0f, verts, normals);

	for (unsigned int i = 0; i < verts.size(); i++) {
		//std::cout << verts[i].x << ", " << verts[i].y << ", " << verts[i].z << "\n";
	}

	std::vector<glm::vec3> colors;
	colors.resize(verts.size(), glm::vec3(1.0f, 0.0f, 1.0f));

	auto sphereMesh = std::make_shared<Mesh>();
	sphereMesh->setVertices(verts, GL_TRIANGLE_STRIP);
	sphereMesh->setNormals(normals);
	sphereMesh->setColors(colors);

	return sphereMesh;
}

std::shared_ptr<Mesh> buildTriMesh() {
	std::vector<glm::vec3> verts = {
		{ -1, 0, 0 },
		{ 1, 0, 0 },
		{ 0, 1, 0 },
	};

	std::vector<glm::vec3> colors = {
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 },
	};

	auto triMesh = std::make_shared<Mesh>();
	triMesh->setVertices(verts, GL_TRIANGLES);
	triMesh->setColors(colors);
	return triMesh;
}

int main()
{
	try {
		Window window(800, 600, "Homework 3");

		std::shared_ptr<Shader> shader = Shader::fromFile("../shaders/vs.glsl", "../shaders/fs.glsl");
		Shader::setDefaultShader(shader);
		shader->use();

		Camera cam(1.57f, 800.0f / 600.0f);
		cam.lookAt(glm::vec3(0, 0, -15), glm::vec3(0, 0, 0));

		shader->setUniform("projectionMatrix", cam.projection());
		shader->setUniform("viewMatrix", cam.world());

		Object obj;
		obj.setMesh(buildSphereMesh());
		//obj.transform.setPosition(glm::vec3(0, 0, 0));

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		while (window.isOpen()) {
			// render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			obj.Render();

			window.swapBuffers();
			window.pollEvents();
		}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		while (true);
	}

	return 0;
}
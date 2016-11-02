#include <iostream>
#include "openglpp/Window.h"
#include "openglpp/Shader.h"

#include "openglpp/Camera.h"

#include "openglpp/generators/SphereGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "openglpp/Object.h"

std::shared_ptr<Mesh> buildQuadMesh()
{
	std::vector<glm::vec3> verts = {
	glm::vec3(-1, -1, 0),
	glm::vec3(+1, -1, 0),
	glm::vec3(-1, +1, 0),

	glm::vec3(-1, +1, 0),
	glm::vec3(+1, -1, 0),
	glm::vec3(+1, +1, 0),
	};

	std::vector<glm::vec2> texCoords = {
	  glm::vec2(0, 0),
	  glm::vec2(1, 0),
	  glm::vec2(0, 1),

	  glm::vec2(0, 1),
	  glm::vec2(1, 0),
	  glm::vec2(1, 1),
	};

	std::vector<glm::vec3> normals;
	normals.resize(verts.size(), glm::vec3(0, 0, 1));

	auto mesh = std::make_shared<Mesh>();
	mesh->setVertices(verts, GL_TRIANGLE_STRIP);
	mesh->setTexCoords(texCoords);
	mesh->setNormals(normals);

	return mesh;
}

int main()
{
	try {
		Window window(800, 600, "Homework 4");

		std::shared_ptr<Shader> shader = Shader::fromFile("../shaders/hw4_shader_vs.glsl", "../shaders/hw4_shader_fs.glsl");
		Shader::POSITION_NAME = "in_Position";
		Shader::NORMAL_NAME = NULL;
		Shader::COLOR_NAME = NULL;
		Shader::TEXCOORDS_NAME = "in_TexCoords";

		Shader::setDefaultShader(shader);
		shader->use();

		Camera cam(1.0f, 800.0f / 600.0f);
		cam.lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0));

		shader->setUniform("projectionMatrix", cam.projection());
		shader->setUniform("viewMatrix", cam.world());
		
		Object quad;

		const auto quadMesh = buildQuadMesh();
		quad.setMesh(quadMesh);
		
		quad.material.set("texture", 0);  // bind texture to sampler 0
		auto tex = std::make_shared<Texture>();
		tex->fromFile("../test.bmp");
		quad.material.setTexture(0, tex);

		quad.transform.setPosition(glm::vec3(0, 0, 0));

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		while (window.isOpen()) {
			// render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			quad.render();

			window.swapBuffers();
			window.pollEvents();
		}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		while (true);
	}

	return 0;
}
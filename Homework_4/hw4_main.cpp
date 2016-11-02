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

		const auto quadMesh = buildQuadMesh();
		Object p1;

		{
			p1.setMesh(quadMesh);

			p1.material.set("opacity", 1.0f);

			p1.material.set("texture", 0);  // bind texture to sampler 0
			p1.material.setTexture(0, Texture::fromFile("../test.bmp"));

			p1.transform.setPosition(glm::vec3(-2.333f, 1.5f, 0));
		}

		Object p2[3];
		for (int i = 0; i < 3; i++) {
			p2[i].setMesh(quadMesh);
			p2[i].material.set("texture", 0);
		}

		p2[0].material.setTexture(0, Texture::fromFile("../p2_gradient.bmp"));
		p2[0].material.set("opacity", 1.0f);
		p2[1].material.setTexture(0, Texture::fromFile("../p2_landscape.bmp"));
		p2[1].material.set("opacity", 0.3f);
		p2[2].material.setTexture(0, Texture::fromFile("../p2_person.bmp"));
		p2[2].material.set("opacity", 0.3f);

		// glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glBlendFunc(s, d);
		/* GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA. GL_CONSTANT_COLOR,
		GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA,
		GL_ONE_MINUS_CONSTANT_ALPHA */
		//glBlendEquation(GL_FUNC_ADD);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		while (window.isOpen()) {
			// render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			p1.render();

			for (int i = 0; i < 3; i++)
				p2[i].render();

			window.swapBuffers();
			window.pollEvents();
		}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		while (true);
	}

	return 0;
}
#include <iostream>
#include "openglpp/Window.h"
#include "openglpp/Shader.h"

#include "openglpp/VertexArrayObject.h"
#include "openglpp/VertexBufferObject.h"

#include "openglpp/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main()
{
	try {
		Window window(800, 600, "Homework 3");

		Shader shader = Shader::fromFile("../shaders/vs.glsl", "../shaders/fs.glsl");
		shader.use();

		Camera cam(1.57f, 800.0f / 600.0f);
		cam.lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0));

		shader.setUniform("projectionMatrix", cam.projection());
		shader.setUniform("viewMatrix", cam.world());

		glm::mat4 model = glm::mat4(1.0f);
		shader.setUniform("modelMatrix", model);

		glm::vec3 verts[3] = {
			{ -10, 0, 0 },
			{ 10, 0, 0 },
			{ 0, 10, 0 },
		};
		auto vboVerts = std::make_shared<VertexBufferObject>();
		vboVerts->bufferData(3, verts, GL_STATIC_DRAW);

		glm::vec3 colors[3] = {
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, 0, 1 },
		};
		auto vboColors = std::make_shared<VertexBufferObject>();
		vboColors->bufferData(3, colors, GL_STATIC_DRAW);

		VertexArrayObject vao;
		vao.bind();
		vao.bindVertexAttrib(vboVerts, shader.attrib("in_Position"), 3, GL_FLOAT);
		vao.bindVertexAttrib(vboColors, shader.attrib("in_Color"), 3, GL_FLOAT);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		while (window.isOpen()) {
			// render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			vao.bind();
			glDrawArrays(GL_TRIANGLES, 0, 3);

			window.swapBuffers();
			window.pollEvents();
		}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		while (true);
	}

	return 0;
}
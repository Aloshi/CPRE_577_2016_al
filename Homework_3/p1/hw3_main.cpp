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

std::shared_ptr<Mesh> buildSphereMesh(int n, float radius)
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	generateSphere(n, radius, verts, normals);

	std::vector<glm::vec3> colors;
	colors.resize(verts.size(), glm::vec3(1.0f, 0.0f, 1.0f));

	auto sphereMesh = std::make_shared<Mesh>();
	sphereMesh->setVertices(verts, GL_TRIANGLE_STRIP);
	sphereMesh->setNormals(normals);
	sphereMesh->setColors(colors);

	return sphereMesh;
}

int main()
{
	try {
		Window window(800, 600, "Homework 3");

		std::shared_ptr<Shader> shader = Shader::fromFile("../shaders/vs.glsl", "../shaders/fs.glsl");
		Shader::setDefaultShader(shader);
		shader->use();

		Camera cam(1.0f, 800.0f / 600.0f);
		cam.lookAt(glm::vec3(0, 0, 25), glm::vec3(0, 0, 0));

		shader->setUniform("projectionMatrix", cam.projection());
		shader->setUniform("viewMatrix", cam.world());

		static const int nSpheres = 4;
		Object spheres[nSpheres];
		const auto sphereMesh = buildSphereMesh(400, 3.0f);
		for (int i = 0; i < nSpheres; i++)
		{
			spheres[i].setMesh(sphereMesh);

			glm::vec3 pos((i / 3.0f - 0.5f) * 25.0f, 0, 0);
			spheres[i].transform.setPosition(pos);

			spheres[i].material.set("numLights", 1);
			spheres[i].material.set("lights[0].diffuse_intensity", 0.5f);
			spheres[i].material.set("lights[0].ambient_intensity", 0.0f);
			spheres[i].material.set("lights[0].specular_intensity", 2.0f);
			spheres[i].material.set("lights[0].attenuation_coefficient", 0.02f);

			spheres[i].material.set("materials[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
			spheres[i].material.set("materials[0].transparency", 1.0f);
		}

		// red sphere
		spheres[0].material.set("lights[0].position", glm::vec4(3.0f, 0.1f, 2.5f, 0.0f));  // will get normalized
		spheres[0].material.set("materials[0].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
		spheres[0].material.set("materials[0].ambient", glm::vec3(1.0f, 0.0f, 0.0f));
		spheres[0].material.set("materials[0].shininess", 170.0f);

		// blue sphere
		spheres[1].material.set("lights[0].diffuse_intensity", 0.55f);
		spheres[1].material.set("lights[0].position", glm::vec4(2.0f, 0.1f, 2.5f, 0.0f));
		spheres[1].material.set("materials[0].diffuse", glm::vec3(0.0f, 0.0f, 1.0f));
		spheres[1].material.set("materials[0].ambient", glm::vec3(0.0f, 0.0f, 1.0f));
		spheres[1].material.set("materials[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		spheres[1].material.set("materials[0].shininess", 30.0f);

		// green sphere
		spheres[2].material.set("numLights", 2);
		
		spheres[2].material.set("lights[0].position", glm::vec4(1.0f, 0.1f, 2.5f, 0.0f));
		spheres[2].material.set("lights[0].diffuse_intensity", 0.0f);

		spheres[2].material.set("lights[1].position", glm::vec4(4.0f, 2.0f, 12, 1.0f));  // spotlight
		spheres[2].material.set("lights[1].diffuse_intensity", 400.0f);
		spheres[2].material.set("lights[1].specular_intensity", 0.0f);
		spheres[2].material.set("lights[1].attenuation_coefficient", 0.0f);
		spheres[2].material.set("lights[1].cone_angle", 15.0f);
		spheres[2].material.set("lights[1].cone_direction", glm::vec3(0, 0, -1));

		spheres[2].material.set("materials[0].diffuse", glm::vec3(0.0f, 1.0f, 0.0f));
		spheres[2].material.set("materials[0].ambient", glm::vec3(0.0f, 1.0f, 0.0f));
		spheres[2].material.set("materials[0].shininess", 100.0f);

		// gold sphere
		spheres[3].material.set("numLights", 3);
		spheres[3].material.set("lights[0].specular_intensity", 0.0f);
		spheres[3].material.set("lights[0].position", glm::vec4(-5.5f, 2, 6, 0.0f));

		spheres[3].material.set("lights[1].diffuse_intensity", 2.5f);
		spheres[3].material.set("lights[1].specular_intensity", 0.0f);
		spheres[3].material.set("lights[1].position", glm::vec4(10.0f, 6, 6, 1));
		spheres[3].material.set("lights[1].attenuation_coefficient", 0.04f);
		spheres[3].material.set("lights[1].cone_angle", 44.0f);
		spheres[3].material.set("lights[1].cone_direction", glm::vec3(0.0f, -1, -0.35f));

		spheres[3].material.set("lights[2].specular_intensity", 0.2f);
		spheres[3].material.set("lights[2].position", glm::vec4(-3, -1, 3, 0.0f));

		spheres[3].material.set("materials[0].diffuse", glm::vec3(0.7f, 0.7f, 0.3f));
		spheres[3].material.set("materials[0].ambient", glm::vec3(0.7f, 0.7f, 0.3f));
		spheres[3].material.set("materials[0].specular", glm::vec3(0.7f, 0.7f, 0.3f));
		spheres[3].material.set("materials[0].shininess", 160.0f);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		while (window.isOpen()) {
			// render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (int i = 0; i < nSpheres; i++)
				spheres[i].Render();
			
			window.swapBuffers();
			window.pollEvents();
		}
	} catch (std::exception& e) {
		std::cout << e.what() << "\n";
		while (true);
	}

	return 0;
}
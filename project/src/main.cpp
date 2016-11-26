#include <iostream>
#include <queue>

#include "openglpp/Window.h"
#include "openglpp/Shader.h"

#include "openglpp/Camera.h"

#include "openglpp/generators/SphereGenerator.h"
#include "openglpp/generators/PlaneGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "openglpp/Object.h"

#include "CameraController.h"
#include "Event.h"

#include <GLFW/glfw3.h>

#include "RoadMesh.h"

#include <openglpp/LoadObj.h>
#include <openglpp/Debug.h>

std::queue<Event> sEventQueue;

void process_key(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int modifiers)
{
	Event e;
	e.type = action == GLFW_PRESS ? Event::KEY_PRESSED : Event::KEY_RELEASED;
	e.key = key;
	e.modifiers = modifiers;
	sEventQueue.push(e);
}

void process_mouse_button(GLFWwindow* window, int button, int action, int modifiers)
{
	Event e;
	e.type = action == GLFW_PRESS ? Event::MOUSE_PRESSED : Event::MOUSE_RELEASED;
	e.mouseButton = button;
	e.modifiers = modifiers;
	sEventQueue.push(e);
}

void process_mouse_move(GLFWwindow* window, double x, double y)
{
	static double lastX, lastY;

	Event e;
	e.type = Event::MOUSE_MOVED;
	e.mousePos = glm::vec2((float)x, (float)y);
	e.mouseDelta = glm::vec2((float)(x - lastX), (float)(y - lastY));
	lastX = x;
	lastY = y;
	sEventQueue.push(e);
}

bool pollEvent(Event& out) {
	if (sEventQueue.empty())
		return false;
	out = sEventQueue.front();
	sEventQueue.pop();
	return true;
}

int main()
{
	Window window(800 * 2, 600 * 2, "trafficsim", true);

	glfwSetKeyCallback(window, process_key);
	glfwSetMouseButtonCallback(window, process_mouse_button);
	glfwSetCursorPosCallback(window, process_mouse_move);

	auto shader = Shader::fromFile("../shaders/vs.glsl", "../shaders/fs.glsl");
	Shader::POSITION_NAME = "in_Position";
	Shader::NORMAL_NAME = "in_Normal";
	Shader::TEXCOORDS_NAME = "in_TexCoords";
	Shader::TANGENT_NAME = "in_Tangent";
	Shader::BITANGENT_NAME = "in_Bitangent";

	Shader::MAT_DIFFUSE_TYPE = "material.diffuseType";
	Shader::MAT_DIFFUSE_TEXTURE = "material.diffuseTexture";
	Shader::MAT_DIFFUSE_COLOR = "material.diffuseColor";

	Shader::MAT_USE_NORMAL_MAP = "material.useNormalMap";
	Shader::MAT_NORMAL_TEXTURE = "material.normalMapTexture";

	Shader::MAT_SPECULAR_TYPE = "material.specularType";
	Shader::MAT_SPECULAR_TEXTURE = "material.specularTexture";
	Shader::MAT_SPECULAR_COLOR = "material.specularColor";

	Shader::MAT_SHININESS = "material.shininess";

	Shader::setDefaultShader(shader);
	shader->use();

	shader->setUniform("lights[0].position", glm::vec4(0, 15, 0, 0));
	//shader->setUniform("lights[0].diffuse_intensity", 1.0f);
	//shader->setUniform("lights[0].ambient_intensity", 0.005f);
	//shader->setUniform("lights[0].specular_intensity", 0.0f);  // 0 for no effect
	//shader->setUniform("lights[0].attenuation_coefficient", 0.0f);  // 0 for no effect
	// shader->setUniform("lights[0].cone_angle", 0);  // w = 1.0
	// shader->setUniform("lights[0].cone_direction", 0);  // w = 1.0

	//shader->setUniform("materials[0].diffuse", glm::vec3(1, 1, 1));
	//shader->setUniform("materials[0].ambient", glm::vec3(1, 1, 1));
	//shader->setUniform("materials[0].specular", glm::vec3(1, 1, 1));
	//shader->setUniform("materials[0].shininess", 1.0f);
	//shader->setUniform("materials[0].transparency", 1.0f);

	std::shared_ptr<Camera> cam = std::make_shared<Camera>(1.0f, 800.0f / 600.0f);
	cam->lookAt(glm::vec3(0, 0, 15), glm::vec3(0, 0, 0));
	shader->setUniform("projectionMatrix", cam->projection());

	CameraController camControl(cam);

	Road test;
	test.vertices = {
		RoadVertex{ glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(4, 0, -3), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(8, 0, -5), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(12, 0, -6), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(16, 0, -5), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(20, 0, -3), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(24, 0, 0), glm::vec3(0, 1, 0) },
	};

	/*auto markerMesh = generateSphere(10, 0.5f);
	std::vector<Object> roadVertMarkers(test.vertices.size());
	for (unsigned int i = 0; i < test.vertices.size(); i++) {
		roadVertMarkers[i].setMesh(markerMesh);
		roadVertMarkers[i].transform.setPosition(test.vertices[i].pos);
	}*/


	std::shared_ptr<Object> roadObj = test.generateObject();
	//ground.setMesh(generateSphere(40, 5.0f));
	//ground.setMesh(test.generateMesh());
	//ground.transform.setPosition(glm::vec3(0, 0, 0));

	

	std::shared_ptr<Object> intersection = loadObj("../models/intersection_4way.obj");

	{
		CatmullRom<RoadVertex> spline;
		spline.set_control_points(test.vertices, true);
		unsigned int n_slices = (int)ceil(spline.total_length() / 1.0f);
		std::vector<glm::vec3> debugPts;
		for (unsigned int i = 0; i <= n_slices; i++) {
			float d = spline.total_length() * ((float)i) / n_slices;
			RoadVertex c0, c1;
			spline.evaluate(d, &c0, &c1);
			debugPts.push_back(c0.pos);
		}
		Debug::drawPath(debugPts, glm::vec3(1, 1, 1));
		/*Debug::drawLineLoop({ {
				glm::vec3(0, 0, 0),
				glm::vec3(5, 0, 0),
				glm::vec3(5, 5, 0),
			glm::vec3(-5, 6, 0)
			} });*/
	}

	int useTexNormal = 1;

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	double lastTime = glfwGetTime();
	while (window.isOpen()) {
		Event e;
		while (pollEvent(e)) {
			switch (e.type) {
			case Event::KEY_PRESSED:
			case Event::KEY_RELEASED:
				camControl.updateMoveDir(window);
				/*if (e.key == GLFW_KEY_N && e.type == Event::KEY_PRESSED) {
					ground.material.set(Shader::MAT_USE_NORMAL_MAP, (useTexNormal = !useTexNormal));
					std::cout << "useTexNormal = " << useTexNormal << "\n";
				}*/
				break;
			case Event::MOUSE_MOVED:
				camControl.onMouseMoved(e.mouseDelta.x, e.mouseDelta.y);
				break;
			case Event::MOUSE_PRESSED:
			case Event::MOUSE_RELEASED:
				camControl.onMouseButton(window, e.mouseButton, e.type == Event::MOUSE_PRESSED, e.modifiers);
			}
		}

		const double now = glfwGetTime();
		const float dt = (float)(now - lastTime);
		lastTime = now;

		// update camera
		camControl.update(dt);

		Shader::defaultShader()->use();
		shader->setUniform("viewMatrix", cam->world());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		roadObj->render();
		intersection->render();

		Debug::draw(*cam);

		window.swapBuffers();
		window.pollEvents();
	}

	return 0;
}

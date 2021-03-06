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

#include "RoadGraph.h"
#include "TrafficController.h"
#include "FollowCameraController.h"

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
	glm::ivec2 resolution(1280, 720);
	Window window(resolution.x, resolution.y, "trafficsim", false);

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

	std::shared_ptr<Camera> cam = std::make_shared<Camera>(1.0f, resolution.x / (float)resolution.y);
	cam->lookAt(glm::vec3(0, 0, 15), glm::vec3(0, 0, 0));
	shader->setUniform("projectionMatrix", cam->projection());


	// Intersection Edge Order (X+ = right, Z+ = down)
	//     3
	//  0 --- 1
	//     2

	// hard-coded road/intersection vertex setup
	auto test1 = std::make_shared<Road>();
	test1->vertices = {
		RoadVertex{ glm::vec3(-10, 0, -13), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(-5, 0, -12), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(0, 0, -10), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(5, 0, -5), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(10, 0, 0), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(15, 0, 0), glm::vec3(0, 1, 0) },
	};

	auto test2 = std::make_shared<Road>();
	test2->vertices = {
		RoadVertex{ glm::vec3(25, 0, 0), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(30, 0, 0), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(35, 0, 5), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(40, 0, 10), glm::vec3(0, 1, 0) },
	};

	auto test3 = std::make_shared<Intersection>();
	test3->edges.push_back(IntersectionEdge{ glm::vec3(15, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0) });
	test3->edges.push_back(IntersectionEdge{ glm::vec3(25, 0, 0), glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0) });
	test3->edges.push_back(IntersectionEdge{ glm::vec3(20, 0, 5), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1) });
	test3->edges.push_back(IntersectionEdge{ glm::vec3(20, 0, -5), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) });

	auto test4 = std::make_shared<Road>();
	test4->vertices = {
		RoadVertex{ glm::vec3(20, 0, 5), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(20, 0, 10), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(20, 0, 15), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(20, 0, 20), glm::vec3(0, 1, 0) },
	};


	auto test5 = std::make_shared<Intersection>();
	test5->edges.push_back(IntersectionEdge{ glm::vec3(15, 0, 25), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0) });
	test5->edges.push_back(IntersectionEdge{ glm::vec3(25, 0, 25), glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0) });
	test5->edges.push_back(IntersectionEdge{ glm::vec3(20, 0, 30), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1) });
	test5->edges.push_back(IntersectionEdge{ glm::vec3(20, 0, 20), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) });

	auto test6 = std::make_shared<Road>();
	test6->vertices = {
		RoadVertex{ glm::vec3(15, 0, 25), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(10, 0, 24.9f), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(0,  0, 23), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(-10, 0, 10), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(-15, 0, 0), glm::vec3(0, 1, 0) },
		RoadVertex{ glm::vec3(-15, 0, -8), glm::vec3(0, 1, 0) },
	};

	auto test7 = std::make_shared<Intersection>();
	test7->edges.push_back(IntersectionEdge{ glm::vec3(-20, 0, -13), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0) });
	test7->edges.push_back(IntersectionEdge{ glm::vec3(-10, 0, -13), glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0) });
	test7->edges.push_back(IntersectionEdge{ glm::vec3(-15, 0, -8), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1) });
	test7->edges.push_back(IntersectionEdge{ glm::vec3(-15, 0, -18), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) });

	connect(test1, 1, test3, 0);
	connect(test2, 0, test3, 1);
	connect(test4, 0, test3, 2);
	connect(test4, 1, test5, 3);
	connect(test6, 0, test5, 0);
	connect(test6, 1, test7, 2);
	connect(test1, 0, test7, 1);


	std::vector<std::shared_ptr<Road> > roads = { test1, test2, test4, test6 };
	std::vector<std::shared_ptr<Intersection> > intersections = { test3, test5, test7 };

	std::vector< std::shared_ptr<Object> > objects;
	for (auto r : roads)
		objects.push_back(r->generateObject());
	for (auto i : intersections)
		objects.push_back(i->generateObject());

	RoadGraph graph = RoadGraph::build(roads, intersections);
	int visualize_type = RoadGraph::VISUALIZE_NONE;

	TrafficController tc;
	tc.setGraph(graph);
	tc.setTargetPopulation(5);

	auto freeCam = std::make_shared<CameraController>(cam);
	auto followCam = std::make_shared<FollowCameraController>(cam, &tc);
	std::shared_ptr<CameraController> camControl;
	camControl = freeCam;

	int allowTextureMaps = 1;
	bool useFollowCamera = false;

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	double lastTime = glfwGetTime();
	while (window.isOpen()) {
		Event e;
		while (pollEvent(e)) {
			switch (e.type) {
			case Event::KEY_PRESSED:
			case Event::KEY_RELEASED:
				camControl->updateMoveDir(window);

				// toggle follow camera
				if (e.key == GLFW_KEY_F && e.type == Event::KEY_PRESSED) {
					useFollowCamera = !useFollowCamera;
					camControl = useFollowCamera ? followCam : freeCam;
					cam->setPosition(glm::vec3(0, 0, -15));
				}

				// toggle normal + specular maps
				if (e.key == GLFW_KEY_N && e.type == Event::KEY_PRESSED) {
					allowTextureMaps = !allowTextureMaps;
					shader->use();
					shader->setUniform("allowTextureMaps", allowTextureMaps);
				}

				// cycle debug visualizations
				if (e.key == GLFW_KEY_G && e.type == Event::KEY_PRESSED) {
					visualize_type = (visualize_type + 1) % RoadGraph::VISUALIZE_COUNT;
					graph.visualize((RoadGraph::VisualizeMode) visualize_type);
				}

				break;
			case Event::MOUSE_MOVED:
				camControl->onMouseMoved(e.mouseDelta.x, e.mouseDelta.y);
				break;
			case Event::MOUSE_PRESSED:
			case Event::MOUSE_RELEASED:
				camControl->onMouseButton(window, e.mouseButton, e.type == Event::MOUSE_PRESSED, e.modifiers);
			}
		}

		const double now = glfwGetTime();
		const float dt = (float)(now - lastTime);
		lastTime = now;

		// update
		camControl->update(dt);
		tc.update(dt);

		Shader::defaultShader()->use();
		shader->setUniform("viewMatrix", cam->view());
		//shader->setUniform("lights[0].position", glm::vec4(0, 10 + sin(now) * 10.0f, 0, 0));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned int i = 0; i < objects.size(); i++) {
			objects[i]->render();
		}
		tc.render();

		Debug::draw(*cam);

		window.swapBuffers();
		window.pollEvents();
	}

	return 0;
}

#include <iostream>
#include "openglpp/Window.h"
#include "openglpp/Shader.h"

#include "openglpp/Camera.h"

#include "openglpp/generators/SphereGenerator.h"
#include "openglpp/generators/PlaneGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

#include "openglpp/Object.h"

std::shared_ptr<Mesh> createPlane(float size)
{
  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> normals;
  //generatePlane(size, glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), verts, normals);
  generateSphere(40, size, verts, normals);

  auto mesh = std::make_shared<Mesh>();
  mesh->setVertices(verts, GL_TRIANGLE_STRIP);
  mesh->setNormals(normals);

  return mesh;
}

int main()
{
  
  Window window(800, 600, "trafficsim");

  auto shader = Shader::fromFile("../shaders/vs.glsl", "../shaders/fs.glsl");
  Shader::POSITION_NAME = "in_Position";
  Shader::NORMAL_NAME = "in_Normal";
  Shader::setDefaultShader(shader);
  shader->use();

  shader->setUniform("lights[0].position", glm::vec4(0, 15, -7, 0));
  shader->setUniform("lights[0].diffuse_intensity", 1.0f);
  shader->setUniform("lights[0].ambient_intensity", 0.0f);
  shader->setUniform("lights[0].specular_intensity", 0.0f);  // 0 for no effect
  shader->setUniform("lights[0].attenuation_coefficient", 0.0f);  // 0 for no effect
  // shader->setUniform("lights[0].cone_angle", 0);  // w = 1.0
  // shader->setUniform("lights[0].cone_direction", 0);  // w = 1.0

  shader->setUniform("materials[0].diffuse", glm::vec3(1, 1, 1));
  shader->setUniform("materials[0].ambient", glm::vec3(1, 1, 1));
  shader->setUniform("materials[0].specular", glm::vec3(1, 1, 1));
  shader->setUniform("materials[0].shininess", 1.0f);
  shader->setUniform("materials[0].transparency", 1.0f);

  Camera cam(1.0f, 800.0f / 600.0f);
  cam.lookAt(glm::vec3(0, 0, 15), glm::vec3(0, 0, 0));

  shader->setUniform("projectionMatrix", cam.projection());
  shader->setUniform("viewMatrix", cam.world());

  Object ground;
  ground.setMesh(createPlane(2.0f));
  ground.transform.setPosition(glm::vec3(0, 0, 0));

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  while (window.isOpen()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ground.render();

    window.swapBuffers();
    window.pollEvents();
  }

  return 0;
}

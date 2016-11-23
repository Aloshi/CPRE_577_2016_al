#include <openglpp/generators/PlaneGenerator.h>

std::shared_ptr<Mesh> generatePlane(float size, const glm::vec3& normal, const glm::vec3& right)
{
  const glm::vec3 up = glm::cross(normal, right);

  std::vector<glm::vec3> pts(6);
  pts[0] = -right * size - up * (size / 2.0f);
  pts[1] = right * size - up * (size / 2.0f);
  pts[2] = -right * size + up * (size / 2.0f);

  pts[3] = -right * size + up * (size / 2.0f);
  pts[4] = right * size - up * (size / 2.0f);
  pts[5] = right * size + up * (size / 2.0f);

  // normals are all the same
  std::vector<glm::vec3> normals;
  normals.resize(pts.size(), normal);

  auto mesh = std::make_shared<Mesh>();
  mesh->setVertices(pts, GL_TRIANGLES);
  mesh->setNormals(normals);

  return mesh;
}

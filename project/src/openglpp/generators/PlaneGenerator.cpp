#include <openglpp/generators/PlaneGenerator.h>

void generatePlane(float size, const glm::vec3& normal, const glm::vec3& right, std::vector<glm::vec3>& pts_out, std::vector<glm::vec3>& normals_out)
{
  const glm::vec3 up = glm::cross(normal, right);

  pts_out.resize(3);
  pts_out[0] = -right * size - up * (size / 2.0f);
  pts_out[1] = right * size - up * (size / 2.0f);
  pts_out[2] = -right * size + up * (size / 2.0f);
  //pts_out[2] = right * size + up * (size / 2.0f);

  // normals are all the same
  normals_out.resize(3, normal);
}

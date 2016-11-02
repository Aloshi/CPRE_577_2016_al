#include "SphereGenerator.h"

#include <math.h>  // for sin/cos
#include <iostream>  // debug

void generateSphere(int n, float radius, std::vector<glm::vec3>& pts_out, std::vector<glm::vec3>& normals_out)
{
	const int segments = n;
	const int rows = n;
	const float r = radius;
	const glm::vec3 center(0.0f, 0.0f, 0.0f);

	const int n_points = (((segments * 2) + 1) * 6 * rows) / 3;
	pts_out.resize(n_points);
	normals_out.resize(n_points);

	int current_size = 0;
	const float PI = 3.141592653589793238462643383279502884197f;

	for (int i = 0; i < rows; i++)
	//for (float theta = 0.; theta < PI; theta += PI / float(rows)) // Elevation [0, PI]
	{
		const float theta = i * (PI / (float)rows);
		const float theta2 = theta + PI / float(rows);

		int count = 0;
		int count_row = 0;

		// Iterate through phi, theta then convert r,theta,phi to  XYZ
		for (int j = 0; j < segments; j++)
		//for (float phi = 0.; phi < 2 * PI + PI / float(segments); phi += PI / float(segments)) // Azimuth [0, 2PI]
		{
			const float phi = j * (PI / (float) segments * 2);
			int index = current_size + count;

			pts_out[index].x = r * cos(phi) * sin(theta) + center[0];
			pts_out[index].y = r * sin(phi) * sin(theta) + center[1];
			pts_out[index].z = r            * cos(theta) + center[2];
			count++;

			pts_out[index + 1].x = r * cos(phi) * sin(theta2) + center[0];
			pts_out[index + 1].y = r * sin(phi) * sin(theta2) + center[1];
			pts_out[index + 1].z = r            * cos(theta2) + center[2];
			count++;

			normals_out[index].x = cos(phi) * sin(theta);
			normals_out[index].y = sin(phi) * sin(theta);
			normals_out[index].z = cos(theta);

			normals_out[index + 1].x = cos(phi) * sin(theta2);
			normals_out[index + 1].y = sin(phi) * sin(theta2);
			normals_out[index + 1].z = cos(theta2);
		}

		if (count_row == 0)
			count_row = count;

		current_size += count;
	}
}
#include <openglpp/LoadObj.h>
#include <openglpp/tiny_obj_loader.h>

#include <iostream>

static std::shared_ptr<Texture> texFromObjMat(const std::string& texname, const tinyobj::texture_option_t& opt)
{
	// TODO intelligently reuse these with a map of weak_ptrs
	auto tex = Texture::fromFile(texname.c_str());
	return tex;
}

std::vector< std::shared_ptr<Object> > loadObj(const std::string& path)
{
	std::string mtl_basedir = path.substr(0, path.find_last_of('/')+1);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), mtl_basedir.c_str(), true);

	if (!err.empty())
		std::cout << err << "\n";
	if (!ret)
		throw std::runtime_error("Failed to load obj");


	// material_id -> triangle data
	std::map<int, std::vector<glm::vec3> > vertices;
	std::map<int, std::vector<glm::vec3> > normals;
	std::map<int, std::vector<glm::vec2> > texCoords;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			unsigned int fv = shapes[s].mesh.num_face_vertices[f];
			int material_id = shapes[s].mesh.material_ids[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				float vx = attrib.vertices[3 * idx.vertex_index + 0];
				float vy = attrib.vertices[3 * idx.vertex_index + 1];
				float vz = attrib.vertices[3 * idx.vertex_index + 2];

				float nx, ny, nz;
				if (idx.normal_index != -1) {
					nx = attrib.normals[3 * idx.normal_index + 0];
					ny = attrib.normals[3 * idx.normal_index + 1];
					nz = attrib.normals[3 * idx.normal_index + 2];
				} else {
					nx = ny = nz = 0.0f;
				}

				float tx, ty;
				if (idx.texcoord_index != -1) {
					tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				} else {
					tx = ty = 0.0f;
				}

				vertices[material_id].push_back(glm::vec3(vx, vy, vz));
				normals[material_id].push_back(glm::vec3(nx, ny, nz));
				texCoords[material_id].push_back(glm::vec2(tx, ty));
			}
			index_offset += fv;
		}
	}

	std::vector< std::shared_ptr<Object> > objs;
	for (auto it = vertices.begin(); it != vertices.end(); it++) {
		int material_id = it->first;
		const tinyobj::material_t mat = (material_id == -1) ? tinyobj::material_t() : materials[material_id];

		const std::vector<glm::vec3>& verts = it->second;
		const std::vector<glm::vec3>& norms = normals.at(material_id);
		const std::vector<glm::vec2>& texs = texCoords.at(material_id);

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->setVertices(verts, GL_TRIANGLES);
		mesh->setNormals(norms);
		mesh->setTexCoords(texs);

		std::shared_ptr<Object> obj = std::make_shared<Object>();
		obj->setMesh(mesh);

		if (Shader::MAT_DIFFUSE_TYPE != NULL) {
			if (mat.diffuse_texname.empty()) {
				obj->material.set(Shader::MAT_DIFFUSE_TYPE, Shader::SOURCE_COLOR);
				obj->material.set(Shader::MAT_DIFFUSE_COLOR, glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));
			} else {
				obj->material.set(Shader::MAT_DIFFUSE_TYPE, Shader::SOURCE_TEXTURE);
				obj->material.set(Shader::MAT_DIFFUSE_TEXTURE, 0);
				obj->material.setTexture(0, texFromObjMat(mat.diffuse_texname, mat.diffuse_texopt));
			}
		}

		if (Shader::MAT_USE_NORMAL_MAP != NULL) {
			if (mat.normal_texname.empty()) {
				obj->material.set(Shader::MAT_USE_NORMAL_MAP, 0);
			} else {
				obj->material.set(Shader::MAT_USE_NORMAL_MAP, 1);
				obj->material.set(Shader::MAT_NORMAL_TEXTURE, 1);
				obj->material.setTexture(1, texFromObjMat(mat.normal_texname, mat.normal_texopt));
			}
		}

		if (Shader::MAT_SPECULAR_TYPE != NULL) {
			if (mat.specular_texname.empty()) {
				obj->material.set(Shader::MAT_SPECULAR_TYPE, Shader::SOURCE_COLOR);
				obj->material.set(Shader::MAT_SPECULAR_COLOR, glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]));
			} else {
				obj->material.set(Shader::MAT_SPECULAR_TYPE, Shader::SOURCE_TEXTURE);
				obj->material.set(Shader::MAT_SPECULAR_TEXTURE, 2);
				obj->material.setTexture(2, texFromObjMat(mat.specular_texname, mat.specular_texopt));
			}
		}

		if (Shader::MAT_SHININESS) {
			obj->material.set(Shader::MAT_SHININESS, mat.shininess);
		}

		objs.push_back(obj);
	}

	return objs;
}
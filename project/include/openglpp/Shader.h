#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <map>
#include <memory>  // std::shared_ptr

#include "GLException.h"

class ShaderException : public GLException {};

class Shader
{
public:
	static const char* POSITION_NAME;
	static const char* NORMAL_NAME;
	static const char* COLOR_NAME;
	static const char* TEXCOORDS_NAME;
	static const char* TANGENT_NAME;  // surface tangent vector
	static const char* BITANGENT_NAME;  // another surface tangent vector

	// material properties
	enum SourceType : int {
		SOURCE_NONE = 0,
		SOURCE_COLOR = 0,
		SOURCE_TEXTURE = 1
	};

	static const char* MAT_DIFFUSE_TYPE;
	static const char* MAT_USE_NORMAL_MAP;
	static const char* MAT_SPECULAR_TYPE;

	static const char* MAT_DIFFUSE_COLOR;
	static const char* MAT_DIFFUSE_TEXTURE;
	static const char* MAT_SPECULAR_COLOR;
	static const char* MAT_SPECULAR_TEXTURE;
	static const char* MAT_NORMAL_TEXTURE;
	static const char* MAT_SHININESS;

	static std::shared_ptr<Shader> fromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	static std::shared_ptr<Shader> fromString(const std::string& vertexShader, const std::string& fragmentShader);

	inline static void setDefaultShader(const std::shared_ptr<Shader>& shader) { sDefaultShader = shader; }
	inline static const std::shared_ptr<Shader>& defaultShader() { return sDefaultShader; }

	Shader();
	Shader(const Shader&) = delete;
	Shader(Shader&& rhs);
	virtual ~Shader();

	void use();

	GLint uniform(const char* name);
	GLint attrib(const char* name);

	void setUniform(const char* name, int val);
	void setUniform(const char* name, float val);
	void setUniform(const char* name, const glm::vec3& val);
	void setUniform(const char* name, const glm::vec4& val);
	void setUniform(const char* name, const glm::mat4x4& mat);

private:
	static std::shared_ptr<Shader> sDefaultShader;
	static GLuint compileShader(const std::string& src, GLuint shaderType);

	GLuint mProgramID;

	static const int mShaderCount = 2;
	GLuint mShaderIDs[mShaderCount];

	struct AttribData {
		GLenum type;
		GLint idx;
	};

	std::map<std::string, AttribData> mUniforms;
	std::map<std::string, AttribData> mAttributes;
};
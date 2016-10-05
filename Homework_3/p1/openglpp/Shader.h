#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <map>

#include "GLException.h"

class ShaderException : public GLException {};

class Shader
{
public:
	static Shader fromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	static Shader fromString(const std::string& vertexShader, const std::string& fragmentShader);

	Shader();
	Shader(const Shader&) = delete;
	Shader(Shader&& rhs);
	virtual ~Shader();

	void use();

	GLint uniform(const char* name);
	GLint attrib(const char* name);

	void setUniform(const char* name, float val);
	void setUniform(const char* name, const glm::vec3& val);
	void setUniform(const char* name, const glm::vec4& val);
	void setUniform(const char* name, const glm::mat4x4& mat);

private:
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
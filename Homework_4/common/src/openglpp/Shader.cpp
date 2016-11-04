#include <openglpp/Shader.h>

#include <fstream>
#include <assert.h>

#include <iostream>  // debugging

std::shared_ptr<Shader> Shader::sDefaultShader = nullptr;
const char* Shader::POSITION_NAME = NULL;  // "in_Position";
const char* Shader::NORMAL_NAME = NULL;  // "in_Normal";
const char* Shader::COLOR_NAME = NULL;  // "in_Color";
const char* Shader::TEXCOORDS_NAME = NULL;  // "in_TexCoords";


Shader::Shader()
	: mProgramID(0), mShaderIDs {0, 0}
{
}

Shader::Shader(Shader&& rhs)
{
	mProgramID = rhs.mProgramID;
	rhs.mProgramID = 0;

	for (int i = 0; i < rhs.mShaderCount; i++) {
		mShaderIDs[i] = rhs.mShaderIDs[i];
		rhs.mShaderIDs[i] = 0;
	}

	mUniforms = rhs.mUniforms;
	mAttributes = rhs.mAttributes;
}

std::string fileToString(const std::string& path)
{
	std::ifstream f(path);
	if (!f.is_open())
		throw ShaderException() << "Shader at '" << path << "' is missing";

	// warning: this is inefficient for large files
	std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	f.close();
	return str;
}

std::shared_ptr<Shader> Shader::fromFile(const std::string& vertPath, const std::string& fragPath)
{
	std::string vertSrc = fileToString(vertPath);
	std::string fragSrc = fileToString(fragPath);
	return Shader::fromString(vertSrc, fragSrc);
}

std::shared_ptr<Shader> Shader::fromString(const std::string& vertShaderSrc, const std::string& fragShaderSrc)
{
	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	shader->mShaderIDs[0] = compileShader(vertShaderSrc, GL_VERTEX_SHADER);
	shader->mShaderIDs[1] = compileShader(fragShaderSrc, GL_FRAGMENT_SHADER);

	shader->mProgramID = glCreateProgram();
	if (shader->mProgramID == 0)
		throw ShaderException() << "Unable to create program";

	glAttachShader(shader->mProgramID, shader->mShaderIDs[0]);
	glAttachShader(shader->mProgramID, shader->mShaderIDs[1]);
	glLinkProgram(shader->mProgramID);

	GLint isLinked;
	glGetProgramiv(shader->mProgramID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength;
		glGetProgramiv(shader->mProgramID, GL_INFO_LOG_LENGTH, &maxLength);

		char* log = new char[maxLength];
		glGetProgramInfoLog(shader->mProgramID, maxLength, NULL, log);

		ShaderException exception;
		exception << log;

		delete[] log;
		// shader will be implicitly cleaned up as the stack unwinds
		throw exception;
	}

	// get uniform and attribute locations
	{
		GLint nUniforms = 0;
		glGetProgramiv(shader->mProgramID, GL_ACTIVE_UNIFORMS, &nUniforms);
		for (GLint i = 0; i < nUniforms; i++) {
			GLchar name[4096];
			GLint size;
			GLenum type;
			glGetActiveUniform(shader->mProgramID, i, sizeof(name), NULL, &size, &type, name);

			shader->mUniforms[std::string(name)] = { type, i };
		}
	}

	{
		GLint nAttribs = 0;
		glGetProgramiv(shader->mProgramID, GL_ACTIVE_ATTRIBUTES, &nAttribs);
		for (GLint i = 0; i < nAttribs; i++) {
			GLchar name[4096];
			GLint size;
			GLenum type;
			glGetActiveAttrib(shader->mProgramID, i, sizeof(name), NULL, &size, &type, name);

			shader->mAttributes[std::string(name)] = { type, glGetAttribLocation(shader->mProgramID, name) };
		}
	}

	return shader;
}

GLuint Shader::compileShader(const std::string& src, GLuint type)
{
	GLuint shader = glCreateShader(type);
	if (shader == 0)
		throw ShaderException() << "Could not create shader of type " << type;

	const char* srcArray[1] = { src.c_str() };
	glShaderSource(shader, 1, srcArray, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		// get error message
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		char* infoLog = new char[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, infoLog);

		ShaderException exception;
		exception << infoLog;

		delete[] infoLog;
		glDeleteShader(shader);

		throw exception;
	}

	return shader;
}

Shader::~Shader()
{
	if (mProgramID)
		glDeleteProgram(mProgramID);
	
	for (int i = 0; i < mShaderCount; i++)
	{
		if (mShaderIDs[i])
			glDeleteShader(mShaderIDs[i]);
	}
}

void Shader::use()
{
	glUseProgram(mProgramID);
}

GLint Shader::uniform(const char* name)
{
	auto it = mUniforms.find(name);
	if (it == mUniforms.end())
		throw GLException() << "Unknown uniform '" << name << "'";

	return it->second.idx;
}

GLint Shader::attrib(const char* name)
{
	auto it = mAttributes.find(name);
	if (it == mAttributes.end())
		throw GLException() << "Unknown attribute '" << name << "'";

	return it->second.idx;
}

void Shader::setUniform(const char* name, int val)
{
	glUniform1i(uniform(name), val);
}

void Shader::setUniform(const char* name, float val)
{
	// ignoring AttribData.type for now, could use to typecheck in the future
	glUniform1f(uniform(name), val);
}

void Shader::setUniform(const char* name, const glm::vec3& val)
{
	glUniform3fv(uniform(name), 1, &val[0]);
}

void Shader::setUniform(const char* name, const glm::vec4& val)
{
	glUniform4fv(uniform(name), 1, &val[0]);
}

void Shader::setUniform(const char* name, const glm::mat4x4& mat)
{
	glUniformMatrix4fv(uniform(name), 1, GL_FALSE, &mat[0][0]);
}
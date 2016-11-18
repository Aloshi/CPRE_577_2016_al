#pragma once

#include <memory>  // std::shared_ptr
#include <map>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

class Material {
public:
  inline Material()
	  : mShader(Shader::defaultShader()) {}
  inline Material(const std::shared_ptr<Shader>& shader) : mShader(shader) {}

  const std::shared_ptr<Shader>& shader() const { return mShader; }

  inline void set(const char* name, int v) { mInts[name] = v; }
  inline void set(const char* name, float v) { mFloats[name] = v; }
  inline void set(const char* name, const glm::vec3& vec) { mVec3s[name] = vec; }
  inline void set(const char* name, const glm::vec4& vec) { mVec4s[name] = vec; }
  inline void setTexture(int idx, const std::shared_ptr<Texture>& tex) {
    mTextures[idx] = tex;
  }

  inline const std::shared_ptr<Texture>& texture(int i) const {
	  assert(i < N_TEXTURE_SAMPLERS);
	  return mTextures[i];
  }

  inline void activate() {
    for (auto it = mInts.begin(); it != mInts.end(); it++)
      mShader->setUniform(it->first.c_str(), it->second);
    for (auto it = mFloats.begin(); it != mFloats.end(); it++)
      mShader->setUniform(it->first.c_str(), it->second);
    for (auto it = mVec3s.begin(); it != mVec3s.end(); it++)
      mShader->setUniform(it->first.c_str(), it->second);
    for (auto it = mVec4s.begin(); it != mVec4s.end(); it++)
      mShader->setUniform(it->first.c_str(), it->second);

    // bind textures
    for (unsigned int i = 0; i < N_TEXTURE_SAMPLERS; i++) {
      if (mTextures[i] != nullptr) {
        glActiveTexture(GL_TEXTURE0 + i);
        mTextures[i]->bind();
      }
    }
  }

private:
  static const int N_TEXTURE_SAMPLERS = 1;

  std::shared_ptr<Shader> mShader;
  std::map<std::string, int> mInts;
  std::map<std::string, float> mFloats;
  std::map<std::string, glm::vec3> mVec3s;
  std::map<std::string, glm::vec4> mVec4s;
  std::shared_ptr<Texture> mTextures[N_TEXTURE_SAMPLERS];
};
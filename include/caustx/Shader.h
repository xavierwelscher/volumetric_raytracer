#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

class ComputeShader {
				public:
								ComputeShader(const std::string& filepath, const std::vector<std::string>& includePaths = {});
								~ComputeShader();

								void Use() const;

								GLuint GetID() const { return ID; }

								void SetBool(const std::string& name, bool value) const;
								void SetInt(const std::string& name, int value) const;
								void SetFloat(const std::string& name, float value) const;
								void SetVec2(const std::string& name, const glm::vec2& value) const;
								void SetVec2(const std::string& name, float x, float y) const;
								void SetVec3(const std::string& name, const glm::vec3& value) const;
								void SetVec3(const std::string& name, float x, float y, float z) const;
								void SetMat4(const std::string& name, const glm::mat4& mat) const;

				private:
								GLuint ID;
};

#include <caustx/Shader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

ComputeShader::ComputeShader(const std::string& filepath, const std::vector<std::string>& includePaths) : ID(0) {
				std::string includeSource = "";

				for (const auto& incPath : includePaths) {
								std::ifstream incFile(incPath);
								if (!incFile.is_open()) {
												std::cerr << "ERROR: Include-File not found: " << incPath << std::endl;
												return;
								}
								std::stringstream incBuffer;
								incBuffer << incFile.rdbuf();
								includeSource += incBuffer.str() + "\n";
				}

				std::ifstream file(filepath);
				if (!file.is_open()) {
								std::cerr << "ERROR: Shader-File not found: " << filepath << std::endl;
								return;
				}

				std::string line;
				std::string versionLine = "#version 430 core\n";
				std::stringstream bodyStream;

				while (std::getline(file, line)) {
								if (line.rfind("#version", 0) == 0) {
												versionLine = line + "\n";
								} else {
												bodyStream << line << "\n";
								}
				}
				std::string bodySource = bodyStream.str();

				const char* sources[3];
				sources[0] = versionLine.c_str();
				sources[1] = includeSource.c_str();
				sources[2] = bodySource.c_str();

				GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
				glShaderSource(shader, 3, sources, nullptr);
				glCompileShader(shader);

				int success;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
								char infoLog[1024];
								glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
								std::cerr << "Kompilierungsfehler in " << filepath << ":\n" << infoLog << std::endl;
								glDeleteShader(shader);
								return;
				}

				ID = glCreateProgram();
				glAttachShader(ID, shader);
				glLinkProgram(ID);

				glGetProgramiv(ID, GL_LINK_STATUS, &success);
				if (!success) {
								char infoLog[1024];
								glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
								std::cerr << "Link-Fehler in " << filepath << ":\n" << infoLog << std::endl;
								glDeleteShader(shader);
								glDeleteProgram(ID);
								ID = 0;
								return;
				}

				glDeleteShader(shader);
}

ComputeShader::~ComputeShader() {
				if (ID != 0) {
								glDeleteProgram(ID);
				}
}

void ComputeShader::Use() const {
				glUseProgram(ID);
}

void ComputeShader::SetBool(const std::string& name, bool value) const {
				glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ComputeShader::SetInt(const std::string& name, int value) const {
				glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::SetFloat(const std::string& name, float value) const {
				glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::SetVec2(const std::string& name, const glm::vec2& value) const {
				glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ComputeShader::SetVec2(const std::string& name, float x, float y) const {
				glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void ComputeShader::SetVec3(const std::string& name, const glm::vec3& value) const {
				glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ComputeShader::SetVec3(const std::string& name, float x, float y, float z) const {
				glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void ComputeShader::SetMat4(const std::string& name, const glm::mat4& mat) const {
				glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

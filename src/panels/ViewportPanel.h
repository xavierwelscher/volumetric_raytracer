#pragma once

#include <caustx/panels/Panel.h>
#include <glad/gl.h>
#include <imgui.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <iomanip>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

class ViewportPanel : public IPanel {
public:
    ViewportPanel(const std::string& name, GLuint sceneTextureId, int width, int height, float* gpuTimeMs)
        : m_name(name), m_textureId(sceneTextureId), m_width(width), m_height(height), m_gpuTimeMs(gpuTimeMs) {}

    void onRender() override {
        ImGui::Begin(m_name.c_str());
        
				ImGuiIO& io = ImGui::GetIO();
        if (m_gpuTimeMs) {
            ImGui::Text("Resolution: %dx%d | FPS: %.1f | GPU Compute: %.2f ms", m_width, m_height, io.Framerate, *m_gpuTimeMs);
        } else {
            ImGui::Text("Resolution: %dx%d | FPS: %.1f", m_width, m_height, io.Framerate);
        }
        
				ImGui::SameLine();
				if (ImGui::Button("Save to PNG")) {
								SaveImage(m_name);
				}

        ImGui::Image(
            (ImTextureID)(intptr_t)m_textureId, 
            ImVec2((float)m_width, (float)m_height), 
            ImVec2(0, 1), ImVec2(1, 0)
        );
        
        ImGui::End();
    }

private:
		std::string m_name;
    GLuint m_textureId;
    int m_width;
    int m_height;
		float* m_gpuTimeMs;

		void SaveImage(const std::string& baseName) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
        std::string timestamp = ss.str();

				std::stringstream msStream;
				float msToLog = m_gpuTimeMs ? *m_gpuTimeMs : 0.0f;
				msStream << std::fixed << std::setprecision(2) << msToLog;
				std::string msString = msStream.str();
				
        std::string directory = "screenshots/";
        std::filesystem::create_directories(directory);

        std::string filepath = directory + baseName + "_" + timestamp + "_" + msString + "ms.png";

        std::vector<unsigned char> pixels(m_width * m_height * 4);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        
        stbi_flip_vertically_on_write(true);
        
        if (stbi_write_png(filepath.c_str(), m_width, m_height, 4, pixels.data(), m_width * 4)) {
								std::cout << "Image saved: " << filepath << std::endl;
        } else {
								std::cerr << "Error: could not save image " << filepath << std::endl;
        }
    }
};

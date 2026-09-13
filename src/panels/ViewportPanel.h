#pragma once

#include <caustx/panels/Panel.h>
#include <glad/gl.h>
#include <imgui.h>

class ViewportPanel : public IPanel {
public:
    ViewportPanel(GLuint sceneTextureId, int width, int height)
        : m_textureId(sceneTextureId), m_width(width), m_height(height) {}

    void onRender() override {
        ImGui::Begin("Volumetric Viewport");
        
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Resolution: %dx%d | FPS: %.1f", m_width, m_height, io.Framerate);
        
        ImGui::Image(
            (ImTextureID)(intptr_t)m_textureId, 
            ImVec2((float)m_width, (float)m_height), 
            ImVec2(0, 1), ImVec2(1, 0)
        );
        
        ImGui::End();
    }

private:
    GLuint m_textureId;
    int m_width;
    int m_height;
};

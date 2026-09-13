#pragma once
#include <caustx/Config.h>
#include <caustx/panels/Panel.h>
#include <glad/gl.h>
#include <imgui.h>

class TerrainSettingsPanel : public IPanel {
public:
    TerrainSettingsPanel(TerrainConfig& config, GLuint previewTextureId)
        : m_config(config), m_textureId(previewTextureId) {}

    void onRender() override {
        ImGui::Begin("Terrain Settings");

        ImGui::Text("Terrain Heightmap Preview:");
        ImGui::Image((ImTextureID)(intptr_t)m_textureId, ImVec2(256, 256));

				if (ImGui::Checkbox("isVisible", &m_config.isVisible)) {
								m_config.needsUpdate = true;
				}

        if (ImGui::SliderFloat("Scale", &m_config.scale, 1.0f, 20.0f)) {
            m_config.needsUpdate = true;
        }
        if (ImGui::SliderFloat2("Offset", m_config.offset, -10.0f, 10.0f)) {
            m_config.needsUpdate = true;
        }
        if (ImGui::Button("Regenerate")) {
            m_config.needsUpdate = true;
        }

        ImGui::End();
    }

private:
    TerrainConfig& m_config;
    GLuint m_textureId;
};

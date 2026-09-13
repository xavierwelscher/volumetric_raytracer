#pragma once
#include <caustx/panels/Panel.h>
#include <caustx/Config.h>
#include <imgui.h>

class CloudPanel : public IPanel {
				private:
								CloudConfig& config;

				public:
								CloudPanel(CloudConfig& cloudConfig) : config(cloudConfig) {}

								void onRender() override {
												ImGui::Begin("Volumetric Clouds");

												ImGui::Checkbox("Cloud Container Visible", &config.isVisible);

												if (config.isVisible) {
																ImGui::Separator();
																ImGui::Text("Cloud Shaping");
																ImGui::SliderFloat("Density Multiplier", &config.densityMultiplier, 0.0f, 10.0f);
																ImGui::SliderFloat("Light Absorption", &config.lightAbsorption, 0.0f, 2.0f);
																ImGui::SliderFloat("Noise Scale", &config.noiseScale, 0.1f, 5.0f);
												}

												ImGui::End();
								}
};

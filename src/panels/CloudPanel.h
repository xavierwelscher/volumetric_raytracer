#pragma once
#include <caustx/panels/Panel.h>
#include <caustx/Config.h>
#include <imgui.h>
#include <cstdlib>

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
																ImGui::SliderFloat("Density Multiplier", &config.densityMultiplier, 0.0f, 30.0f);
																ImGui::SliderFloat("Light Absorption", &config.lightAbsorption, 0.0f, 2.0f);
																ImGui::SliderFloat("Noise Scale", &config.noiseScale, 0.1f, 5.0f);
																ImGui::SliderFloat("Coverage", &config.coverage, 0.0f, 1.0f);

																ImGui::InputInt("Seed", &config.seed);
																ImGui::SameLine();
																if (ImGui::Button("Random")) {
																				config.seed = rand() % 100000;
																}


																ImGui::Separator();
																ImGui::Text("Time & Animation");
																ImGui::Checkbox("Pause Time", &config.timePaused);
																if (!config.timePaused) {
																				ImGui::SliderFloat("Time Speed", &config.timeSpeed, 0.1f, 10.0f, "%.1fx");
																}

																ImGui::Separator();
																ImGui::Text("Sun Position");
																ImGui::SliderFloat("Elevation", &config.sunElevation, -10.0f, 90.0f);
																ImGui::SliderFloat("Azimuth", &config.sunAzimuth, 0.0f, 360.0f);
												}

												ImGui::End();
								}
};

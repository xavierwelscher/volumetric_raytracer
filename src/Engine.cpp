#include <caustx/Engine.h>
#include "panels/TerrainSettingsPanel.h"
#include "panels/ViewportPanel.h"
#include "panels/CloudPanel.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <memory>
#include <cmath>

constexpr int RENDER_WIDTH = 800;
constexpr int RENDER_HEIGHT = 600;
constexpr int TERRAIN_SIZE = 1024;

CaustXEngine::CaustXEngine() 
				: window(nullptr), windowWidth(1280), windowHeight(720),
				renderTexture(0), heightmapTex(0) {}

				CaustXEngine::~CaustXEngine() {
								Shutdown();
				}

bool CaustXEngine::Init(int width, int height, const std::string& title) {
				windowWidth = width;
				windowHeight = height;

				if (!glfwInit()) return false;

				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

				window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
				if (!window) { 
								glfwTerminate(); 
								return false; 
				}
				glfwMakeContextCurrent(window);
				glfwSwapInterval(0);

				if (gladLoadGL((GLADloadfunc)glfwGetProcAddress) == 0) {
								std::cerr << "Error during GLAD-Initialization" << std::endl;
								return false;
				}

				terrainShader = std::make_unique<ComputeShader>("shaders/terrain_gen.comp", std::vector<std::string>{"shaders/noise.glsl"});
				raytraceShader = std::make_unique<ComputeShader>(
												"shaders/raytracer.comp", 
												std::vector<std::string>{
																"shaders/common.glsl",
																"shaders/terrain.glsl", 
																"shaders/clouds.glsl"
												}
								);

				if (terrainShader->GetID() == 0 || raytraceShader->GetID() == 0) {
								std::cerr << "Error: Shader could not be loaded." << std::endl;
								return false;
				}

				glGenTextures(1, &renderTexture);
				glBindTexture(GL_TEXTURE_2D, renderTexture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glBindImageTexture(0, renderTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

				glGenTextures(1, &heightmapTex);
				glBindTexture(GL_TEXTURE_2D, heightmapTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE); 

				glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, TERRAIN_SIZE, TERRAIN_SIZE, 0, GL_RED, GL_FLOAT, nullptr);

				IMGUI_CHECKVERSION();
				ImGui::CreateContext();
				ImGui::StyleColorsDark();
				ImGui_ImplGlfw_InitForOpenGL(window, true);
				ImGui_ImplOpenGL3_Init("#version 430");


				panels.push_back(std::make_unique<ViewportPanel>(renderTexture, RENDER_WIDTH, RENDER_HEIGHT));
				panels.push_back(std::make_unique<TerrainSettingsPanel>(terrainConfig, heightmapTex));
				panels.push_back(std::make_unique<CloudPanel>(cloudConfig));

				return true;
}

void CaustXEngine::Run() {
				float lastFrame = 0.0f;

				while (!glfwWindowShouldClose(window)) {
								float currentFrame = static_cast<float>(glfwGetTime());
								float deltaTime = currentFrame - lastFrame;
								lastFrame = currentFrame;

								glfwPollEvents();

								Update(deltaTime);
								Render();
								DrawUI();

								glfwSwapBuffers(window);
				}
}

void CaustXEngine::Update(float deltaTime) {
    glm::vec3 pos(cameraConfig.position[0], cameraConfig.position[1], cameraConfig.position[2]);
    glm::vec3 target(cameraConfig.target[0], cameraConfig.target[1], cameraConfig.target[2]);
    
    glm::vec3 forward = glm::normalize(target - pos);
    
    static double lastMouseX = 0.0;
    static double lastMouseY = 0.0;
    static bool isDragging = false;
    bool moved = false;

    // if (!ImGui::GetIO().WantCaptureMouse) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (!isDragging) {
                lastMouseX = mouseX;
                lastMouseY = mouseY;
                isDragging = true;
            }

            float deltaX = static_cast<float>(mouseX - lastMouseX);
            float deltaY = static_cast<float>(lastMouseY - mouseY);
            
            lastMouseX = mouseX;
            lastMouseY = mouseY;

            if (deltaX != 0.0f || deltaY != 0.0f) {
                float sensitivity = 0.005f; 
                
                float yaw = std::atan2(forward.z, forward.x);
                float pitch = std::asin(forward.y);

                yaw += deltaX * sensitivity;
                pitch += deltaY * sensitivity;

                if (pitch > 1.5f) pitch = 1.5f;
                if (pitch < -1.5f) pitch = -1.5f;

                forward.x = std::cos(yaw) * std::cos(pitch);
                forward.y = std::sin(pitch);
                forward.z = std::sin(yaw) * std::cos(pitch);
                forward = glm::normalize(forward);
                
                moved = true;
            }
        } else {
            isDragging = false;
        }
    // }

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    float speed = 10.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { pos += forward * speed; moved = true; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { pos -= forward * speed; moved = true; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { pos += right * speed; moved = true; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { pos -= right * speed; moved = true; }
    
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { pos.y += speed; moved = true; }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { pos.y -= speed; moved = true; }

    if (moved) {
        target = pos + forward; 
        
        cameraConfig.position[0] = pos.x; cameraConfig.position[1] = pos.y; cameraConfig.position[2] = pos.z;
        cameraConfig.target[0] = target.x; cameraConfig.target[1] = target.y; cameraConfig.target[2] = target.z;
    }
}

void CaustXEngine::Render() {
				if (terrainConfig.needsUpdate) {
								terrainShader->Use();
								glBindImageTexture(1, heightmapTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

								terrainShader->SetFloat("u_scale", terrainConfig.scale);
								terrainShader->SetVec2("u_offset", terrainConfig.offset[0], terrainConfig.offset[1]);

								glDispatchCompute(TERRAIN_SIZE / 16, TERRAIN_SIZE / 16, 1);
								glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

								terrainConfig.needsUpdate = false;
				}

				raytraceShader->Use();

				raytraceShader->SetVec3("u_camPos", cameraConfig.position[0], cameraConfig.position[1], cameraConfig.position[2]);
				raytraceShader->SetVec3("u_camTarget", cameraConfig.target[0], cameraConfig.target[1], cameraConfig.target[2]);
				raytraceShader->SetFloat("u_fov", cameraConfig.fov);

				raytraceShader->SetInt("u_terrainVisible", terrainConfig.isVisible ? 1 : 0);
				raytraceShader->SetInt("u_cloudVisible", cloudConfig.isVisible ? 1 : 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, heightmapTex);
				raytraceShader->SetInt("u_heightmap", 1);

				int workGroupsX = (RENDER_WIDTH + 7) / 8;
				int workGroupsY = (RENDER_HEIGHT + 7) / 8;
				glDispatchCompute(workGroupsX, workGroupsY, 1);
				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void CaustXEngine::DrawUI() {
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				for (auto& panel : panels) {
								panel->onRender();
				}

				ImGui::Render();

				int display_w, display_h;
				glfwGetFramebufferSize(window, &display_w, &display_h);
				glViewport(0, 0, display_w, display_h);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CaustXEngine::Shutdown() {
				if (renderTexture) glDeleteTextures(1, &renderTexture);
				if (heightmapTex) glDeleteTextures(1, &heightmapTex);

				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplGlfw_Shutdown();
				ImGui::DestroyContext();

				if (window) {
								glfwDestroyWindow(window);
								window = nullptr;
				}
				glfwTerminate();
}

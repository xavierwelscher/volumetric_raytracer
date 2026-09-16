#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <vector>

#include "Shader.h" 
#include "caustx/Config.h"
#include "caustx/panels/Panel.h"

class CaustXEngine {
public:
    CaustXEngine();
    ~CaustXEngine();

    bool Init(int width, int height, const std::string& title);
    void Run();

private:
    void Update(float deltaTime);
    void Render();
    void DrawUI();
    void Shutdown();

    GLFWwindow* window;
    int windowWidth;
    int windowHeight;

    std::unique_ptr<ComputeShader> raytraceShader;
    std::unique_ptr<ComputeShader> terrainShader;

    GLuint renderTexture;
		GLuint heatmapTexture;
    GLuint heightmapTex;

		TerrainConfig terrainConfig;
		CameraConfig cameraConfig;
		CloudConfig cloudConfig;
		std::vector<std::unique_ptr<IPanel>> panels;

		GLuint gpuTimerQuery;
		float gpuTimeMs = 0.0f;
};

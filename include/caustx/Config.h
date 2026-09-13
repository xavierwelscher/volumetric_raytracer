#pragma once

struct TerrainConfig {
				bool isVisible 		= true;
				float scale 			= 1.5f;
				float offset[2] 	= {0.0f, 0.0f};
				bool needsUpdate 	= true;
};

struct CameraConfig {
				float position[3] = {0.0f, 25.0f, -20.0f};
				float target[3] 	= {0.0f, 5.0f, 10.0f};
				float fov 				= 60.0f;
};

struct CloudConfig {
				bool isVisible = true;
				float noiseScale = 1.0f;
				float densityMultiplier = 1.0f;
				float lightAbsorption = 0.5f;
};

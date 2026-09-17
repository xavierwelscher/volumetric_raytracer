uniform float u_time;
uniform float u_cloudDensityMultiplier;
uniform float u_cloudLightAbsorption;
uniform float u_cloudNoiseScale;

float sdBox(vec3 p, vec3 b) {
				vec3 q = abs(p) - b;
				return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float getCloudDistance(vec3 p) {
    vec3 center = vec3(0.0, 15.0, 0.0);
    vec3 bounds = vec3(6.0, 4.0, 6.0);
    return sdBox(p - center, bounds);
}

float getCloudDensity(vec3 p) {
				float dist = getCloudDistance(p);

				if (dist > 0.0) {
								return 0.0;
				}

				float edgeSoftness = 1.0;
				float baseDensity = clamp(-dist * edgeSoftness, 0.0, 1.0);
				vec3 windOffset = vec3(u_time * 0.8, 0.0, u_time * 0.3);
				float noiseVal = fbm3D((p + windOffset) * u_cloudNoiseScale);
				float finalDensity = baseDensity - (1.0 - noiseVal) * 1.2;

				return max(finalDensity * u_cloudDensityMultiplier, 0.0);
}

float calculateLightEnergy(vec3 p, vec3 lightDir) {
				float lightTransmittance = 1.0;
				float lightStepSize = 0.5;
				vec3 lp = p;

				for (int i = 0; i < 6; i++) {
								lp += lightDir * lightStepSize;
								float d = getCloudDensity(lp);

								lightTransmittance *= exp(-d * lightStepSize * u_cloudLightAbsorption * 2.0); 

								if (lightTransmittance < 0.01) break;
				}
				return lightTransmittance;
}


void renderClouds(vec3 pos, float step_size, inout float transmittance, inout vec3 color) {
				float density = getCloudDensity(pos);

				if (density > 0.001) {
								float stepTransmittance = exp(-density * step_size * u_cloudLightAbsorption);

								vec3 lightDir = normalize(vec3(0.8, 0.8, -0.3));
								float lightEnergy = calculateLightEnergy(pos, lightDir);

								vec3 sunColor = vec3(1.0, 0.95, 0.9);
								vec3 ambientLight = vec3(0.2, 0.3, 0.4); 

								vec3 scatteredLight = (sunColor * lightEnergy * 1.5 + ambientLight) * density * step_size;
								color += transmittance * scatteredLight;
								transmittance *= stepTransmittance;
				}
}

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

float dualHenyeyGreenstein(float cosTheta) {
				float g1 = 0.8;  
				float g2 = -0.2;
				float blend = 0.5; 

				float g1Sq = g1 * g1;
				float g2Sq = g2 * g2;

				float pi = 3.14159265359;

				float phase1 = (1.0 - g1Sq) / (4.0 * pi * pow(1.0 + g1Sq - 2.0 * g1 * cosTheta, 1.5));
				float phase2 = (1.0 - g2Sq) / (4.0 * pi * pow(1.0 + g2Sq - 2.0 * g2 * cosTheta, 1.5));

				return mix(phase1, phase2, blend);
}

float calculateLightEnergy(vec3 p, vec3 lightDir) {
				float totalDensity = 0.0;
				float lightStepSize = 0.5;
				vec3 lp = p;

				for (int i = 0; i < 6; i++) {
								lp += lightDir * lightStepSize;
								float d = getCloudDensity(lp);
								totalDensity += d * lightStepSize;

								if (totalDensity * u_cloudLightAbsorption > 5.0) break;
				}

				float beer = exp(-totalDensity * u_cloudLightAbsorption);
				float powder = 1.0 - exp(-totalDensity * u_cloudLightAbsorption * 2.0);

				return beer * mix(1.0, powder * 2.0, 0.8);
}


void renderClouds(vec3 pos, vec3 rd, float step_size, inout float transmittance, inout vec3 accumulated_color) {
				float density = getCloudDensity(pos);

				if (density > 0.001) {
								float lightEnergy = calculateLightEnergy(pos, lightDir);
								float cosTheta = dot(rd, lightDir);
								float phaseVal = dualHenyeyGreenstein(cosTheta);
								
								vec3 cloudColor = vec3(1.0, 0.95, 0.9) * lightEnergy * phaseVal * 15.0;
								float stepTransmittance = exp(-density * step_size * u_cloudLightAbsorption);
								accumulated_color += transmittance * cloudColor * density * step_size;
								transmittance *= stepTransmittance;
				}
}

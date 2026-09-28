uniform float u_time;
uniform float u_cloudDensityMultiplier;
uniform float u_cloudLightAbsorption;
uniform float u_cloudNoiseScale;
uniform sampler3D u_noise3D;
uniform float u_cloudCoverage;
uniform sampler3D u_detailNoise3D;

float remap(float value, float originalMin, float originalMax, float newMin, float newMax) {
				return newMin + (((value - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

float sdBox(vec3 p, vec3 b) {
				vec3 q = abs(p) - b;
				return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float getDensityHeightGradient(float heightFraction) {
    float bottom = smoothstep(0.0, 0.05, heightFraction);
    float top = 1.0 - smoothstep(0.3, 1.0, heightFraction);
    return bottom * top;
}

float getCloudDistance(vec3 p) {
				vec3 center = vec3(0.0, 30.0, 0.0);
				vec3 bounds = vec3(200.0, 20.0, 200.0);
				return sdBox(p - center, bounds);
}

float getCloudDensity(vec3 p) {
				vec3 center = vec3(0.0, 30.0, 0.0);
				vec3 bounds = vec3(200.0, 20.0, 200.0);
				float dist = sdBox(p - center, bounds);

				if (dist > 0.0) return 0.0;
				
				float heightFraction = (p.y - (center.y - bounds.y)) / (2.0 * bounds.y); 
				float edgeSoftness = 0.5;
				float baseDensity = clamp(-dist * edgeSoftness, 0.0, 1.0);

				vec3 windOffset = vec3(u_time * 0.8, 0.0, u_time * 0.3);

				float weatherNoise = texture(u_noise3D, p * 0.001 + windOffset * 0.0005).r;
				float variedHeightFraction = heightFraction - (weatherNoise - 0.5) * 0.5;

				float bottom = smoothstep(0.1, 0.3, variedHeightFraction);
				float top = 1.0 - smoothstep(0.4, 0.9, variedHeightFraction);
				float heightGradient = bottom * top;

				vec3 samplePos = (p + windOffset) * u_cloudNoiseScale * 0.005;
				float noiseVal = texture(u_noise3D, samplePos).r;
				noiseVal *= heightGradient;

				float densityWithCoverage = noiseVal - u_cloudCoverage;
				float finalDensity = max(densityWithCoverage * baseDensity, 0.0);
				// float finalDensity = baseDensity - (1.0 - noiseVal) * 1.2;

				if (finalDensity > 0.0) {
								vec3 detailSamplePos = (p + windOffset * 1.5) * u_cloudNoiseScale * 0.02;
								float detailNoiseVal = texture(u_detailNoise3D, detailSamplePos).r;

								float highFreqNoiseModifier = mix(detailNoiseVal, 1.0 - detailNoiseVal, clamp(heightFraction * 3.0, 0.0, 1.0));
								float erodedDensity = remap(finalDensity, highFreqNoiseModifier * 0.2, 1.0, 0.0, 1.0);

								finalDensity = max(erodedDensity, 0.0);
				}

				return finalDensity * u_cloudDensityMultiplier;
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

								vec3 sunLightColor = vec3(1.0, 0.95, 0.9);
								vec3 directLight = sunLightColor * lightEnergy * phaseVal * 15.0;
								
								float heightGradient = clamp((pos.y - 5.0) / 20.0, 0.0, 1.0);
								vec3 skyBlue = vec3(0.3, 0.4, 0.5);
								vec3 earthBrown = vec3(0.15, 0.15, 0.18);
								vec3 ambientLight = mix(earthBrown, skyBlue, heightGradient);
								vec3 cloudColor = ambientLight + directLight;

								float stepTransmittance = exp(-density * step_size * u_cloudLightAbsorption);
								accumulated_color += transmittance * cloudColor * density * step_size;
								transmittance *= stepTransmittance;
				}
}

bool renderTerrain(vec3 pos, vec3 rd, float step_size, inout float transmittance, inout vec3 color) {
    vec2 terrainUV = (pos.xz * 0.05); 
    float rawHeight = texture(u_heightmap, terrainUV).r * MAX_HEIGHT;
    float actualHeight = max(rawHeight, WATER_LEVEL);

    if (pos.y < actualHeight) {
        vec3 prevPos = pos - rd * step_size;
        float prevRawHeight = texture(u_heightmap, prevPos.xz * 0.05).r * MAX_HEIGHT;
        float prevActualHeight = max(prevRawHeight, WATER_LEVEL);

        float distOver = prevPos.y - prevActualHeight;
        float distUnder = actualHeight - pos.y;
        float fraction = distOver / (distOver + distUnder);
        pos = prevPos + rd * (step_size * fraction);

        terrainUV = pos.xz * 0.05;
        actualHeight = max(texture(u_heightmap, terrainUV).r * MAX_HEIGHT, WATER_LEVEL);

        vec3 tColor;
       if (actualHeight <= WATER_LEVEL + 0.01) {
            tColor = vec3(0.1, 0.35, 0.7);
        } else {
            vec3 grassColor = vec3(0.08, 0.30, 0.05); 
            vec3 rockColor  = vec3(0.4, 0.4, 0.42);  
            vec3 snowColor  = vec3(0.95, 0.95, 0.98); 
            
            float rockBlend = smoothstep(WATER_LEVEL + 1.5, WATER_LEVEL + 2.0, actualHeight);
            vec3 groundColor = mix(grassColor, rockColor, rockBlend);
            float snowBlend = smoothstep(9.5, 10.0, actualHeight);
            
            tColor = mix(groundColor, snowColor, snowBlend);
        }

        float hR = max(texture(u_heightmap, terrainUV + vec2(0.01, 0.0)).r * MAX_HEIGHT, WATER_LEVEL);
        float hU = max(texture(u_heightmap, terrainUV + vec2(0.0, 0.01)).r * MAX_HEIGHT, WATER_LEVEL);
        vec3 normal = normalize(vec3(actualHeight - hR, 0.2, actualHeight - hU));

        float diffuse = max(dot(normal, lightDir), 0.0);
        tColor *= (diffuse * 0.8 + 0.3); 

        color += transmittance * tColor;
        transmittance = 0.0; 
        return true; 
    }

    float heightOverWater = pos.y - WATER_LEVEL;
    if (heightOverWater > 0.0 && heightOverWater < 8.0) {
        float density = (1.0 - (heightOverWater / 8.0)) * 0.05; 
        float stepTransmittance = exp(-density * step_size);
        vec3 stepScatteredLight = fogColor * density * step_size;

        color += transmittance * stepScatteredLight;
        transmittance *= stepTransmittance;
    }

    return false;
}

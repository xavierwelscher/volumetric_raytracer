float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float getCloudDensity(vec3 p) {
    return 0.0; 
}

bool renderClouds(vec3 pos, float step_size, inout float transmittance, inout vec3 color) {
    vec3 cloudCenter = vec3(0.0, 15.0, 0.0);
    float distToCloud = sdBox(pos - cloudCenter, vec3(5.0, 5.0, 5.0));

    if (distToCloud < 0.01) {
        vec2 e = vec2(0.01, 0.0);
        vec3 normal = normalize(vec3(
            sdBox(pos + e.xyy - cloudCenter, vec3(5.0)) - sdBox(pos - e.xyy - cloudCenter, vec3(5.0)), 
            sdBox(pos + e.yxy - cloudCenter, vec3(5.0)) - sdBox(pos - e.yxy - cloudCenter, vec3(5.0)), 
            sdBox(pos + e.yyx - cloudCenter, vec3(5.0)) - sdBox(pos - e.yyx - cloudCenter, vec3(5.0))
        ));

        float diffuse = max(dot(normal, lightDir), 0.0);
        vec3 cloudColor = vec3(0.8, 0.3, 0.3) * (diffuse * 0.8 + 0.2); 

        color += transmittance * cloudColor;
        transmittance = 0.0;
        return true;
    }
    return false;
}

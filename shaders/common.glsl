layout(binding = 1) uniform sampler2D u_heightmap;

uniform vec3 u_camPos;
uniform vec3 u_camTarget;
uniform float u_fov;

uniform int u_terrainVisible;
uniform int u_cloudVisible;

const vec3 lightDir = normalize(vec3(0.8, 0.8, -0.3)); 
const vec3 skyColor = vec3(0.5, 0.7, 0.9);
const vec3 fogColor = vec3(0.8, 0.85, 0.9);

const float WATER_LEVEL = 3.5;
const float SNOW_LEVEL = 11.0;
const float MAX_HEIGHT = 15.0;

mat3 setCamera(in vec3 ro, in vec3 ta, float cr) {
    vec3 cw = normalize(ta - ro); 
    vec3 cp = vec3(sin(cr), cos(cr), 0.0); 
    vec3 cu = normalize(cross(cw, cp));    
    vec3 cv = normalize(cross(cu, cw));    
    return mat3(cu, cv, cw);
}

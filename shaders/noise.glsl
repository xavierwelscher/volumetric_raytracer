// Dave Hoskins Hash
vec2 hash21(vec2 p) {
				vec3 p3 = fract(vec3(p.xyx) * vec3(0.1031, 0.1030, 0.0973));
				p3 += dot(p3, p3.yzx + 33.33);
				vec2 g = fract((p3.xx + p3.yz) * p3.zy) * 2.0 - 1.0;
				return normalize(g);
}

vec2 hash22(vec2 p) {
				vec3 p3 = fract(vec3(p.xyx) * vec3(0.1031, 0.1030, 0.0973));
				p3 += dot(p3, p3.yzx + 33.33);
				return fract((p3.xx + p3.yz) * p3.zy);
}

float hash31(vec3 p3) {
				p3 = fract(p3 * 0.1031);
				p3 += dot(p3, p3.yzx + 33.33);
				return fract((p3.x + p3.y) * p3.z);
}

vec3 hash33(vec3 p) {
				p = vec3(dot(p, vec3(127.1, 311.7, 74.7)),
												dot(p, vec3(269.5, 183.3, 246.1)),
												dot(p, vec3(113.5, 271.9, 124.6)));
				return fract(sin(p) * 43758.5453123);
}

float noise3D(vec3 x) {
				vec3 i = floor(x);
				vec3 f = fract(x);
				f = f * f * (3.0 - 2.0 * f);

				return mix(mix(mix(hash31(i + vec3(0,0,0)), hash31(i + vec3(1,0,0)), f.x),
																mix(hash31(i + vec3(0,1,0)), hash31(i + vec3(1,1,0)), f.x), f.y),
												mix(mix(hash31(i + vec3(0,0,1)), hash31(i + vec3(1,0,1)), f.x),
																mix(hash31(i + vec3(0,1,1)), hash31(i + vec3(1,1,1)), f.x), f.y), f.z);
}

float perlin(vec2 p, float wrap) {
				vec2 i = floor(p);
				vec2 f = fract(p);

				vec2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

				vec2 i00 = mod(i + vec2(0.0, 0.0), wrap);
				vec2 i10 = mod(i + vec2(1.0, 0.0), wrap);
				vec2 i01 = mod(i + vec2(0.0, 1.0), wrap);
				vec2 i11 = mod(i + vec2(1.0, 1.0), wrap);

				vec2 g00 = hash21(i00);
				vec2 g10 = hash21(i10);
				vec2 g01 = hash21(i01);
				vec2 g11 = hash21(i11);

				vec2 d00 = f - vec2(0.0, 0.0);
				vec2 d10 = f - vec2(1.0, 0.0);
				vec2 d01 = f - vec2(0.0, 1.0);
				vec2 d11 = f - vec2(1.0, 1.0);

				float n00 = dot(g00, d00);
				float n10 = dot(g10, d10);
				float n01 = dot(g01, d01);
				float n11 = dot(g11, d11);

				float nx0 = mix(n00, n10, u.x);
				float nx1 = mix(n01, n11, u.x);
				float nxy = mix(nx0, nx1, u.y);

				return nxy * 0.5 + 0.5;
}

float fbmPerlin(vec2 p, float wrap) {
				float total = 0.0;
				float amplitude = 0.5;
				float frequency = 1.0;
				float maxValue = 0.0;

				for (int i = 0; i < 6; i++) {
								total += perlin(p * frequency, wrap * frequency) * amplitude;
								maxValue += amplitude;
								frequency *= 2.0;
								amplitude *= 0.5;
				}

				return total / maxValue;
}

float worley(vec2 p, float wrap) {
				vec2 n = floor(p); 				
				vec2 f = fract(p); 
				float min_dist = 1.0; 

				for (int y = -1; y <= 1; y++) {
								for (int x = -1; x <= 1; x++) {
												vec2 neighbor = vec2(float(x), float(y));
												vec2 point = hash22(mod(n + neighbor, wrap));
												vec2 diff = neighbor + point - f;
												float dist = length(diff);
												min_dist = min(min_dist, dist);
								}
				}

				return min_dist;
}

float worley3D(vec3 p) {
    vec3 n = floor(p);
    vec3 f = fract(p);
    float min_dist = 1.0;
    
    for(int k = -1; k <= 1; k++) {
        for(int j = -1; j <= 1; j++) {
            for(int i = -1; i <= 1; i++) {
                vec3 neighbor = vec3(float(i), float(j), float(k));
                vec3 point = hash33(n + neighbor);
                vec3 diff = neighbor + point - f;
                float dist = length(diff);
                min_dist = min(min_dist, dist);
            }
        }
    }
    return min_dist;
}

float fbm3D(vec3 p) {
				float f = 0.0;
				float amp = 0.5;

				for(int i = 0; i < 4; i++) {
								f += amp * noise3D(p);
								p *= 2.0;
								amp *= 0.5;
				}

				return f;
}

float fbmWorley(vec2 p, float wrap) {
				float total = 0.0;
				float amplitude = 0.5;
				float frequency = 1.0;
				float maxValue = 0.0;

				for (int i = 0; i < 6; i++) {
								total += worley(p * frequency, wrap * frequency) * amplitude;
								maxValue += amplitude;
								frequency *= 2.0;
								amplitude *= 0.5;
				}

				return total / maxValue;
}

float fbmWorley3D(vec3 p) {
				float total = 0.0;
				float amplitude = 0.5;
				float maxValue = 0.0;

				for (int i = 0; i < 3; i++) { 
								total += worley3D(p) * amplitude;
								maxValue += amplitude;
								p *= 2.0;
								amplitude *= 0.5;
				}

				return total / maxValue;
}

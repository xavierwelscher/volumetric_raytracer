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

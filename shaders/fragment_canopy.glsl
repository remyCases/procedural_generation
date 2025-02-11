// Fragment
#version 330 core
out vec4 FragColor;

uniform vec2 resolution;
uniform float time;
uniform float thickness;
uniform float decay;
uniform float branch_length;
uniform float branch_angle;
uniform vec3 color1;         // First color gradient
uniform vec3 color2;         // Second color gradient

#define MAX_ITERATIONS 8

struct Branch 
{
    vec2 start;
    vec2 end;
};

float line(vec2 p, vec2 a, vec2 b, float thickness) 
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h) - thickness;
}

Branch generate_branch(vec2 p, float angle, float length) 
{
    Branch b;
    b.start = p;
    b.end = p + vec2(cos(angle), sin(angle)) * length;

    return b;
}

float canopy_fractal(vec2 uv) {
    vec2 p = uv * 2.0 - 1.0;
    p *= resolution.x / resolution.y;
    float lines = 1.0;
    
    // Base trunk
    vec2 trunk = vec2(0.0, -0.8);
    float base_angle = 3.14159 / 2.0;

    // Initial trunk branches
    float angle_variation = 0; //time * 0.2;
    Branch left_branch = generate_branch(trunk, base_angle + angle_variation, branch_length);
    Branch right_branch = generate_branch(trunk, base_angle - angle_variation, branch_length);

    // Track branches for recursive generation
    Branch current_branches[MAX_ITERATIONS];
    current_branches[0] = left_branch;
    current_branches[1] = right_branch;
    int active_branches = 2;

    // Fractal generation
    for (float i = 0.0; i < MAX_ITERATIONS; i++) 
    {
        Branch next_branches[MAX_ITERATIONS * 2];
        int next_branch_count = 0;

        for (int j = 0; j < active_branches; j++) 
        {
            Branch current = current_branches[j];

            // Draw line for current generate_branch
            lines = min(lines, abs(line(p, current.start, current.end, thickness)));

            // Generate sub-branches
            float angle_variation = i * branch_angle; //time * 0.2 + i * branch_angle;

            // Left and right sub-branches for each parent generate_branch
            float parent_angle = atan(
                current.end.y - current.start.y, 
                current.end.x - current.start.x
            );

            Branch left_sub = generate_branch(current.end, parent_angle + angle_variation, branch_length * pow(decay, i));
            Branch right_sub = generate_branch(current.end, parent_angle - angle_variation, branch_length * pow(decay, i));

            next_branches[next_branch_count++] = left_sub;
            next_branches[next_branch_count++] = right_sub;
        }

        // Update branches for next iteration
        for (int k = 0; k < next_branch_count; k++) 
        {
            current_branches[k] = next_branches[k];
        }
        active_branches = next_branch_count;
    }
    
    return 1.0 - smoothstep(0.0, thickness, lines);
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;
    float fractal = canopy_fractal(uv);
    
    // Flexible color mixing with debug visualization
    vec3 color = mix(color1, color2, fractal);
    
    FragColor = vec4(color, 1.0);
};

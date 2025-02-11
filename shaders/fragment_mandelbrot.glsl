#version 330 core
out vec4 FragColor;
uniform vec2 resolution;

void main() 
{
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 4.0 - vec2(2.0);
    uv.x *= resolution.x / resolution.y;

    vec2 z = vec2(0.0);
    vec2 c = uv;
    float iter = 0.0;
    const float MAX_ITER = 100.0;

    for(float i = 0.0; i < MAX_ITER; i++) 
    {
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        if(dot(z, z) > 4.0) {
            iter = i;
            break;
        }
    }

    vec3 color;
    if(iter == MAX_ITER) 
    {
        color = vec3(0.0);
    } 
    else
    {
        float normalized = iter / MAX_ITER;
        color = vec3(normalized * 0.5, normalized, normalized * 0.7);
    }
    FragColor = vec4(color, 1.0);
};
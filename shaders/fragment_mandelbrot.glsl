#version 330 core
precision highp float;
out vec4 FragColor;
uniform vec2 resolution;
uniform vec2 offset;
uniform float zoom;
uniform float time;
uniform bool show_glow;

//By Björn Ottosson
//https://bottosson.github.io/posts/oklab
//Shader functions adapted by "mattz"
//https://www.shadertoy.com/view/WtccD7

#define GAMMA 2.2
//Classic gamma correction functions
vec3 linear_from_srgb(vec3 rgb)
{
    return pow(rgb, vec3(GAMMA));
}
vec3 srgb_from_linear(vec3 lin)
{
    return pow(lin, vec3(1.0/GAMMA));
}

vec3 oklab_from_linear(vec3 linear)
{
    const mat3 im1 = mat3(0.4121656120, 0.2118591070, 0.0883097947,
                          0.5362752080, 0.6807189584, 0.2818474174,
                          0.0514575653, 0.1074065790, 0.6302613616);
                       
    const mat3 im2 = mat3(+0.2104542553, +1.9779984951, +0.0259040371,
                          +0.7936177850, -2.4285922050, +0.7827717662,
                          -0.0040720468, +0.4505937099, -0.8086757660);
                       
    vec3 lms = im1 * linear;
            
    return im2 * (sign(lms) * pow(abs(lms), vec3(1.0/3.0)));
}

vec3 linear_from_oklab(vec3 oklab)
{
    const mat3 m1 = mat3(+1.000000000, +1.000000000, +1.000000000,
                         +0.396337777, -0.105561346, -0.089484178,
                         +0.215803757, -0.063854173, -1.291485548);
                       
    const mat3 m2 = mat3(+4.076724529, -1.268143773, -0.004111989,
                         -3.307216883, +2.609332323, -0.703476310,
                         +0.230759054, -0.341134429, +1.706862569);
    vec3 lms = m1 * oklab;
    
    return m2 * (lms * lms * lms);
}

//By Inigo Quilez, under MIT license
//https://www.shadertoy.com/view/ttcyRS
vec3 oklab_mix(vec3 lin1, vec3 lin2, float a)
{
    // https://bottosson.github.io/posts/oklab
    const mat3 kCONEtoLMS = mat3(                
         0.4121656120,  0.2118591070,  0.0883097947,
         0.5362752080,  0.6807189584,  0.2818474174,
         0.0514575653,  0.1074065790,  0.6302613616);
    const mat3 kLMStoCONE = mat3(
         4.0767245293, -1.2681437731, -0.0041119885,
        -3.3072168827,  2.6093323231, -0.7034763098,
         0.2307590544, -0.3411344290,  1.7068625689);
                    
    // rgb to cone (arg of pow can't be negative)
    vec3 lms1 = pow( kCONEtoLMS*lin1, vec3(1.0/3.0) );
    vec3 lms2 = pow( kCONEtoLMS*lin2, vec3(1.0/3.0) );
    // lerp
    vec3 lms = mix( lms1, lms2, a );
    // gain in the middle (no oklab anymore, but looks better?)
    lms *= 1.0+0.2*a*(1.0-a);
    // cone to rgb
    return kLMStoCONE*(lms*lms*lms);
}

float get_adaptive_iterations(float zoom, vec2 uv) 
{
    float BASE_ITER = 1000.0;
    float distance_from_center = length(uv);
    float zoom_factor = log(zoom + 1.0);
    
    return BASE_ITER * (1.0 + zoom_factor);
}

vec3 get_color_and_glow(vec2 z, float de, float iter, float max_iter) 
{
    if(iter >= max_iter) return vec3(0.0);
    
    // Smooth iteration count
    float log_zn = log(length(z)) / 2.0;
    float nu = log(log_zn / log(2.0)) / log(2.0);
    float smoothed = iter + 1.0 - nu;
    float normalized = smoothed / max_iter;
    
    vec3 rgb1 = vec3(0.0, 0.0, 0.0);
    vec3 rgb2 = vec3(0.5, 1.0, 0.7);
    
    //Convert to linear color space
    vec3 lin1 = linear_from_srgb(rgb1);
    vec3 lin2 = linear_from_srgb(rgb2);

    vec3 base_color_oklab = srgb_from_linear(oklab_mix(lin1, lin2, normalized));

    // Add glow based on distance estimation
    float glow_intensity = 1.0 / (de * 2.0);            // Reduced multiplication factor for stronger effect
    glow_intensity = pow(glow_intensity, 1.5);          // Adjust power for stronger falloff
    glow_intensity = clamp(glow_intensity, 0.0, 5.0);   // Allow for overbright glow

    // Create multiple layers of glow with different colors
    float inner_glow = smoothstep(0.0, 1.0, glow_intensity);
    float mid_glow = smoothstep(0.2, 0.8, glow_intensity);
    float outer_glow = smoothstep(0.4, 0.6, glow_intensity);

    // Define more vibrant glow colors
    vec3 inner_color = vec3(1.0, 0.3, 0.1);    // Bright orange-red
    vec3 mid_color = vec3(1.0, 0.8, 0.2);      // Bright yellow
    vec3 outer_color = vec3(0.2, 0.5, 1.0);    // Bright blue
    
    float t = iter / max_iter;
    vec3 base_color_rgb  = vec3(t * 0.5, t, t * 0.7);

    // Combine all glow layers
    vec3 col = base_color_oklab;

    if (!show_glow) 
    {
        return col;
    }
    
    col = mix(col, outer_color, outer_glow * 0.8);
    col = mix(col, mid_color, mid_glow * 0.6);
    col = mix(col, inner_color, inner_glow * 0.4);

    // Add brightness boost
    col *= (1.0 + glow_intensity * 0.5);

    return col;
}

vec3 fractal(vec2 uv, float max_iter)
{
    vec2 c = uv;
    float dr = 1.0;
    vec2 z = vec2(0.0);
    vec2 dz = vec2(1.0, 0.0);
    vec2 sum_dz = vec2(0.0);
    float iter = 0.0;
    float dbail = 1e6;
    
    for(float i = 0.0; i < max_iter; i++) 
    {
        dr = 2.0 * length(z) * dr;
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        dz = vec2(z.x * dz.x - z.y * dz.y + 1.0, z.x * dz.y + dz.x * z.y);
        sum_dz += dz;
        if(dot(sum_dz, sum_dz) > dbail) {
            iter = i;
            break;
        }
    }

    // Calculate distance estimation
    float mod_z = length(z);
    float de = 2.0 * mod_z * log(mod_z) / dr;

    vec3 color = get_color_and_glow(z, de, iter, max_iter);

    return color;
}

void main() 
{

    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 4.0 - vec2(2.0);
    uv.x *= resolution.x / resolution.y;
    uv = uv / zoom + offset;  // Apply zoom and pan

    float MAX_ITER = get_adaptive_iterations(zoom, uv);
    vec3 color = fractal(uv, MAX_ITER);

    // Add post-processing effects
    color = pow(color, vec3(0.8));     // Gamma correction
    color *= 1.2;                      // Brightness boost
       
    FragColor = vec4(color, 1.0);
};
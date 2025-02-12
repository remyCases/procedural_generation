#version 330 core
precision highp float;
out vec4 FragColor;
uniform vec2 resolution;
uniform vec2 offset;
uniform float zoom;
uniform float time;

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
    float distance_factor = exp(-distance_from_center);
    
    return BASE_ITER * (1.0 + zoom_factor * distance_factor);
}

vec3 get_color(vec2 z, float iter, float max_iter) 
{
    if(iter == max_iter) return vec3(0.0);
    
    // Smooth iteration count
    float log_zn = log(length(z)) / 2.0;
    float nu = log(log_zn / log(2.0)) / log(2.0);
    float smoothed = iter + 1.0 - nu;
    float normalized = smoothed / max_iter;
    
    //Pseudo random colors
    vec3 rgb1 = cos(floor(time / 10.0)*vec3(4,7,8))*+.5+.5;
    vec3 rgb2 = cos(round(time / 10.0)*vec3(4,7,8))*-.5+.5;

    vec3 lin1 = vec3(0.0, 0.2, 0.2);
    vec3 lin2 = vec3(1.0, 0.3, 0.9);
    
    //Convert to linear color space
    lin1 = linear_from_srgb(rgb1);
    lin2 = linear_from_srgb(rgb2);

    return srgb_from_linear(oklab_mix(lin1, lin2, normalized));
}

vec3 fractal(vec2 uv, float max_iter)
{
    vec2 c = uv;
    vec2 z = vec2(0.0);
    vec2 dz = vec2(1.0, 0.0);
    vec2 sum_dz = vec2(0.0);
    float iter = 0.0;
    float dbail = 1e6;
    
    for(float i = 0.0; i < max_iter; i++) 
    {
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        dz = vec2(z.x * dz.x - z.y * dz.y + 1.0, z.x * dz.y + dz.x * z.y);
        sum_dz += dz;
        if(dot(sum_dz, sum_dz) > dbail) {
            iter = i;
            break;
        }
    }

    vec3 color = get_color(z, iter, max_iter);

    return color;
}

void main() 
{

    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 4.0 - vec2(2.0);
    uv.x *= resolution.x / resolution.y;
    uv = uv / zoom + offset;  // Apply zoom and pan

    float MAX_ITER = get_adaptive_iterations(zoom, uv);
    vec3 color = fractal(uv, MAX_ITER);
       
    FragColor = vec4(color, 1.0);
};
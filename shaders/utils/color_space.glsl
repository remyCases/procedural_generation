// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

// no version indication here it will be included and not used as its own

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

vec3 rgb2xyz(vec3 rgb) 
{
    // Convert RGB to XYZ color space
    vec3 xyz;
    vec3 rgb2 = rgb * rgb; // Gamma correction
    xyz.x = dot(vec3(0.4124564, 0.3575761, 0.1804375), rgb2);
    xyz.y = dot(vec3(0.2126729, 0.7151522, 0.0721750), rgb2);
    xyz.z = dot(vec3(0.0193339, 0.1191920, 0.9503041), rgb2);
    return xyz;
}

vec3 xyz2lab(vec3 xyz) 
{
    // Convert XYZ to LAB color space
    vec3 n = xyz / vec3(0.95047, 1.0, 1.08883);
    vec3 v;
    v.x = (n.x > 0.008856) ? pow(n.x, 1.0/3.0) : (7.787 * n.x) + (16.0/116.0);
    v.y = (n.y > 0.008856) ? pow(n.y, 1.0/3.0) : (7.787 * n.y) + (16.0/116.0);
    v.z = (n.z > 0.008856) ? pow(n.z, 1.0/3.0) : (7.787 * n.z) + (16.0/116.0);
    return vec3((116.0 * v.y) - 16.0, 500.0 * (v.x - v.y), 200.0 * (v.y - v.z));
}

vec3 lab2xyz(vec3 lab) 
{
    float fy = (lab.x + 16.0) / 116.0;
    float fx = lab.y / 500.0 + fy;
    float fz = fy - lab.z / 200.0;
    
    float fx3 = fx * fx * fx;
    float fy3 = fy * fy * fy;
    float fz3 = fz * fz * fz;
    
    return vec3(
        0.95047 * ((fx3 > 0.008856) ? fx3 : (fx - 16.0/116.0) / 7.787),
        1.00000 * ((fy3 > 0.008856) ? fy3 : (fy - 16.0/116.0) / 7.787),
        1.08883 * ((fz3 > 0.008856) ? fz3 : (fz - 16.0/116.0) / 7.787)
    );
}

vec3 xyz2rgb(vec3 xyz) 
{
    // Convert XYZ back to RGB
    vec3 rgb;
    rgb.r = dot(vec3( 3.2404542, -1.5371385, -0.4985314), xyz);
    rgb.g = dot(vec3(-0.9692660,  1.8760108,  0.0415560), xyz);
    rgb.b = dot(vec3( 0.0556434, -0.2040259,  1.0572252), xyz);
    return sqrt(rgb); // Inverse gamma correction
}

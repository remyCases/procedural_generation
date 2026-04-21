// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#version 330 core
precision highp float;
out vec4 FragColor;
uniform vec2 resolution;
uniform vec2 offset;
uniform float zoom;
uniform float time;
uniform bool show_glow;

#include "color_space.glsl"

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
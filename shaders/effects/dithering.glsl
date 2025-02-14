
// Convert RGB to perceived brightness
float get_luminance(vec3 color) 
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

// 8x8 Bayer matrix for higher quality ordered dithering
const float bayer_matrix[64] = float[64](
    0.0/64.0,  48.0/64.0, 12.0/64.0, 60.0/64.0, 3.0/64.0,  51.0/64.0, 15.0/64.0, 63.0/64.0,
    32.0/64.0, 16.0/64.0, 44.0/64.0, 28.0/64.0, 35.0/64.0, 19.0/64.0, 47.0/64.0, 31.0/64.0,
    8.0/64.0,  56.0/64.0, 4.0/64.0,  52.0/64.0, 11.0/64.0, 59.0/64.0, 7.0/64.0,  55.0/64.0,
    40.0/64.0, 24.0/64.0, 36.0/64.0, 20.0/64.0, 43.0/64.0, 27.0/64.0, 39.0/64.0, 23.0/64.0,
    2.0/64.0,  50.0/64.0, 14.0/64.0, 62.0/64.0, 1.0/64.0,  49.0/64.0, 13.0/64.0, 61.0/64.0,
    34.0/64.0, 18.0/64.0, 46.0/64.0, 30.0/64.0, 33.0/64.0, 17.0/64.0, 45.0/64.0, 29.0/64.0,
    10.0/64.0, 58.0/64.0, 6.0/64.0,  54.0/64.0, 9.0/64.0,  57.0/64.0, 5.0/64.0,  53.0/64.0,
    42.0/64.0, 26.0/64.0, 38.0/64.0, 22.0/64.0, 41.0/64.0, 25.0/64.0, 37.0/64.0, 21.0/64.0
);

// Blue noise texture (you would need to provide this as a uniform)
uniform sampler2D blue_noise_texture;

// Hash function for random dithering
float hash(vec2 p) 
{
    vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 apply_dithering(vec2 frag_coord, vec3 color, vec3 neighbor_color, int dithering_pattern, float strength) 
{
    float dither = 0.0;
    float luma = get_luminance(color);
    float local_dither_strength = strength * (1.0 - pow(abs(2.0 * luma - 1.0), 2.0));

    // Select dithering pattern
    if (dithering_pattern == 0)
    {
        // Bayer dithering
        ivec2 pos = ivec2(frag_coord.xy) % 8;
        dither = bayer_matrix[pos.y * 8 + pos.x];
    }
    else if (dithering_pattern == 1) 
    {
        // Blue noise dithering
        vec2 noise_coord = frag_coord.xy / vec2(textureSize(blue_noise_texture, 0));
        dither = texture(blue_noise_texture, noise_coord).r;
    }
    else if (dithering_pattern == 2) 
    {
        // Simplified Floyd-Steinberg-like pattern
        vec2 pos = fract(frag_coord.xy / 2.0);
        dither = (pos.x + pos.y) / 2.0;
    }
    else if (dithering_pattern == 3) 
    {
        // Random dithering
        dither = hash(frag_coord.xy);
    }
    
    vec3 color_diff = abs(color - neighbor_color);
    vec3 edge_intensity = 1.0 - smoothstep(0.0, 0.1, color_diff);
    
    // Reduce dithering near edges
    local_dither_strength *= min(edge_intensity.r, min(edge_intensity.g, edge_intensity.b));

    // Apply dithering
    return color + (dither - 0.5) * local_dither_strength * (1.0 - abs(color - 0.5));
}
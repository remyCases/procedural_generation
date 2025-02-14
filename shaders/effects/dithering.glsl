
// 8x8 Bayer matrix for higher quality ordered dithering
const float bayer_matrix[64] = float[64](
    0, 32, 8, 40, 2, 34, 10, 42,
    48, 16, 56, 24, 50, 18, 58, 26,
    12, 44, 4, 36, 14, 46, 6, 38,
    60, 28, 52, 20, 62, 30, 54, 22,
    3, 35, 11, 43, 1, 33, 9, 41,
    51, 19, 59, 27, 49, 17, 57, 25,
    15, 47, 7, 39, 13, 45, 5, 37,
    63, 31, 55, 23, 61, 29, 53, 21
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

float apply_dithering(vec2 frag_coord, int dithering_pattern, float strength) 
{
    float dither = 0.0;
    
    // Select dithering pattern
    if (dithering_pattern == 0)
    {
        // Bayer dithering
        ivec2 pos = ivec2(frag_coord.xy) % 8;
        dither = bayer_matrix[pos.y * 8 + pos.x] / 64.0;
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
    
    // Apply dithering
    return (dither - 0.5) * strength;
}
// Fragment
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D source_texture;
uniform int dithering_pattern;
uniform float dithering_strength;
uniform int quantization_method;
uniform int quantization_levels;

// Include other shader files
#include "dithering.glsl"
#include "quantization.glsl"

void main() 
{
    // Get input color
    vec4 color = texture(source_texture, TexCoord);
    
    // Apply effects in sequence
    vec3 result = color.rgb;
    vec3 neighbor_color = textureOffset(source_texture, TexCoord, ivec2(1, 1)).rgb;
    
    // quantize
    result = apply_quantization(result, quantization_method, quantization_levels);

    // dithering
    result = apply_dithering(gl_FragCoord.xy, result, neighbor_color, dithering_pattern, dithering_strength);
    
    FragColor = vec4(result, color.a);
}

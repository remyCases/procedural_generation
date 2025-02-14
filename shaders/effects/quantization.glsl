
// Linear quantization
vec3 quantize_linear(vec3 color, int quantization_levels) 
{
    float steps = float(quantization_levels - 1);
    return floor(color * steps + 0.5) / steps;
}

// Gamma-corrected quantization
vec3 quantize_gamma(vec3 color, int quantization_levels) 
{
    float gamma = 2.2;
    vec3 gamma_color = pow(color, vec3(gamma));
    float steps = float(quantization_levels - 1);
    gamma_color = floor(gamma_color * steps + 0.5) / steps;
    return pow(gamma_color, vec3(1.0/gamma));
}

// Perceptual quantization using luminance-based weights
vec3 quantize_perceptual(vec3 color, int quantization_levels) 
{
    vec3 weights = vec3(0.299, 0.587, 0.114); // RGB to luminance weights
    float luma = dot(color, weights);
    float steps = float(quantization_levels - 1);
    
    // Adjust quantization based on luminance
    float adjustment = 1.0 + (luma * 0.5); // More levels in brighter areas
    vec3 adjusted = floor(color * steps * adjustment + 0.5) / (steps * adjustment);
    
    return adjusted;
}

vec3 apply_quantization(vec3 color, int quantization_method, int quantization_levels) 
{
    // Apply quantization based on selected method
    if (quantization_method == 0) 
    {
        color.rgb = quantize_linear(color.rgb, quantization_levels);
    }
    else if (quantization_method == 1) 
    {
        color.rgb = quantize_gamma(color.rgb, quantization_levels);
    }
    else if (quantization_method == 2) 
    {
        color.rgb = quantize_perceptual(color.rgb, quantization_levels);
    }

    return color;
}
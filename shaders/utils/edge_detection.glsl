// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

// no version indication here it will be included and not used as its own

float getEdgeIntensity(vec2 uv) {
    vec3 center = texture(sourceTexture, uv).rgb;
    
    // Sobel operators
    vec3 dx = -texture(sourceTexture, uv + vec2(-1, -1) / textureSize(sourceTexture, 0)).rgb
              -2.0 * texture(sourceTexture, uv + vec2(-1, 0) / textureSize(sourceTexture, 0)).rgb
              -texture(sourceTexture, uv + vec2(-1, 1) / textureSize(sourceTexture, 0)).rgb
              +texture(sourceTexture, uv + vec2(1, -1) / textureSize(sourceTexture, 0)).rgb
              +2.0 * texture(sourceTexture, uv + vec2(1, 0) / textureSize(sourceTexture, 0)).rgb
              +texture(sourceTexture, uv + vec2(1, 1) / textureSize(sourceTexture, 0)).rgb;
              
    vec3 dy = -texture(sourceTexture, uv + vec2(-1, -1) / textureSize(sourceTexture, 0)).rgb
              -2.0 * texture(sourceTexture, uv + vec2(0, -1) / textureSize(sourceTexture, 0)).rgb
              -texture(sourceTexture, uv + vec2(1, -1) / textureSize(sourceTexture, 0)).rgb
              +texture(sourceTexture, uv + vec2(-1, 1) / textureSize(sourceTexture, 0)).rgb
              +2.0 * texture(sourceTexture, uv + vec2(0, 1) / textureSize(sourceTexture, 0)).rgb
              +texture(sourceTexture, uv + vec2(1, 1) / textureSize(sourceTexture, 0)).rgb;
              
    return length(dx) + length(dy);
}
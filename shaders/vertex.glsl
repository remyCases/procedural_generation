// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#version 330 core
layout (location = 0) in vec2 position;
void main() 
{
   gl_Position = vec4(position, 0.0, 1.0);
}
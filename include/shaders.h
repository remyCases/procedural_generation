// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#ifndef SHADERS_H_
#define SHADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

char* read_shader_file(const char*);
int create_shader_program(GLuint*, const char*);

#endif /* !SHADERS_H_ */
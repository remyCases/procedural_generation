// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#ifndef SHADERS_H_
#define SHADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "error.h"
#include "structs.h"
#include "shaders_preprocessing.h"

int create_shader_program(data_t*);

#endif /* !SHADERS_H_ */
// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of MSLYYC_exploration project from .

#ifndef SAVE_H_
#define SAVE_H_

#include <stdint.h>
#include <stdlib.h>
#include <png.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "structs.h"

int save_png(const char*, data_t*);

#endif /* !SAVE_H_ */

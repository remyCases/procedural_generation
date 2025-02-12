// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of MSLYYC_exploration project from .

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow*, int, int);
void error_callback(int, const char*);
void scroll_callback(GLFWwindow*, double, double);
void mouse_button_callback(GLFWwindow*, int, int, int);
void cursor_position_callback(GLFWwindow*, double, double);

#endif /* !CALLBACKS_H_ */
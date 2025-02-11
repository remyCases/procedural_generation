// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of MSLYYC_exploration project from .

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct window_data_s window_data_t;
typedef struct data_s data_t;

struct window_data_s 
{
    int* width;
    int* height;
};

struct data_s
{
    GLuint vao;
    GLuint vbo;
    GLuint shader_program;
    GLFWwindow* window;
    int window_width;
    int window_height;
    window_data_t window_data;
};

#endif /* !STRUCTS_H_ */
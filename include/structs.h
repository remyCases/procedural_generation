// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#define NK_GLFW_GL3_IMPLEMENTATION

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

typedef struct state_s state_t;
typedef struct data_s data_t;

struct state_s 
{
    int width;
    int height;
    float zoom;
    float offset[2];
    double last_x;
    double last_y;
    int is_dragging;

    // UI state
    int show_glow;
};

struct data_s
{
    GLuint vao;
    GLuint vbo;
    GLuint shader_program;
    GLFWwindow* window;
    int window_width;
    int window_height;
    state_t state;
};

#endif /* !STRUCTS_H_ */
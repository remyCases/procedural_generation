// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of MSLYYC_exploration project from .

#include "../include/callbacks.h"
#include "../include/structs.h"

void error_callback(int error, const char* description) 
{
    printf("GLFW Error %d: %s\n", error, description);
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    window_data_t* data = (window_data_t*)glfwGetWindowUserPointer(window);
    // Update window size
    *data->width = width;
    *data->height = height;
    // Update viewport
    glViewport(0, 0, width, height);
}

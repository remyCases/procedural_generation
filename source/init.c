// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of MSLYYC_exploration project from .

#include "../include/init.h"
#include "../include/callbacks.h"

static int init_data(int height, int width, data_t* data)
{
    data->window_height = height;
    data->window_width = width;

    data->window_data.width = &data->window_width;
    data->window_data.height = &data->window_height;
}

static int init_vaovbo(GLuint* VAO, GLuint* VBO) 
{
    // Vertex data for fullscreen quad
    float vertices[] = 
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    // Create and bind VAO and VBO
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

static int init_window(GLFWwindow** p_window, window_data_t* window_data) 
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    // for debug purpose
    printf("[>] GLFW Version: %s\n", glfwGetVersionString());

    // OpenGL context hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(*window_data->width, *window_data->height, "Mandelbrot Set", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, window_data);

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
        fprintf(stderr, "GLEW initialization failed: %s\n", glewGetErrorString(err));
        glfwTerminate();
        return -1;
    }

    *p_window = window;

    // Set the viewport and resize callback
    glViewport(0, 0, (float)*window_data->width, (float)*window_data->height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return 0;
}

int init(int height, int width, data_t* data)
{
    init_data(height, width, data);
    init_window(&data->window, &data->window_data);
    init_vaovbo(&data->vao, &data->vbo);
}
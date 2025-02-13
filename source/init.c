// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/init.h"
#include "../include/callbacks.h"

static int init_data(int height, int width, data_t* data)
{
    int last_status = PG_SUCCESS;

    data->window_height = height;
    data->window_width = width;

    data->state.zoom = 1.0f;
    data->state.offset[0] = 0.0f;
    data->state.offset[1] = 0.0f;
    data->state.last_x = 0.0;
    data->state.last_y = 0.0;
    data->state.is_dragging = 0;
    data->state.width = data->window_width;
    data->state.height = data->window_height;
    data->state.show_glow = 0;

    return last_status;
}

static int init_vaovbo_generation(GLuint* VAO, GLuint* VBO) 
{
    int last_status = PG_SUCCESS;

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

    return last_status;
}

static int init_vaovbo_image(GLuint* VAO, GLuint* VBO) 
{
    int last_status = PG_SUCCESS;

    // Vertex data for fullscreen quad
    float vertices[] = 
    {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    // Create and bind VAO and VBO
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); 

    return last_status;
}

static int init_window(GLFWwindow** p_window, state_t* state) 
{
    int last_status = PG_SUCCESS;

    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return PG_INITIALIZATION_ERROR;
    }
    // for debug purpose
    printf("[>] GLFW Version: %s\n", glfwGetVersionString());

    // OpenGL context hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    GLFWwindow* window = glfwCreateWindow(state->width, state->height, "Mandelbrot Set", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return PG_GLFW_WINDOW_ERROR;
    }
    glfwSetWindowUserPointer(window, state);

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
        fprintf(stderr, "GLEW initialization failed: %s\n", glewGetErrorString(err));
        glfwTerminate();
        return PG_INITIALIZATION_ERROR;
    }

    *p_window = window;

    // Set the viewport and resize callback
    glViewport(0, 0, (float)state->width, (float)state->height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // enable MSAA
    glEnable(GL_MULTISAMPLE); 

    return last_status;
}

int init(int height, int width, data_t* data)
{
    int last_status = PG_SUCCESS;

    CHECK_CALL(init_data, height, width, data);
    CHECK_CALL(init_window, &data->window, &data->state);
    CHECK_CALL(init_vaovbo, &data->vao, &data->vbo);

    return last_status;
}
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

static int init_vaovbo_image(GLuint* VAO, GLuint* VBO, GLuint* EBO) 
{
    int last_status = PG_SUCCESS;

    // Vertex data for fullscreen quad
    float vertices[] = 
    {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f    // top left
    };

    unsigned int indices[] = 
    {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Clear any existing vertex attribute bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create and bind VAO and VBO
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);
    
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLint vertex_attrib_enabled = 0;
    GLint tex_coord_attrib_enabled = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &vertex_attrib_enabled);
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &tex_coord_attrib_enabled);
    PRINT("Position attribute enabled: %d\n", vertex_attrib_enabled);
    PRINT("TexCoord attribute enabled: %d\n", tex_coord_attrib_enabled);

    return last_status;
}

static int init_image(const char* path, GLuint* p_texture)
{
    int last_status = PG_SUCCESS;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w;
    int h;
    int n_channels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &n_channels, 0);

    if(!image) return PG_EXTERNAL_ERROR;

    printf("[>] Image loaded successfully: %dx%d with %d channels\n", w, h, n_channels);
    // If your image has an alpha channel (4 channels), use GL_RGBA instead of GL_RGB
    GLenum format = (n_channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) 
    {
        fprintf(stderr, "OpenGL error after texture creation: 0x%x\n", error);
        return PG_INITIALIZATION_ERROR;
    }

    GLint width_check = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width_check);
    PRINT("Texture dimensions in OpenGL: %d\n", width_check);

    // And check the texture binding
    GLint current_texture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);
    PRINT("Currently bound texture: %u\n", current_texture);

    GLint mipmap_level = 0;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &mipmap_level);
    PRINT("Mipmap level: %d\n", mipmap_level);

    *p_texture = texture;

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

    switch (data->flag & 1)
    {
    case PROCEDURAL:
        CHECK_CALL(init_vaovbo_generation, &data->vao, &data->vbo);
        break;

    case IMAGE:
        CHECK_CALL(init_image, data->path, &data->texture);
        CHECK_CALL(init_vaovbo_image, &data->vao, &data->vbo, &data->ebo);
        break;
    
    default:
        return PG_INVALID_PARAMETER;
        break;
    }
    

    return last_status;
}
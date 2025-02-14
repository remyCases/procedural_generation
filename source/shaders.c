// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/shaders.h"

static int choose_shaders_path(data_t* data, const char** vertex_path, const char** fragment_path)
{
    int last_status = PG_SUCCESS;
    int type = data->flag & 1;
    int procedural = data->flag >> 1;

    switch (type)
    {
    case PROCEDURAL:
        switch(procedural)
        {
            case MANDELBROT:
                *fragment_path = "shaders/fragment_mandelbrot.glsl";
                break;

            case CANOPY:
                *fragment_path = "shaders/fragment_canopy.glsl";
                break;

            default:
                return PG_INVALID_PARAMETER;
                break;
        }
        *vertex_path = "shaders/vertex.glsl";
        break;
    
    case IMAGE:
        *fragment_path = "shaders/fragment_postprocessing.frag";
        *vertex_path = "shaders/vertex_postprocessing.glsl";
        break;

    default:
        return PG_INVALID_PARAMETER;
        break;
    }

    return last_status;
}

static int read_shader_file(const char* file_path, const GLchar** buf) 
{
    FILE* file = fopen(file_path, "rb");
    if (!file) 
    {
        fprintf(stderr, "Error opening shader file: %s\n", file_path);
        return PG_ACCESS_DENIED;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // Allocate memory
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) 
    {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return PG_ALLOCATION_ERROR;
    }

    // Read file contents
    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) 
    {
        fprintf(stderr, "Error reading shader file\n");
        free(buffer);
        fclose(file);
        return PG_EXTERNAL_ERROR;
    }

    // Null-terminate string
    buffer[file_size] = '\0';
    fclose(file);

    *buf = buffer;
    return PG_SUCCESS;
}

static int create_shader(const char* shader_path, int shader_type, GLuint* p_shader)
{
    int last_status = PG_SUCCESS;
    GLint success;
    GLchar info_log[512];

    const GLchar* shader_source = NULL;
    CHECK_CALL(read_shader_file, shader_path, &shader_source);

    // Compile shader
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    // Error checking
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "shader compilation failed: %s\n", info_log);
        return last_status;
    }

    *p_shader = shader;

    return last_status;
}

int create_shader_program(data_t* data) 
{
    int last_status = PG_SUCCESS;
    GLint success;
    GLchar info_log[512];

    const char* vertex_shader_path = NULL;
    const char* fragment_shader_path = NULL;
    CHECK_CALL(choose_shaders_path, data, &vertex_shader_path, &fragment_shader_path);

    GLuint vertex_shader;
    GLuint fragment_shader;
    CHECK_CALL(create_shader, vertex_shader_path, GL_VERTEX_SHADER, &vertex_shader);
    CHECK_CALL(create_shader_fragment, fragment_shader_path, &fragment_shader);
    
    // Create shader program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check program linking
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) 
    {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Shader program linking failed: %s\n", info_log);
        return PG_FAIL;
    }

    // Add debug prints here
    GLint pos_attrib = glGetAttribLocation(shader_program, "aPos");
    GLint tex_attrib = glGetAttribLocation(shader_program, "aTexCoord");
    PRINT("Position attribute location: %d\n", pos_attrib);
    PRINT("TexCoord attribute location: %d\n", tex_attrib);

    GLint tex_uniform = glGetUniformLocation(shader_program, "texture1");
    PRINT("Texture uniform location: %d\n", tex_uniform);

    data->shader_program = shader_program;

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return last_status;
}

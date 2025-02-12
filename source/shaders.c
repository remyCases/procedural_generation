// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/shaders.h"

static int read_shader_file(const char* file_path, char** buf) 
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

int create_shader_program(GLuint* p_shader_program, const char* fragment_shader_path) 
{
    int last_status = PG_SUCCESS;
    GLint success;
    GLchar info_log[512];

    char* vertex_shader_source = NULL;
    CHECK_CALL(read_shader_file, "shaders/vertex.glsl", &vertex_shader_source);

    // Compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    // Error checking
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "vertex_shader compilation failed: %s\n", info_log);
        return last_status;
    }

    char* fragment_shader_source = NULL;
    CHECK_CALL(read_shader_file, fragment_shader_path, &fragment_shader_source);

    // Compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // Error checking
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        glGetShaderInfoLog(fragment_shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "fragment_shader compilation failed: %s\n", info_log);
        return last_status;
    }

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

    *p_shader_program = shader_program;

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return last_status;
}

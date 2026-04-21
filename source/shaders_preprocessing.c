// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/shaders_preprocessing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int get_file_size(const char* filename, size_t* size)
{
    int last_status = PG_SUCCESS;

    FILE* file = fopen(filename, "rb");
    if (!file) 
    {
        return PG_ACCESS_DENIED;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fclose(file);

    return last_status;
}

static int read_file(const char* filename, const char** buf) 
{
    int last_status = PG_SUCCESS;

    PRINT("Trying opening %s", filename);
    
    // Get file size
    size_t size = 0;
    CHECK_CALL(get_file_size, filename, &size);
    
    FILE* file = fopen(filename, "rb");
    if (!file) 
    {
        return PG_ACCESS_DENIED;
    }

    // Read file content
    char* content = (char*)malloc(size + 1);
    if (!content)
    {
        return PG_ALLOCATION_ERROR;
    }

    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);

    *buf = content;

    PRINT("Subfragment loaded %s", filename);
    return last_status;
}

// Very basic include processing
// NOTE: Currently searches all base_paths sequentially and breaks on first success.
// Do not reuse with complex/multiple includes
static int process_includes(const char* base_path[], int base_path_size, const char* shader_source, char** buf) 
{
    int last_status = PG_SUCCESS;

    const char* shader_start = shader_source;
    PRINT("Shader script\t %llu", strlen(shader_source));
    char* result = strdup(shader_source);

    const char* included_content = NULL;
    size_t content_size = 0;
    size_t total_added_size = 0;
    size_t total_deleted_size = 0;
    char* include_start = NULL;

    if ((include_start = strstr(shader_start, "#include"))) 
    {
        result[include_start - shader_start] = '\0';
    }
    else
    {
        *buf = result;
        return last_status;
    }

    PRINT("Shader header\t %llu", strlen(result));
    char* quote_start = NULL;
    char* quote_end = NULL;
    
    while ((include_start = strstr(shader_start, "#include")))
    {
        char filename[256];
        content_size = 0;
    
        quote_start = strchr(include_start, '"');
        quote_end = strchr(quote_start + 1, '"');
        size_t filename_size = quote_end - quote_start - 1;
        total_deleted_size += quote_end - include_start;
        
        // Extract filename
        strncpy(filename, quote_start + 1, filename_size);
        filename[filename_size] = '\0';
        
        // Build full path
        char full_path[512];

        // amongst all base_path, find the included file
        for (int i = 0; i < base_path_size; i++)
        {
            snprintf(full_path, sizeof(full_path), "%s/%s", base_path[i], filename);

            FILE* file = fopen(full_path, "rb");
            if (!file) continue;
            fclose(file);

            PRINT("Find file %s", full_path);

            // Read included file
            CHECK_CALL_GOTO_ERROR(get_file_size, cleanup, full_path, &content_size);
            total_added_size += content_size;
            CHECK_CALL_GOTO_ERROR(read_file, cleanup, full_path, &included_content);

            if (last_status == PG_SUCCESS) 
            {
                break;
            }
        }

        PRINT("Shader subscript\t %llu", content_size);
        result = (char*)realloc(result, strlen(result) + content_size + 1);
        if (!result)
        {
            return PG_ALLOCATION_ERROR;
        }

        strncat(result, included_content, content_size);
        PRINT("Current file\t %llu", strlen(result));

        shader_start = quote_end;
    }

    PRINT("Shader program\t %llu", strlen(shader_start));
    PRINT("Shader with included subscript %llu", strlen(result) + strlen(shader_start));
    PRINT("Expected size\t %llu", total_added_size + strlen(shader_source) - total_deleted_size);

    assert(strlen(result) + strlen(shader_start) == total_added_size + strlen(shader_source) - total_deleted_size);
    result = (char*)realloc(result, strlen(result) + strlen(shader_start));
    if (!result)
    {
        return PG_ALLOCATION_ERROR;
    }
    strncat(result, shader_start + 1, strlen(shader_start));
    
    *buf = result;

    cleanup:
    if((void*)included_content) free((void*)included_content);

    return last_status;
}

int create_shader_fragment(const char* frag_path, GLuint* fragment_shader)
{
    int last_status = PG_SUCCESS;
    const char* shader_file = NULL;
    const char* base_paths[2] = {"shaders/effects", "shaders/utils"};
    char* processed_shader = NULL;

    CHECK_CALL(read_file, frag_path, &shader_file);
    CHECK_CALL(process_includes, base_paths, 2, shader_file, &processed_shader);

    // Now use processed_shader with glShaderSource
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, (const GLchar**)&processed_shader, NULL);
    glCompileShader(shader);
    
    free(processed_shader);
    free((void*)shader_file);
    
    *fragment_shader = shader;

    return last_status;
}
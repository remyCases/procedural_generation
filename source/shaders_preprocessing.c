// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/shaders_preprocessing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_file(const char* filename, const char** buf) 
{
    int last_status = PG_SUCCESS;

    FILE* file = fopen(filename, "rb");
    if (!file) 
    {
        return PG_ACCESS_DENIED;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file content
    char* content = (char*)malloc(size + 1);
    if (!content)
    {
        return PG_ALLOCATION_ERROR;
    }

    fread(content, 1, size, file);
    content[size] = '\0';
    
    PRINT("READ\n%s\n\n", content);
    fclose(file);

    *buf = content;

    PRINT("Subfragment loaded %s", filename);
    return last_status;
}

// Very basic include processing
static int process_includes(const char* base_path, const char* shader_source, char** buf) 
{
    int last_status = PG_SUCCESS;

    char* result = strdup(shader_source);
    const char* included_content = NULL;
    char* include_start = NULL;
    char* new_result = NULL;
    
    while ((include_start = strstr(result, "#include"))) 
    {
        char filename[256];
        char* quote_start = strchr(include_start, '"');
        char* quote_end = strchr(quote_start + 1, '"');
        
        // Extract filename
        strncpy(filename, quote_start + 1, quote_end - quote_start - 1);
        filename[quote_end - quote_start - 1] = '\0';
        
        // Build full path
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, filename);
        
        // Read included file
        CHECK_CALL_GOTO_ERROR(read_file, cleanup, full_path, &included_content);
        
        // Replace include directive with file content
        new_result = (char*)malloc(strlen(result) + strlen(included_content) + 1);
        if (!new_result)
        {
            return PG_ALLOCATION_ERROR;
        }

        strncpy(new_result, result, include_start - result);
        PRINT("1\n%s\n\n", new_result);
        strcpy(new_result + (include_start - result), included_content);
        PRINT("2\n%s\n\n", new_result);
        strcat(new_result, quote_end + 1);
        PRINT("3\n%s\n\n", new_result);
        
        free((void*)included_content);
        included_content = NULL;
        free(result);
        result = new_result;

    }
    
    *buf = result;

    cleanup:
    if((void*)included_content) free((void*)included_content);

    return last_status;
}

int create_shader_fragment(const char* frag_path, GLuint* fragment_shader)
{
    int last_status = PG_SUCCESS;
    const char* shader_file = NULL;
    char* processed_shader = NULL;

    CHECK_CALL(read_file, frag_path, &shader_file);
    CHECK_CALL(process_includes, "shaders/effects", shader_file, &processed_shader);

    // Now use processed_shader with glShaderSource
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, (const GLchar**)&processed_shader, NULL);
    glCompileShader(shader);
    
    free(processed_shader);
    free((void*)shader_file);
    
    *fragment_shader = shader;

    return last_status;
}
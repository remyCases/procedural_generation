#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <png.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/utils_macro.h"
#include "include/shaders.h"
#include "include/callbacks.h"
#include "include/structs.h"
#include "include/save.h"
#include "include/init.h"
#include "include/error.h"

#define WIDTH 800
#define HEIGHT 600

int parse_args(int, char**, data_t*);
int display(GLFWwindow*, int, GLuint, GLuint, GLuint, state_t*);

int parse_args(int argc, char** argv, data_t* data)
{
    int last_status = PG_SUCCESS;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "mandelbrot"))
        {
            data->flag = PROCEDURAL | (MANDELBROT << 1);
        }
        else if (!strcmp(argv[1], "canopy")) 
        {
            data->flag = PROCEDURAL | (CANOPY << 1);
        }
        else if (!strcmp(argv[1], "file") && argc > 2)
        {
            data->flag = IMAGE;
            data->path = argv[2];
        }
        else return PG_INVALID_PARAMETER;
    }

    return last_status;
}

int display(GLFWwindow* window, int type, GLuint shader_program, GLuint VAO, GLuint texture, state_t* state)
{
    int last_status = PG_SUCCESS;
    // Clear screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Uniform locations
    GLint resolution_loc = glGetUniformLocation(shader_program, "resolution");
    GLint time_loc = glGetUniformLocation(shader_program, "time");
    GLint zoom_loc = glGetUniformLocation(shader_program, "zoom");
    GLint offset_loc = glGetUniformLocation(shader_program, "offset");
    GLint glow_loc = glGetUniformLocation(shader_program, "show_glow");

    // Use shader program
    glUseProgram(shader_program);

    switch(type)
    {
        case PROCEDURAL:

            glUniform1f(glGetUniformLocation(shader_program, "thickness"), 0.005);
            glUniform1f(glGetUniformLocation(shader_program, "branch_angle"), M_PI/6);
            glUniform1f(glGetUniformLocation(shader_program, "branch_length"), 0.5);
            glUniform1f(glGetUniformLocation(shader_program, "decay"), 0.5);
            glUniform3f(glGetUniformLocation(shader_program, "color1"), 0.0, 0.0, 0.0);
            glUniform3f(glGetUniformLocation(shader_program, "color2"), 0.5, 1.0, 0.7);
            
            glUniform2f(resolution_loc, (float)state->width, (float)state->height);
            glUniform1f(time_loc, glfwGetTime());
            glUniform1f(zoom_loc, state->zoom);
            glUniform2f(offset_loc, state->offset[0], state->offset[1]);
            glUniform1f(glow_loc, state->show_glow);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            break;

        case IMAGE:
            
            glUniform1i(glGetUniformLocation(shader_program, "source_texture"), 0);
            glUniform1i(glGetUniformLocation(shader_program, "dithering_pattern"), 0);
            glUniform1f(glGetUniformLocation(shader_program, "dithering_strength"), 1.0);
            glUniform1i(glGetUniformLocation(shader_program, "quantization_method"), 0);
            glUniform1i(glGetUniformLocation(shader_program, "quantization_levels"), 256);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            break;

        default:
            break;
    }

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();

    return last_status;
}

int main(int argc, char** argv) 
{
    int last_status = PG_SUCCESS;

    glfwSetErrorCallback(error_callback);
    
    // init data
    data_t data = { 0 };
    CHECK_CALL_GOTO_ERROR(parse_args, cleanup, argc, argv, &data);
    CHECK_CALL(init, HEIGHT, WIDTH, &data);

    // Create and use shader program
    CHECK_CALL_GOTO_ERROR(create_shader_program, cleanup, &data)

    printf("[>] Initialization done.\n");
    // main
    while (!glfwWindowShouldClose(data.window)) 
    {
        CHECK_CALL_GOTO_ERROR(display, cleanup, 
            data.window, 
            data.flag & 1, 
            data.shader_program, 
            data.vao, 
            data.texture,
            &data.state);
    }

    // export
    CHECK_CALL_GOTO_ERROR(save_png, cleanup, "export/fractal.png", &data);

    // Cleanup
    cleanup:
    glDeleteVertexArrays(1, &data.vao);
    glDeleteBuffers(1, &data.vbo);
    glDeleteBuffers(1, &data.ebo);
    glDeleteProgram(data.shader_program);
    
    glfwTerminate();
    return last_status;
}
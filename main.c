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

int parse_args(int, char**, const char**);
int display(GLFWwindow*, GLuint*, GLuint*, state_t*);

int parse_args(int argc, char** argv, const char** fragment_shader_path)
{
    int last_status = PG_SUCCESS;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "mandelbrot")) *fragment_shader_path = "shaders/fragment_mandelbrot.glsl";
        else if (!strcmp(argv[1], "canopy")) *fragment_shader_path = "shaders/fragment_canopy.glsl";
        else return PG_INVALID_PARAMETER;
    }

    return last_status;
}

int display(GLFWwindow* window, GLuint* shader_program, GLuint* VAO, state_t* state)
{
    int last_status = PG_SUCCESS;
    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Uniform locations
    GLint resolution_loc = glGetUniformLocation(*shader_program, "resolution");
    GLint time_loc = glGetUniformLocation(*shader_program, "time");
    GLint zoom_loc = glGetUniformLocation(*shader_program, "zoom");
    GLint offset_loc = glGetUniformLocation(*shader_program, "offset");
    GLint glow_loc = glGetUniformLocation(*shader_program, "show_glow");
    glUniform1f(glGetUniformLocation(*shader_program, "thickness"), 0.005);
    glUniform1f(glGetUniformLocation(*shader_program, "branch_angle"), M_PI/6);
    glUniform1f(glGetUniformLocation(*shader_program, "branch_length"), 0.5);
    glUniform1f(glGetUniformLocation(*shader_program, "decay"), 0.5);
    glUniform3f(glGetUniformLocation(*shader_program, "color1"), 0.0, 0.0, 0.0);
    glUniform3f(glGetUniformLocation(*shader_program, "color2"), 0.5, 1.0, 0.7);

    // Use shader program
    glUseProgram(*shader_program);

    // Update resolution uniform
    glUniform2f(resolution_loc, (float)state->width, (float)state->height);
    glUniform1f(time_loc, glfwGetTime());
    glUniform1f(zoom_loc, state->zoom);
    glUniform2f(offset_loc, state->offset[0], state->offset[1]);
    glUniform1f(glow_loc, state->show_glow);

    // Draw fullscreen quad
    glBindVertexArray(*VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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
    CHECK_CALL(init, HEIGHT, WIDTH, &data);

    const char* fragment_shader_path = NULL;
    CHECK_CALL_GOTO_ERROR(parse_args, cleanup, argc, argv, &fragment_shader_path);

    // Create and use shader program
    CHECK_CALL_GOTO_ERROR(create_shader_program, cleanup, &data.shader_program, fragment_shader_path)

    printf("[>] Initialization done.\n");
    // main
    while (!glfwWindowShouldClose(data.window)) 
    {
        CHECK_CALL_GOTO_ERROR(display, cleanup, data.window, &data.shader_program, &data.vao, &data.state);
    }

    // export
    CHECK_CALL_GOTO_ERROR(save_png, cleanup, "export/fractal.png", &data);

    // Cleanup
    cleanup:
    glDeleteVertexArrays(1, &data.vao);
    glDeleteBuffers(1, &data.vbo);
    glDeleteProgram(data.shader_program);
    
    glfwTerminate();
    return last_status;
}
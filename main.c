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

#define WIDTH 800
#define HEIGHT 600

void display(GLFWwindow*, GLuint*, GLuint*, window_data_t*);

void display(GLFWwindow* window, GLuint* shader_program, GLuint* VAO, window_data_t* window_data)
{
    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Uniform locations
    GLint resolution_loc = glGetUniformLocation(*shader_program, "resolution");
    GLint time_loc = glGetUniformLocation(*shader_program, "time");
    GLint zoom_loc = glGetUniformLocation(*shader_program, "zoom");
    GLint offset_loc = glGetUniformLocation(*shader_program, "offset");
    glUniform1f(glGetUniformLocation(*shader_program, "thickness"), 0.005);
    glUniform1f(glGetUniformLocation(*shader_program, "branch_angle"), M_PI/6);
    glUniform1f(glGetUniformLocation(*shader_program, "branch_length"), 0.5);
    glUniform1f(glGetUniformLocation(*shader_program, "decay"), 0.5);
    glUniform3f(glGetUniformLocation(*shader_program, "color1"), 0.0, 0.0, 0.0);
    glUniform3f(glGetUniformLocation(*shader_program, "color2"), 0.5, 1.0, 0.7);

    // Use shader program
    glUseProgram(*shader_program);

    // Update resolution uniform
    glUniform2f(resolution_loc, (float)*window_data->width, (float)*window_data->height);
    glUniform1f(time_loc, glfwGetTime());
    glUniform1f(zoom_loc, window_data->zoom);
    glUniform2f(offset_loc, window_data->offset[0], window_data->offset[1]);

    // Draw fullscreen quad
    glBindVertexArray(*VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(int argc, char** argv) 
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    glfwSetErrorCallback(error_callback);

    // init data
    data_t data = { 0 };
    init(HEIGHT, WIDTH, &data);

    const char* fragment_shader_path = NULL;
    if (argc > 1)
    {
        if (!strcmp(argv[1], "mandelbrot")) fragment_shader_path = "shaders/fragment_mandelbrot.glsl";
        else if (!strcmp(argv[1], "canopy")) fragment_shader_path = "shaders/fragment_canopy.glsl";
        else return -1;
    }

    // Create and use shader program
    if (create_shader_program(&data.shader_program, fragment_shader_path) == -1)
    {
        return -1;
    }

    printf("[>] Initialization done.\n");
    // main
    while (!glfwWindowShouldClose(data.window)) 
    {
        display(data.window, &data.shader_program, &data.vao, &data.window_data);
    }

    // export
    save_png("export/fractal.png", &data);

    // Cleanup
    glDeleteVertexArrays(1, &data.vao);
    glDeleteBuffers(1, &data.vbo);
    glDeleteProgram(data.shader_program);
    
    glfwTerminate();
    return 0;
}
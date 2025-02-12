// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of MSLYYC_exploration project from .

#include "../include/callbacks.h"
#include "../include/structs.h"

void error_callback(int error, const char* description) 
{
    printf("GLFW Error %d: %s\n", error, description);
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    window_data_t* data = (window_data_t*)glfwGetWindowUserPointer(window);
    // Update window size
    *data->width = width;
    *data->height = height;
    // Update viewport
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    window_data_t* data = (window_data_t*)glfwGetWindowUserPointer(window);

    // Adjust zoom speed by changing the 0.1f value
    float zoom_speed = 0.1f;
    float prev_zoom = data->zoom;
    
    // Calculate zoom
    data->zoom *= (yoffset > 0) ? (1.0f + zoom_speed) : (1.0f / (1.0f + zoom_speed));
    
    // Get mouse position
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    
    // Convert mouse pos to world space before zoom
    float mouse_world_x = (mouse_x / *data->width * 4.0f - 2.0f) / prev_zoom + data->offset[0];
    float mouse_world_y = (mouse_y / *data->height * 4.0f - 2.0f) / prev_zoom + data->offset[1];
    
    // Convert mouse pos to world space after zoom
    float newMouseWorldX = (mouse_x / *data->width * 4.0f - 2.0f) / data->zoom + data->offset[0];
    float newMouseWorldY = (mouse_y / *data->height * 4.0f - 2.0f) / data->zoom + data->offset[1];
    
    // Adjust offset to keep mouse position fixed
    data->offset[0] += mouse_world_x - newMouseWorldX;
    data->offset[1] += mouse_world_y - newMouseWorldY;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    window_data_t* data = (window_data_t*)glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_LEFT) 
    {
        if (action == GLFW_PRESS)
        {
            data->is_dragging = 1;
            glfwGetCursorPos(window, &data->last_x, &data->last_y);
        } 
        else if (action == GLFW_RELEASE) 
        {
            data->is_dragging = 0;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    window_data_t* data = (window_data_t*)glfwGetWindowUserPointer(window);

    if (data->is_dragging) 
    {
        // Convert screen coordinates to world coordinates
        double delta_x = (xpos - data->last_x) / *data->width * 4.0 / data->zoom;
        double delta_y = (ypos - data->last_y) / *data->height * 4.0 / data->zoom;
        
        data->offset[0] -= (float)delta_x;
        data->offset[1] += (float)delta_y;  // Invert Y because screen coordinates are flipped
        
        data->last_x = xpos;
        data->last_y = ypos;
    }
}
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "mainwindow.h"

#define START_WINDOW_WIDTH 1280
#define START_WINDOW_HEIGHT 720

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

// separate function to support calling MainWindow destructor before glfwTerminate
void run() {
    // global mainWindow to allow calling its methods in GLFW callbacks
    mainWindow.initialize(START_WINDOW_WIDTH, START_WINDOW_HEIGHT);

    mainWindow.runMainLoop();

    mainWindow.finish();
}

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    run();

    // double time = glfwGetTime();

    // poll events
    // glfwPollEvents();

    // cleanup
    glfwTerminate();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "realtime.h"

class MainWindow {
public:
    void initialize(int width, int height);
    void finish();
    void resize(int w, int h);
    void handleKeyEvent(int key, int action);
    void handleMouseButtonEvent(int button, int action);
    void handleMouseMoveEvent(double xpos, double ypos);
    std::pair<int, int> getViewportSize() const;
    void close();
    void runMainLoop();
    void makeCurrent();
    void doneCurrent();
private:
    GLFWwindow* m_window;
    std::unique_ptr<Realtime> m_realtime;
    // just so we make sure we don't call runMainLoop twice
    bool m_running = false;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};

// we need this to be global to be able to call its methods in GLFW callbacks
extern MainWindow mainWindow;

#pragma clang diagnostic pop
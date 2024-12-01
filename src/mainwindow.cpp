#include "mainwindow.h"
#include "settings.h"

#define PHYSICS_RATE 60

void MainWindow::initialize(int width, int height) {
    // set minimum OpenGL version to 3.3 (this was an arbitrary decision that seemed reasonable)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfw docs says this is needed for MacOS support i think?
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create window
    m_window = glfwCreateWindow(width, height, "Final Project: Lights, Camera, Explosion!", nullptr, nullptr);
    if (!m_window) {
        // Window or OpenGL context creation failed
        // glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    // set opengl context to the new window
    glfwMakeContextCurrent(m_window);

    // lock cursor to window
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // window callbacks
    glfwSetCursorPosCallback(m_window, MainWindow::cursorPositionCallback);
    glfwSetMouseButtonCallback(m_window, MainWindow::mouseButtonCallback);
    glfwSetKeyCallback(m_window, MainWindow::keyCallback);
    glfwSetFramebufferSizeCallback(m_window, MainWindow::framebufferSizeCallback);

    // TODO better settings initialization
    settings.farPlane = 100.f;
    settings.nearPlane = 0.1f;
    settings.shapeParameter1 = 20;
    settings.shapeParameter2 = 20;
    settings.sceneFilePath = "scenefiles/action/required/movement/chess.json";

    auto [w, h] = getViewportSize();
    m_realtime = std::make_unique<Realtime>(w, h);
    m_realtime->initializeGL();
    // to match projects 5/6 behavior
    m_realtime->resizeGL(w, h);

    glfwMakeContextCurrent(nullptr);
}

void MainWindow::runMainLoop() {
    if (m_running) {
        throw std::runtime_error("Main loop already running");
    }
    m_running = true;

    double timerInterval = 1.0 / (double) PHYSICS_RATE;
    double lastTickTime = glfwGetTime();

    // monitor for window close
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();

        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastTickTime;

        if (elapsedTime >= timerInterval) {
            m_realtime->timerEvent(elapsedTime);
            lastTickTime = currentTime;
        }

        makeCurrent();
        m_realtime->paintGL();

        // will wait for vsync
        glfwSwapBuffers(m_window);
        doneCurrent();
    }
}

void MainWindow::close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void MainWindow::finish() {
    m_realtime->finish();
    m_realtime.reset();
    glfwDestroyWindow(m_window);
}

void MainWindow::resize(int w, int h) {
    makeCurrent();
    m_realtime->resizeGL(w, h);
    doneCurrent();
}

void MainWindow::handleKeyEvent(int key, int action) {
    if (action == GLFW_PRESS) {
        m_realtime->keyPressEvent(key);
    } else if (action == GLFW_RELEASE) {
        m_realtime->keyReleaseEvent(key);
    }
}

void MainWindow::handleMouseButtonEvent(int button, int action) {
    if (action == GLFW_PRESS) {
        m_realtime->mousePressEvent(button);
    } else if (action == GLFW_RELEASE) {
        m_realtime->mouseReleaseEvent(button);
    }
}

void MainWindow::handleMouseMoveEvent(double xpos, double ypos) {
    m_realtime->mouseMoveEvent(xpos, ypos);
}


std::pair<int, int> MainWindow::getViewportSize() const {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {width, height};
}

void MainWindow::makeCurrent() {
    glfwMakeContextCurrent(m_window);
}

void MainWindow::doneCurrent() {
    glfwMakeContextCurrent(nullptr);
}

// static callbacks for GLFW
void MainWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //     glfwSetWindowShouldClose(window, GLFW_TRUE);
    mainWindow.handleKeyEvent(key, action);
}

void MainWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    mainWindow.handleMouseButtonEvent(button, action);
}

void MainWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    mainWindow.resize(width, height);
}

void MainWindow::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    mainWindow.handleMouseMoveEvent(xpos, ypos);
}

MainWindow mainWindow;
#include "realtime.h"
#include "objects/primitivemesh.h"
#include "mainwindow.h"
#include "realtimescene.h"

#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"

#define MOVE_SPEED 5.f
#define ROTATE_SENSITIVITY 0.01f

// ================== Project 5: Lights, Camera

Realtime::Realtime(int w, int h)
        : m_scene(std::nullopt), m_width(w), m_height(h),
          m_param1(settings.shapeParameter1), m_param2(settings.shapeParameter2),
          m_meshes(PrimitiveMesh::initMeshes(settings.shapeParameter1, settings.shapeParameter2))
{
    // m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    // setMouseTracking(true);
    // setFocusPolicy(Qt::StrongFocus);

    // m_keyMap[Qt::Key_W]       = false;
    // m_keyMap[Qt::Key_A]       = false;
    // m_keyMap[Qt::Key_S]       = false;
    // m_keyMap[Qt::Key_D]       = false;
    // m_keyMap[Qt::Key_Control] = false;
    // m_keyMap[Qt::Key_Space]   = false;
}

void Realtime::tryInitScene() {
    // we can't init until the user has selected a scene file and we've been told the dimensions by resizeGL
    if (settings.sceneFilePath.empty() || !m_width.has_value() || !m_height.has_value()) {
        return;
    }

    m_scene = RealtimeScene::init(m_width.value(), m_height.value(), settings.sceneFilePath,
                                  settings.nearPlane, settings.farPlane, m_meshes);
}

bool Realtime::isInited() const {
    return m_scene.has_value();
}

void Realtime::finish() {
    // killTimer(m_timer);
    mainWindow.makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    for (auto& [_, mesh] : m_meshes) {
        mesh->deleteBuffers();
    }

    mainWindow.doneCurrent();
}

void Realtime::initializeGL() {
    // m_devicePixelRatio = this->devicePixelRatio();

    // m_timer = startTimer(1000/60);
    // m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    auto [width, height] = mainWindow.getViewportSize();
    glViewport(0, 0, width, height);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // Set clear color to black
    glClearColor(0,0,0,1);

    // enable vsync
    glfwSwapInterval(1);

    m_shader = ShaderLoader::createShaderProgram("resources/shaders/default.vert", "resources/shaders/default.frag");

    for (auto& [_, mesh] : m_meshes) {
        // my updateBuffers() function makes sure the mesh is allocated before updating
        mesh->updateBuffers();
    }
}

void Realtime::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_queuedBufferUpdate) {
        for (auto& [_, mesh] : m_meshes) {
            mesh->updateBuffers();
        }
        m_queuedBufferUpdate = false;
    }
    // the scene starts uninitialized, we need to check for if the user has selected a scene file yet (i.e. if the scene is initialized)
    if (!isInited()) {
        return;
    }
    // we don't really know when exactly the scene is initialized, so just check if we've passed the shader to
    // the scene every frame--it's cheap
    if (!m_scene->shaderInitialized()) {
        m_scene->initShader(m_shader);
    }

    glUseProgram(m_shader);

    m_scene->paintObjects();

    glUseProgram(0);
}



void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, w, h);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_width = w;
    m_height = h;
    if (isInited()) {
        m_scene->setDimensions(w, h);
    } else {
        tryInitScene();
    }
}

void Realtime::keyPressEvent(int key) {
    m_keyMap[key] = true;
}

void Realtime::keyReleaseEvent(int key) {
    m_keyMap[key] = false;
}

void Realtime::mousePressEvent(int button) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        m_mouseDown = true;
    }
}

void Realtime::mouseReleaseEvent(int button) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(double xpos, double ypos) {
    if (m_mouseDown) {
        int deltaX = xpos - m_prev_mouse_pos.x;
        int deltaY = ypos - m_prev_mouse_pos.y;

        // Use deltaX and deltaY here to rotate (negate them because idk)
        m_scene->rotateCamera(glm::vec3(0.f, 1.f, 0.f), (float) -deltaX * ROTATE_SENSITIVITY);
        m_scene->rotateCamera(glm::normalize(glm::cross(m_scene->cameraLook(), m_scene->cameraUp())), (float) -deltaY * ROTATE_SENSITIVITY);

        // update(); // asks for a PaintGL() call to occur
    }
    m_prev_mouse_pos = glm::dvec2(xpos, ypos);
}

void Realtime::timerEvent(double elapsedSeconds) {
    float deltaTime = (float) elapsedSeconds;

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[GLFW_KEY_W]) {
        m_scene->translateCamera(MOVE_SPEED * deltaTime * m_scene->cameraLook());
    }
    if (m_keyMap[GLFW_KEY_S]) {
        m_scene->translateCamera(-MOVE_SPEED * deltaTime * m_scene->cameraLook());
    }
    if (m_keyMap[GLFW_KEY_A]) {
        m_scene->translateCamera(MOVE_SPEED * deltaTime * glm::normalize(glm::cross(m_scene->cameraUp(), m_scene->cameraLook())));
    }
    if (m_keyMap[GLFW_KEY_D]) {
        m_scene->translateCamera(MOVE_SPEED * deltaTime * glm::normalize(glm::cross(m_scene->cameraLook(), m_scene->cameraUp())));
    }
    if (m_keyMap[GLFW_KEY_SPACE]) {
        m_scene->translateCamera(MOVE_SPEED * deltaTime * glm::vec3(0.f, 1.f, 0.f));
    }
    if (m_keyMap[GLFW_KEY_LEFT_CONTROL] || m_keyMap[GLFW_KEY_RIGHT_CONTROL]) {
        m_scene->translateCamera(MOVE_SPEED * deltaTime * glm::vec3(0.f, -1.f, 0.f));
    }

    // update(); // asks for a PaintGL() call to occur
}

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "shaderloader.h"

#define START_WINDOW_WIDTH 640
#define START_WINDOW_HEIGHT 480

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

typedef struct Vertex
{
    glm::vec2 pos;
    glm::vec3 col;
} Vertex;

static const Vertex vertices[3] =
    {
            { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
            { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
            { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
    };


int main(int argc, char *argv[]) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    // set minimum OpenGL version to 3.3 (this was an arbitrary decision that seemed reasonable)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfw docs says this is needed for MacOS support i think?
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create window
    GLFWwindow* window = glfwCreateWindow(START_WINDOW_WIDTH, START_WINDOW_HEIGHT, "test window", nullptr, nullptr);
    if (!window) {
        // Window or OpenGL context creation failed
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }
    // set opengl context to the new window
    glfwMakeContextCurrent(window);


    // initialize GLEW (code mostly taken from projects 5/6)
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // window callbacks
    glfwSetKeyCallback(window, key_callback);
    // Set clear color to black
    glClearColor(0,0,0,1);
    // glfw uses double buffering, call this to swap buffers
    // glfwSwapBuffers(window);
    glfwSwapInterval(1); // enable vsync

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is TODO does this deal with high DPI screens?
    // int width, height;
    // glfwGetFramebufferSize(window, &width, &height);
    // glViewport(0, 0, width, height);

    // double time = glfwGetTime();

    // poll events
    // glfwPollEvents();

    GLuint program = ShaderLoader::createShaderProgram("resources/shaders/default.vert", "resources/shaders/default.frag");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    // const GLint vpos_location = glGetAttribLocation(program, "vPos");
    // const GLint vcol_location = glGetAttribLocation(program, "vCol");
    const GLint vpos_location = 0;
    const GLint vcol_location = 1;

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));

    // monitor for window close
    // (TODO try using glfwSetWindowCloseCallback; or manually with in-game shortcut with glfwSetWindowShouldClose)
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 m, p, mvp;
        m = glm::mat4(1.0f);
        m = glm::rotate(m, (float) glfwGetTime(), glm::vec3(0.f, 0.f, 1.f));
        p = glm::ortho(-ratio, ratio, -1.f, 1.f, -1.f, 1.f); // Near = -1.f, Far = 1.f
        mvp = p * m;

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
}

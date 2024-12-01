#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int sceneWidth, int sceneHeight, const SceneCameraData& cameraData, float near, float far)
        : m_aspectRatio((float)sceneWidth / (float)sceneHeight), m_heightAngle(cameraData.heightAngle),
          m_pos(cameraData.pos.xyz()),
          m_look(glm::normalize(cameraData.look.xyz())),
          m_up(glm::normalize(cameraData.up.xyz())), m_near(near), m_far(far) {
    m_widthAngle = computeWidthAngle();
    m_viewMatrix = computeViewMatrix();
    m_inverseViewMatrix = glm::inverse(m_viewMatrix);
    m_projectionMatrix = computeProjectionMatrix();
}

void Camera::translate(const glm::vec3& translation) {
    m_pos += translation;
    m_viewMatrix = computeViewMatrix();
    m_inverseViewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::rotate(const glm::vec3& axisWS, float angleRad) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.f), angleRad, axisWS);
    m_look = glm::normalize(rot * glm::vec4(m_look, 0.f));
    m_up = glm::normalize(rot * glm::vec4(m_up, 0.f));
    m_viewMatrix = computeViewMatrix();
    m_inverseViewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::setCameraData(const SceneCameraData& cameraData) {
    m_pos = cameraData.pos;
    m_look = glm::normalize(cameraData.look);
    m_up = glm::normalize(cameraData.up);
    m_heightAngle = cameraData.heightAngle;
    m_widthAngle = computeWidthAngle();

    m_viewMatrix = computeViewMatrix();
    m_inverseViewMatrix = glm::inverse(m_viewMatrix);

    m_projectionMatrix = computeProjectionMatrix();
}


void Camera::setSceneDimensions(int sceneWidth, int sceneHeight) {
    m_aspectRatio = (float)sceneWidth / (float)sceneHeight;
    m_widthAngle = computeWidthAngle();

    m_projectionMatrix = computeProjectionMatrix();
}

void Camera::setNearAndFar(float near, float far) {
    m_near = near;
    m_far = far;

    m_projectionMatrix = computeProjectionMatrix();
}

glm::mat4 Camera::computeViewMatrix() {
    // (u,v,w) are the basis vectors of the camera space *within world space*
    glm::vec3 w = -m_look;
    glm::vec3 v = glm::normalize(m_up - (glm::dot(m_up, w) * w));
    glm::vec3 u = glm::cross(v, w);
    // to send a basis vector of *camera space* to *world space*, you'd multiply by a matrix
    // whose columns are u, v, w; but we want to send the basis vectors of *world space* to *camera space*,
    // so the rot matrix is the inverse of the matrix whose columns are u, v, w.
    // this rotates points in world space to camera space.
    glm::mat4 rot = glm::mat4(u.x, v.x, w.x, 0.f,
                              u.y, v.y, w.y, 0.f,
                              u.z, v.z, w.z, 0.f,
                              0.f, 0.f, 0.f, 1.f);
    // to send points from world space to camera space, we also need to translate by the negation of the camera position
    return rot * glm::translate(glm::mat4(1.f), -m_pos);
}

glm::mat4 Camera::computeProjectionMatrix() const {
    glm::mat4 scale_matrix = glm::mat4(
            1.f / (m_far * tanf(m_widthAngle / 2)), 0.f, 0.f, 0.f,
            0.f, 1.f / (m_far * tanf(m_heightAngle / 2)), 0.f, 0.f,
            0.f, 0.f, 1.f / m_far, 0.f,
            0.f, 0.f, 0.f, 1.f
            );
    float c = -m_near / m_far;
    glm::mat4 unhinge_matrix = glm::mat4(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f / (1 + c), -1.f,
            0.f, 0.f, -c / (1 + c), 0.f
            );
    glm::mat4 opengl_correction_matrix = glm::mat4(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, -2.f, 0.f,
            0.f, 0.f, -1.f, 1.f
            );
    return opengl_correction_matrix * unhinge_matrix * scale_matrix;
}

float Camera::computeWidthAngle() const {
    // using trig, you can figure out the relationship between aspect ratio, height angle, and width angle
    return 2 * glm::atan(m_aspectRatio * glm::tan(m_heightAngle / 2));
}


const glm::mat4& Camera::viewMatrix() const {
    return m_viewMatrix;
}

const glm::mat4& Camera::inverseViewMatrix() const {
    return m_inverseViewMatrix;
}

const glm::mat4& Camera::projectionMatrix() const {
    return m_projectionMatrix;
}

float Camera::aspectRatio() const {
    return m_aspectRatio;
}

float Camera::heightAngle() const {
    return m_heightAngle;
}

float Camera::widthAngle() const {
    return m_widthAngle;
}

const glm::vec3& Camera::pos() const {
    return m_pos;
}

const glm::vec3& Camera::look() const {
    return m_look;
}

const glm::vec3& Camera::up() const {
    return m_up;
}
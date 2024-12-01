#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include <glm/glm.hpp>
#include "utils/scenedata.h"

// A class representing a virtual camera.

class Camera {
public:
    Camera(int sceneWidth, int sceneHeight, const SceneCameraData& cameraData, float near, float far);
    void translate(const glm::vec3& translation);
    void rotate(const glm::vec3& axisWS, float angleRad);
    void setCameraData(const SceneCameraData& cameraData);
    void setSceneDimensions(int sceneWidth, int sceneHeight);
    void setNearAndFar(float near, float far);

    // Returns the view matrix for the current camera settings.
    // (i.e. a matrix that takes points in world space and transforms them to camera space)
    const glm::mat4& viewMatrix() const;

    // Returns the inverse view matrix for the current camera settings.
    // (i.e. a matrix that takes points in camera space and transforms them to world space)
    const glm::mat4& inverseViewMatrix() const;

    // Returns the project matrix for the current camera settings.
    // (i.e. a matrix that takes points in camera space and transforms them to clip space)
    const glm::mat4& projectionMatrix() const;

    // Returns the aspect ratio of the camera.
    float aspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float heightAngle() const;

    // Returns the width angle of the camera in RADIANS.
    float widthAngle() const;

    // Returns the position of the camera in world space.
    const glm::vec3& pos() const;

    // Returns the look vector of the camera in world space.
    const glm::vec3& look() const;

    // Returns the up vector of the camera in world space.
    const glm::vec3& up() const;
private:
    glm::mat4 computeViewMatrix();
    glm::mat4 computeProjectionMatrix() const;
    float computeWidthAngle() const;
    // we can compute all of the values behind the above getters from the SceneCameraData.
    // and since we only ever render one scene with no changing camera settings, we can compute these values once and store them.
    glm::mat4 m_viewMatrix{};
    glm::mat4 m_inverseViewMatrix{};
    glm::mat4 m_projectionMatrix{};
    float m_aspectRatio;
    float m_widthAngle;
    float m_heightAngle;
    glm::vec3 m_pos;
    glm::vec3 m_look;
    glm::vec3 m_up;
    float m_near;
    float m_far;

    static glm::mat4 translateMatrix(const glm::vec3& translation);
    static glm::mat4 rotateMatrix(const glm::vec3& axisWS, float angleRad);
};

#pragma clang diagnostic pop
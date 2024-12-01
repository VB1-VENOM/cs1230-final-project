#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

// A class representing a scene to be rendered in real-time

#include <map>
#include "utils/scenedata.h"
#include "camera.h"
#include "objects/realtimeobject.h"


/// Analogous to RayTraceScene from project 3/4; represents a scene to be rendered in real-time
/// An instance of this class is created each time the scene is changed in the GUI
class RealtimeScene {
public:
    /// Initializes the scene with the given parameters; returns an empty optional if the scene file could not be parsed
    /// or if there are too many lights
    /// Note: the `meshes` map is copied to avoid reference issues; the meshes themselves are not copied
    static std::optional<RealtimeScene> init(int width, int height, const std::string& sceneFilePath,
                                             float nearPlane, float farPlane,
                                             std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes);

    /// Paints every object in the scene; to be called in paintGL
    void paintObjects();

    /// Sets the dimensions of the scene, and updates the camera's info accordingly
    void setDimensions(int width, int height);

    /// Translates the camera by the given amount
    void translateCamera(const glm::vec3& translation);

    /// Rotates the camera by the given amount around the given axis
    void rotateCamera(const glm::vec3& axisWS, float angleRad);

    /// Updates the near and far planes of the scene, and updates the camera's info accordingly
    void updateSettings(float nearPlane, float farPlane);

    /// Passes the shader ID to the scene (can't be done in the constructor because the shader isn't created yet)
    void initShader(GLuint shader);
    /// Returns whether the shader has been initialized
    bool shaderInitialized() const;

    /// Returns the width of the scene
    int width() const;

    /// Returns the height of the scene
    int height() const;

    const glm::vec3& cameraPos() const;
    const glm::vec3& cameraLook() const;
    const glm::vec3& cameraUp() const;
private:
    RealtimeScene(int width, int height, float nearPlane, float farPlane, RenderData renderData,
                  std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes);

    float m_nearPlane;
    float m_farPlane;

    int m_width;
    int m_height;
    SceneGlobalData m_globalData;
    Camera m_camera;
    std::vector<RealtimeObject> m_objects;
    std::vector<SceneLightData> m_lights;
    // need this to not be a reference to avoid C++ issues; so there's just two copies of this map at all times, oh welllll
    // (it's fineeee, the meshes themselves aren't copied)
    std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> m_meshes;
    std::optional<GLuint> m_shader;

    // helper functions for passing uniforms to the shader (and checking for -1 locations)
    void passUniformMat4(const char* name, const glm::mat4& mat);
    void passUniformMat3(const char* name, const glm::mat3& mat);
    void passUniformFloat(const char* name, float value);
    void passUniformInt(const char* name, int value);
    void passUniformVec3(const char* name, const glm::vec3& vec);
    void passUniformVec3Array(const char* name, const std::vector<glm::vec3>& vecs);
    void passUniformLightArray(const char* name, const std::vector<SceneLightData>& lights);
    void passUniformLight(const char* name, SceneLightData type);

    GLint getUniformLocation(const char* name, bool checkValidLoc = true) const;

    /// convert enum class LightType to the corresponding uniform value
    static GLuint lightTypeToUniform(LightType type);
};

#pragma clang diagnostic pop
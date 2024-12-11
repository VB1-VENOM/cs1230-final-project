#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

// A class representing a scene to be rendered in real-time

#include <map>
#include <ranges>

#include "utils/scenedata.h"
#include "camera.h"
#include "objects/realtimeobject.h"
#include "objects/collisionobject.h"
#include "objects/playerobject.h"

#include <unordered_set>
#define GRACE_PERIOD_MS 10000
#define TIME_BETWEEN_SPAWNS_MS 5000
#define PROBABILITY_OF_SPAWN 0.3
#define TIME_TO_INCREMENT_SPAWN_S 15
#define INCREMENT 0.1 //for probability of spawn


struct pair_hash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        std::size_t h1 = std::hash<T1>{}(pair.first);
        std::size_t h2 = std::hash<T2>{}(pair.second);
        return h1 ^ (h2 << 1); // Combine hashes using XOR and bit shifting
    }
};

//stuff required to make hashmaps work for vec2; we need both a way to check equality and a way to actually hash
//we do this so we can store spawn locations as a hash from vec2's (grid) to vec3's (xyz)
struct Vec2Hash {
    std::size_t operator()(const glm::vec2& v) const {
        return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1); //bitwise shift to add randomness
    }
};
struct Vec2Equal {
    bool operator()(const glm::vec2& lhs, const glm::vec2& rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

#include "objects/enemyobject.h"


/// Analogous to RayTraceScene from project 3/4; represents a scene to be rendered in real-time
/// An instance of this class is created each time the scene is changed in the GUI
class RealtimeScene : public std::enable_shared_from_this<RealtimeScene> {
public:

    /// Initializes the scene with the given parameters; returns an empty shared_ptr if the scene file could not be parsed
    /// or if there are too many lights
    /// Note: the `meshes` map is copied to avoid reference issues; the meshes themselves are not copied
    static std::shared_ptr<RealtimeScene> init(int width, int height, const std::string& sceneFilePath,
                                             float nearPlane, float farPlane,
                                             std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes, std::shared_ptr<bool> taken_damage);

    /// Paints every object in the scene; to be called in paintGL
    void paintObjects();
    void generateProceduralCity(int gridX, int gridZ, int rows, int cols, float spacing) ;
    void spawnEnemiesInGrids();

    /// Convenience method for constructing and adding a new object to the scene
    /// Returns a shared_ptr to the object.
    std::shared_ptr<RealtimeObject> addObject(PrimitiveType type, const glm::mat4& ctm, const SceneMaterial& material, RealtimeObjectType objType);
    /// Insert new object into the scene.
    /// `object` should be a RealtimeObject or a subclass of RealtimeObject.
    /// If `object` is a subclass of CollisionObject, it will also be added to the collision objects list.
    /// Returns a shared_ptr to the object.
    std::shared_ptr<RealtimeObject> addObject(std::unique_ptr<RealtimeObject> object);

    /// Called every physics tick
    void tick(double elapsedSeconds);

    /// Sets the dimensions of the scene, and updates the camera's info accordingly
    void setDimensions(int width, int height);

    /// Updates the near and far planes of the scene, and updates the camera's info accordingly
    void updateSettings(float nearPlane, float farPlane);

    /// Passes the shader ID to the scene (can't be done in the constructor because the shader isn't created yet)
    void initShader(GLuint phongShader);
    /// Returns whether the shader has been initialized
    bool shaderInitialized() const;

    /// Returns the width of the scene
    int width() const;

    /// Returns the height of the scene
    int height() const;

    /// Returns the meshes map of the scene
    const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes() const;

    // Returns the collision objects list of the scene
    const std::vector<std::weak_ptr<CollisionObject>>& collisionObjects() const;
    std::unordered_set<std::pair<int, int>, pair_hash> existingBuildings;
    void removeGridObjects(int gridX, int gridZ, int rows, int cols);

    // input events methods; currently called manually by realtime (ideally we'd have some callback system or something for this)
    float m_nearPlane;
    float m_farPlane;
    static std::unordered_set<std::pair<int, int>, pair_hash> m_activeGrids;
    //std::vector<pair<int,int>> m_usedGrid;


    RenderData m_renderData;
    void keyPressEvent(int key);
    void keyReleaseEvent(int key);
    void mousePressEvent(int button);
    void mouseReleaseEvent(int button);
    void mouseMoveEvent(double xpos, double ypos);

    std::shared_ptr <RealtimeObject> addBuilding(const glm::vec3& position);
    void updateDynamicCity(const glm::vec3& playerPosition, int gridCellUpdateDist);
    //std::shared_ptr<RealtimeScene> generateProceduralCity(int cityWidth, int cityDepth, int blockSize);
    // TODO there might be a smarter way to store these... maybe a map from id to object?
    //      I feel like we also need some sort of callback system to register objects that want to listen for input, etc
    std::vector<std::shared_ptr<RealtimeObject>> m_objects;
    // if we want to pass this to the objects themselves, we need weak_ptrs to avoid circular reference issues
    std::vector<std::weak_ptr<CollisionObject>> m_collisionObjects;
    std::shared_ptr<PlayerObject> m_playerObject;
    //std::pair<int, int> gridCoord;


    void finish();

private:
    RealtimeScene(int width, int height, float nearPlane, float farPlane, SceneGlobalData globalData, SceneCameraData cameraData,
                  std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes);

    //float m_nearPlane;
    //float m_farPlane;

    int m_width;
    int m_height;
    SceneGlobalData m_globalData;
    std::shared_ptr<Camera> m_camera;


    std::shared_ptr<std::vector<SceneLightData>> m_lights;
    // need this to not be a reference to avoid C++ issues; so there's just two copies of this map at all times, oh welllll
    // (it's fineeee, the meshes themselves aren't copied)
    std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> m_meshes;
    std::optional<GLuint> m_phongShader;

    std::shared_ptr<bool> m_taken_damage;

    // helper functions for passing uniforms to the shader (and checking for -1 locations)
    void passUniformMat4(const char* name, const glm::mat4& mat);
    void passUniformMat3(const char* name, const glm::mat3& mat);
    void passUniformFloat(const char* name, float value);
    void passUniformInt(const char* name, int value);
    void passUniformVec3(const char* name, const glm::vec3& vec);
    void passUniformVec3Array(const char* name, const std::vector<glm::vec3>& vecs);
    void passUniformLightArray(const char* name, std::shared_ptr<std::vector<SceneLightData>> lights);
    void passUniformLight(const char* name, SceneLightData type);

    GLint getUniformLocation(const char* name, bool checkValidLoc = true) const;

    /// convert enum class LightType to the corresponding uniform value
    static GLuint lightTypeToUniform(LightType type);

    //Skybox stuff
    std::shared_ptr<RealtimeObject> m_skyboxObject;
    GLuint m_skyboxTextureID;
    // GLuint loadCubemap(std::vector<std::string> faces);
    std::optional<GLuint> m_skyboxShader;



    // Helper function for initializing enemies
    std::vector<std::shared_ptr<EnemyObject>> createEnemies(std::vector<glm::vec3> enemy_positions,
        std::shared_ptr<RealtimeScene> scene);

    void addEnemy(glm::vec3 position);

    //grace period for when you spawn in
    std::chrono::time_point<std::chrono::steady_clock> m_enemy_spawn_start;

    std::chrono::time_point<std::chrono::steady_clock> m_time_last_spawn;

    //hash map from grid x, z to xyz to spawn enemy at
    std::unordered_map<glm::vec2,glm::vec3, Vec2Hash, Vec2Equal> m_enemy_spawn_locations;

    int current_difficulty_scaling = 0;
    std::chrono::time_point<std::chrono::steady_clock> m_time_last_scaling;
};



#pragma clang diagnostic pop

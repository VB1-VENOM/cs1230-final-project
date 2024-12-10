#pragma once

#include <unordered_map>
#include "collisionobject.h"
#include "camera.h"

// TODO play with defaults
#define DEFAULT_PLAYER_GRAVITY 10.f
#define PLAYER_TERMINAL_VELOCITY 100.f
#define PLAYER_MOVE_ACCEL 20.f
#define PLAYER_MAX_HORIZ_SPEED 5.f
#define PLAYER_FRICTION_ACCEL 30.f
#define PLAYER_JUMP_SPEED 150.f
#define ROTATE_SENSITIVITY 0.005f
#define EPSILON 0.0001f
#define PLAYER_MOVE_ACCEL_WITH_FRICTION (PLAYER_MOVE_ACCEL + PLAYER_FRICTION_ACCEL)

class PlayerObject : public CollisionObject {
public:
    PlayerObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene,
                 std::shared_ptr<Camera> camera, std::shared_ptr<std::vector<SceneLightData>> lights);
    void tick(double elapsedSeconds) override;
    /// Moves the player and camera
    void translate(const glm::vec3& translation) override;
    // input events methods; currently called manually by realtimescene;
    // ideally we'd have some callback system or something for this
    void keyPressEvent(int key);
    void keyReleaseEvent(int key);
    void mousePressEvent(int button);
    void mouseReleaseEvent(int button);
    void mouseMoveEvent(double xpos, double ypos);
    // glm::vec2 m_cityStart = glm::vec2(0, 0); // Starting corner of the city grid
    // glm::vec2 m_cityEnd = glm::vec2(10, 10); // Current end of the city grid (10x10 grid)
    // float m_citySpacing = 5.0f; // Spacing between buildings (same as used in generateProceduralCity)
    // int m_cityRows = 10; // Number of rows in each grid section
    // int m_cityCols = 10; // Number of columns in each grid section
    std::vector<std::shared_ptr<RealtimeObject>> m_cityObjects; // Track city objects
    void expandCity(const glm::vec2& direction);
    void expandCityWithProceduralGeneration(const glm::vec2& direction);
    void removeOutOfBoundsObjects();
    void removeCityEdge(const glm::vec2& direction);
    glm::vec2 m_cityStart = glm::vec2(0, 0); // Grid start (bottom-left corner)
    glm::vec2 m_cityEnd = glm::vec2(10, 10); // Grid end (top-right corner)
    float m_citySpacing = 5.0f; // Spacing between buildings
    int m_cityRows = 10; // Number of rows in the grid
    int m_cityCols = 10; // Number of columns in the grid
    struct Vec2Comparator {
        bool operator()(const glm::vec2& lhs, const glm::vec2& rhs) const {
            if (lhs.x != rhs.x) {
                return lhs.x < rhs.x;
            }
            return lhs.y < rhs.y;
        }
    };

    std::map<glm::vec2, std::shared_ptr<RealtimeObject>, Vec2Comparator> m_cityGrid;
     // Map of grid positions to objects


private:
    std::shared_ptr<std::vector<SceneLightData>> m_lights;
    bool m_flashLightOn = true;
    std::optional<SceneLightData> m_savedLight;

    // we could avoid storing a new keymap/etc for each object, but i found this the simplest way of designing things
    std::unordered_map<int, bool> m_keyMap;
    std::unordered_map<int, bool> m_mouseButtonMap;
    std::optional<glm::dvec2> m_prev_mouse_pos;

    std::shared_ptr<Camera> m_camera;
    float m_gravity = DEFAULT_PLAYER_GRAVITY;
    glm::vec3 m_velocity = glm::vec3(0.f);
    bool m_onGround = false;

    // java-like super
    typedef CollisionObject super;


    void spawnBullet();
};


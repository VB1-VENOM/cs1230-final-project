#pragma once

#include "collisionobject.h"
#include "camera.h"
#include <unordered_map>

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
    PlayerObject(const RenderShapeData& data, const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes,
                 std::shared_ptr<std::vector<std::weak_ptr<CollisionObject>>> collisionObjects,
                 std::shared_ptr<Camera> camera);
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

    bool shouldRender() const override;
private:
    // TODO we could avoid storing a new keymap/etc for each object, but i found this the simplest way of designing things
    std::unordered_map<int, bool> m_keyMap;
    std::optional<glm::dvec2> m_prev_mouse_pos;

    std::shared_ptr<Camera> m_camera;
    float m_gravity = DEFAULT_PLAYER_GRAVITY;
    glm::vec3 m_velocity = glm::vec3(0.f);
    bool m_onGround = false;

    // java-like super
    typedef CollisionObject super;
};
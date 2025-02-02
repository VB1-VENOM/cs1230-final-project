#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "enemyobject.h"
#include "realtimescene.h"
#include "projectileobject.h"
#include <glm/gtx/string_cast.hpp>
#include <utility>
#include "material_constants/enemy_materials.h"


EnemyObject::EnemyObject(RenderShapeData& data,
                         const std::shared_ptr<RealtimeScene>& scene,
                         std::shared_ptr<Camera> camera, std::shared_ptr<bool> taken_damage)
    : CollisionObject(data, scene), m_renderShapeData(data)
{
    m_taken_damage = taken_damage;
    // enemy should render by default
    m_camera = std::move(camera);
    setShouldRender(true);
}

void EnemyObject::translate(const glm::vec3& translation) {
    super::translate(translation);
}

void EnemyObject::tick(double elapsedSeconds) {
    //determine where the player is relative to the enemy.
    //does not take into account the y component.
    glm::vec3 direction_to_player = glm::normalize(glm::vec3(m_camera->pos().x, 0.f, m_camera->pos().z)
                                                   - glm::vec3(pos().x, 0.f, pos().z));

    //if enemy is more than 50 away despawn
    if (glm::length(glm::vec3(m_camera->pos().x, 0.f, m_camera->pos().z)
                                                   - glm::vec3(pos().x, 0.f, pos().z)) > 50)
    {
        queueFree();
    }

    glm::vec3 enemy2DVelocity = direction_to_player * ENEMY_SPEED;
    m_velocity.x = enemy2DVelocity.x;
    m_velocity.z = enemy2DVelocity.z;


    // Basic physics/collision; COPIED FROM playerobject.cpp!!!

    super::tick(elapsedSeconds);

    float deltaTime = (float) elapsedSeconds;
    glm::vec3 accelVec = glm::vec3(0.f);


    if (!m_onGround) {
        m_velocity.y -= m_gravity * deltaTime;
    }
    // std::cout << glm::to_string(m_velocity) << std::endl;

    glm::vec3 translation = m_velocity * deltaTime;
    auto collisionInfoOpt = getCollisionInfo(translation);

    // reset velocity in direction of collision
    // TODO this method of doing this allow the player to cling to walls by moving into them
    // TODO fix duplicate collisions upon landing on the ground
    if (collisionInfoOpt.has_value()) {
        glm::vec3 collisionMovementDir = collisionInfoOpt->collisionCorrectionVec;
        glm::vec3 projOfVelOnCollisionMovementDir =
                m_velocity * (glm::dot(glm::normalize(m_velocity), collisionMovementDir));
        m_velocity += projOfVelOnCollisionMovementDir;
        if (collisionMovementDir.y > 0.f) {
            m_onGround = true;
        }
        translation += collisionInfoOpt->collisionCorrectionVec;

        //if we collide with the player
        for (const std::shared_ptr<CollisionObject>& obj : collisionInfoOpt->objects) {
            if (std::shared_ptr<PlayerObject> player = std::dynamic_pointer_cast<PlayerObject>(obj)) {
                *m_taken_damage = true;
            }
        }

    }
    // reset onGround if we are not on the ground
    if (m_onGround && !getCollisionInfo(glm::vec3(0.f, -EPSILON, 0.f)).has_value()) {
        m_onGround = false;
    }


    //reset the way that the damaged enemies look
    if (std::chrono::steady_clock::now() > damage_end_time && health > 0) {
        setMaterial(enemy_materials::enemyMaterial1);
    }

    translate(translation);
}

//called in ProjectileObject
void EnemyObject::onShot() {
    health -= 1;
    if (health <= 0)
    {
        queueFree();
        return;
    }
    damage_end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(ON_ENEMY_HIT_FLASH_MS);
    setMaterial(enemy_materials::damagedEnemyMaterial1);
}

#pragma clang diagnostic pop

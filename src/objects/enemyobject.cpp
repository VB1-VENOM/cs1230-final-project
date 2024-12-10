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


EnemyObject::EnemyObject(const RenderShapeData& data,
                         const std::shared_ptr<RealtimeScene>& scene,
                         std::shared_ptr<Camera> camera)
        : CollisionObject(data, scene) {
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

        //has the enemy been shot?
        for (const std::shared_ptr<CollisionObject>& obj : collisionInfoOpt->objects) {
            if (dynamic_pointer_cast<ProjectileObject>(obj)) {
                std::cout << "shot " << std::endl;
                queueFree(); //delete self
                break;
            }
        }
    }
    // reset onGround if we are not on the ground
    if (m_onGround && !getCollisionInfo(glm::vec3(0.f, -EPSILON, 0.f)).has_value()) {
        m_onGround = false;
    }



    translate(translation);
}

#pragma clang diagnostic pop

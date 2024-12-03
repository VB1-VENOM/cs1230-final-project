#include <optional>
#include <iostream>
#include "realtimescene.h"
#include "objects/realtimeobject.h"
#include "objects/staticobject.h"
#include "glm/ext/matrix_transform.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantParameter"
#define MAX_LIGHTS 8

const unsigned int SHADER_LIGHT_POINT       = 0x1u;
const unsigned int SHADER_LIGHT_DIRECTIONAL = 0x2u;
const unsigned int SHADER_LIGHT_SPOT        = 0x4u;

std::optional<RealtimeScene> RealtimeScene::init(int width, int height, const std::string& sceneFilePath,
                                                 float nearPlane, float farPlane,
                                                 std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes) {
    RenderData renderData;
    if (!SceneParser::parse(sceneFilePath, renderData)) {
        std::cerr << "Failed to initialize scene: failed to parse scene file" << std::endl;
        return std::nullopt;
    }
    if (renderData.lights.size() > MAX_LIGHTS) {
        std::cerr << "Failed to initialize scene: too many lights" << std::endl;
        return std::nullopt;
    }
    return RealtimeScene(width, height, nearPlane, farPlane, std::move(renderData), std::move(meshes));
}

RealtimeScene::RealtimeScene(int width, int height, float nearPlane, float farPlane, RenderData renderData,
                             std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes) :
m_width(width), m_height(height), m_globalData(renderData.globalData),
m_camera(std::make_shared<Camera>(width, height, renderData.cameraData, nearPlane, farPlane)),
m_nearPlane(nearPlane), m_farPlane(farPlane),
m_meshes(std::move(meshes)),
m_collisionObjects(std::make_shared<std::vector<std::weak_ptr<CollisionObject>>>(0)) {
    // TODO currently all objects imported from the scene file will be static collidable objects.
    //  ideally we'd extend the scene format to allow it to specify what physics properties an object has
    m_objects.reserve(renderData.shapes.size() + 1);
    m_collisionObjects->reserve(renderData.shapes.size() + 1);
    for (const auto& shape : renderData.shapes) {
        auto staticObject = std::make_shared<StaticObject>(shape, m_meshes, m_collisionObjects);
        auto collisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(staticObject));
        auto object = std::static_pointer_cast<RealtimeObject>(staticObject);
        m_objects.push_back(object);
        m_collisionObjects->push_back(collisionObject);
    }
    // create player object TODO better way of doing this
    ScenePrimitive playerPrimitive = {.type = PrimitiveType::PRIMITIVE_CUBE};
    // start player scaled up by 1 (i.e. 1 unit wide); start player centered at camera
    glm::mat4 playerCTM = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.f)), m_camera->pos());
    RenderShapeData playerShapeData = {.primitive = playerPrimitive, .ctm = playerCTM};
    m_playerObject = std::make_shared<PlayerObject>(playerShapeData, m_meshes, m_collisionObjects, m_camera);
    auto playerCollisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(m_playerObject));
    auto playerRealtimeObject = std::static_pointer_cast<RealtimeObject>(m_playerObject);
    m_objects.push_back(playerRealtimeObject);
    m_collisionObjects->push_back(playerCollisionObject);

    m_lights.reserve(MAX_LIGHTS);
    for (const auto& light : renderData.lights) {
        // normalizing is important (and faster to do here than on the gpu for every fragment)
        m_lights.emplace_back(light.id, light.type, light.color, light.function, light.pos, glm::normalize(light.dir),
                              light.penumbra, light.angle, light.width, light.height);
    }
}

void RealtimeScene::tick(double elapsedSeconds) {
    for (auto& object : m_objects) {
        object->tick(elapsedSeconds);
    }
}

void RealtimeScene::paintObjects() {
    if (!shaderInitialized()) {
        std::cerr << "Failed to paint objects: shader not initialized" << std::endl;
        return;
    }

    passUniformMat4("view", m_camera->viewMatrix());
    passUniformMat4("proj", m_camera->projectionMatrix());
    passUniformInt("numLights", (int) m_lights.size());
    passUniformLightArray("lights", m_lights);
    passUniformVec3("cameraPosWS", m_camera->pos().xyz());
    passUniformFloat("ka", m_globalData.ka);
    passUniformFloat("kd", m_globalData.kd);
    passUniformFloat("ks", m_globalData.ks);
    for (const auto& object : m_objects) {
        if (!object->shouldRender()) {
            continue;
        }
        const SceneMaterial& material = object->material();
        passUniformMat4("model", object->CTM());
        passUniformMat3("inverseTransposeModel", object->inverseTransposeCTM());
        passUniformVec3("cAmbient", material.cAmbient.xyz());
        passUniformVec3("cDiffuse", material.cDiffuse.xyz());
        passUniformVec3("cSpecular", material.cSpecular.xyz());
        passUniformFloat("shininess", material.shininess);
        glBindVertexArray(object->mesh()->vao());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (object->mesh()->vertexData().size() / 3));
        glBindVertexArray(0);
    }
}

void RealtimeScene::passUniformMat4(const char* name, const glm::mat4& mat) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void RealtimeScene::passUniformMat3(const char* name, const glm::mat3& mat) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void RealtimeScene::passUniformFloat(const char* name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void RealtimeScene::passUniformInt(const char* name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void RealtimeScene::passUniformVec3(const char* name, const glm::vec3& vec) {
    glUniform3fv(getUniformLocation(name), 1, &vec[0]);
}

void RealtimeScene::passUniformVec3Array(const char* name, const std::vector<glm::vec3>& vecs) {
    glUniform3fv(getUniformLocation(name), (GLint) vecs.size(), &vecs[0][0]);
}

void RealtimeScene::passUniformLightArray(const char* name, const std::vector<SceneLightData>& lights) {
    // hmm this will cause allocations every time TODO maybe optimize
    std::string lightName = std::string(name);
    for (int i = 0; i < lights.size(); i++) {
        passUniformLight((lightName + "[" + std::to_string(i) + "]").c_str(), lights[i]);
    }
}

void RealtimeScene::passUniformLight(const char* name, SceneLightData type) {
    // hmm this will cause allocations every time TODO maybe optimize
    std::string lightName = std::string(name);
    glUniform1ui(getUniformLocation((lightName + ".type").c_str()), lightTypeToUniform(type.type));
    glUniform3fv(getUniformLocation((lightName + ".color").c_str()), 1, &type.color.xyz()[0]);
    glUniform3fv(getUniformLocation((lightName + ".pos").c_str()), 1, &type.pos.xyz()[0]);
    glUniform3fv(getUniformLocation((lightName + ".dir").c_str()), 1, &type.dir.xyz()[0]);
    glUniform3fv(getUniformLocation((lightName + ".function").c_str()), 1, &type.function[0]);
    glUniform1f(getUniformLocation((lightName + ".penumbra").c_str()), type.penumbra);
    glUniform1f(getUniformLocation((lightName + ".angle").c_str()), type.angle);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantConditionsOC"
GLint RealtimeScene::getUniformLocation(const char* name, bool checkValidLoc) const {
    GLint loc = glGetUniformLocation(*m_shader, name);
    if (checkValidLoc && loc == -1) {
        std::cerr << "Could not find uniform " << name << " in shader program" << std::endl;
    }
    return loc;
}
#pragma clang diagnostic pop

void RealtimeScene::setDimensions(int width, int height) {
    m_width = width;
    m_height = height;
    m_camera->setSceneDimensions(width, height);
}


void RealtimeScene::updateSettings(float nearPlane, float farPlane) {
    if (farPlane != m_farPlane || nearPlane != m_nearPlane) {
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        m_camera->setNearAndFar(nearPlane, farPlane);
    }
}

void RealtimeScene::initShader(GLuint shader) {
    m_shader = shader;
}

bool RealtimeScene::shaderInitialized() const {
    return m_shader.has_value();
}


int RealtimeScene::width() const {
    return m_width;
}

int RealtimeScene::height() const {
    return m_height;
}

GLuint RealtimeScene::lightTypeToUniform(LightType type) {
    switch (type) {
        case LightType::LIGHT_POINT:
            return SHADER_LIGHT_POINT;
        case LightType::LIGHT_DIRECTIONAL:
            return SHADER_LIGHT_DIRECTIONAL;
        case LightType::LIGHT_SPOT:
            return SHADER_LIGHT_SPOT;
        default:
            throw std::runtime_error("Invalid light type");
    }
}

void RealtimeScene::keyPressEvent(int key) {
    m_playerObject->keyPressEvent(key);
}

void RealtimeScene::keyReleaseEvent(int key) {
    m_playerObject->keyReleaseEvent(key);
}

void RealtimeScene::mousePressEvent(int button) {
    m_playerObject->mousePressEvent(button);
}

void RealtimeScene::mouseReleaseEvent(int button) {
    m_playerObject->mouseReleaseEvent(button);
}

void RealtimeScene::mouseMoveEvent(double xpos, double ypos) {
    m_playerObject->mouseMoveEvent(xpos, ypos);
}

#pragma clang diagnostic pop
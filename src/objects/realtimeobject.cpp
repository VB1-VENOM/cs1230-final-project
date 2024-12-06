#include "glm/ext/matrix_transform.hpp"
#include "realtimeobject.h"
#include "realtimescene.h"

RealtimeObject::RealtimeObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene) :
m_mesh(scene->meshes().at(data.primitive.type)), m_ctm(data.ctm),
m_inverseOfTranspose3x3CTM(glm::inverse(glm::transpose(glm::mat3(data.ctm)))),
m_material(data.primitive.material), m_type(data.primitive.type), m_shouldRender(true), m_scene(std::weak_ptr(scene)),
m_queuedFree(false) {}

void RealtimeObject::translate(const glm::vec3& translation) {
    m_ctm = glm::translate(m_ctm, translation);
    m_inverseOfTranspose3x3CTM = glm::inverse(glm::transpose(glm::mat3(m_ctm)));
}

// default physics tick does nothing
void RealtimeObject::tick(double elapsedSeconds) {}

glm::vec3 RealtimeObject::pos() const {
    // get position from last column of CTM
    // TODO this always works right?
    return {m_ctm[3]};
}

const std::shared_ptr<PrimitiveMesh>& RealtimeObject::mesh() const {
    return m_mesh;
}

const glm::mat4& RealtimeObject::CTM() const {
    return m_ctm;
}

const glm::mat3& RealtimeObject::inverseTransposeCTM() const {
    return m_inverseOfTranspose3x3CTM;
}

const SceneMaterial& RealtimeObject::material() const {
    return m_material;
}

PrimitiveType RealtimeObject::type() const {
    return m_type;
}

bool RealtimeObject::shouldRender() const {
    return m_shouldRender;
}

void RealtimeObject::setShouldRender(bool shouldRender) {
    m_shouldRender = shouldRender;
}

std::shared_ptr<RealtimeScene> RealtimeObject::scene() const {
    return m_scene.lock();
}

void RealtimeObject::queueFree() {
    m_queuedFree = true;
}

bool RealtimeObject::isQueuedFree() const {
    return m_queuedFree;
}

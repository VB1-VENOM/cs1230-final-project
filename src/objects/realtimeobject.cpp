#include "realtimeobject.h"

RealtimeObject::RealtimeObject(const RenderShapeData& data, const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes) :
m_mesh(meshes.at(data.primitive.type)), m_ctm(data.ctm), m_inverseCTM(glm::inverse(data.ctm)),
m_inverseOfTranspose3x3CTM(glm::inverse(glm::transpose(glm::mat3(data.ctm)))),
m_material(data.primitive.material), m_type(data.primitive.type) {}

const std::shared_ptr<PrimitiveMesh>& RealtimeObject::mesh() const {
    return m_mesh;
}

const glm::mat4& RealtimeObject::CTM() const {
    return m_ctm;
}

const glm::mat4& RealtimeObject::inverseCTM() const {
    return m_inverseCTM;
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
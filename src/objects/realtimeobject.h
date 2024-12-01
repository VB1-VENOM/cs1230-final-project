#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include <memory>
#include <map>
#include "utils/sceneparser.h"
#include "primitivemesh.h"

/// Represents a single object in the scene, with a transformation matrix, material, type, and pointer to the mesh for that type
/// Just used as a container for the data that the renderer needs to draw the object; all methods are just getters
class RealtimeObject {
public:
    RealtimeObject(const RenderShapeData& data, const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes);
    const std::shared_ptr<PrimitiveMesh>& mesh() const;
    const glm::mat4& CTM() const;
    const glm::mat4& inverseCTM() const;
    const glm::mat3& inverseTransposeCTM() const;
    const SceneMaterial& material() const;
    PrimitiveType type() const;
private:
    std::shared_ptr<PrimitiveMesh> m_mesh;
    glm::mat4 m_ctm;
    glm::mat4 m_inverseCTM;
    glm::mat3 m_inverseOfTranspose3x3CTM;
    SceneMaterial m_material;
    PrimitiveType m_type;
};


#pragma clang diagnostic pop
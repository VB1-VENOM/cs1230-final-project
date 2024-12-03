#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include <memory>
#include <map>
#include "utils/sceneparser.h"
#include "meshes/primitivemesh.h"
#include "aabb.h"

// class hierarchy (could be split up more if we want more modularity)
// RealtimeObject -> CollisionObject
// CollisionObject -> {StaticObject, PlayerObject, BulletObject}

/// Represents a single object in the scene, with a transformation matrix, material, type, and pointer to the mesh for that type
/// Base RealtimeObject does not have collision.
class RealtimeObject {
public:
    RealtimeObject(const RenderShapeData& data, const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes);

    /// called every physics tick
    virtual void tick(double elapsedSeconds);

    /// Translates the object by the given vector
    virtual void translate(const glm::vec3& translation);

    // getters
    glm::vec3 pos() const;
    const std::shared_ptr<PrimitiveMesh>& mesh() const;
    const glm::mat4& CTM() const;
    const glm::mat3& inverseTransposeCTM() const;
    const SceneMaterial& material() const;
    PrimitiveType type() const;
    virtual bool shouldRender() const;
private:
    std::shared_ptr<PrimitiveMesh> m_mesh;
    glm::mat4 m_ctm;
    glm::mat3 m_inverseOfTranspose3x3CTM;
    SceneMaterial m_material;
    PrimitiveType m_type;
};


#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include <memory>
#include <map>
#include "utils/sceneparser.h"
#include "meshes/primitivemesh.h"
#include "aabb.h"
#include "utils/imagereader.h"

/// only used as a convenience for the factory function in RealtimeScene
enum class RealtimeObjectType {
    OBJECT,
    STATIC
};

// class hierarchy (could be split up more if we want more modularity)
// RealtimeObject -> CollisionObject
// CollisionObject -> {StaticObject, PlayerObject, BulletObject}

class RealtimeScene;

/// Represents a single object in the scene, with a transformation matrix, material, type, and pointer to the mesh for that type
/// Base RealtimeObject does not have collision.
class RealtimeObject {
public:
    RealtimeObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene);

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

    void setShouldRender(bool shouldRender);
    bool shouldRender() const;

    void queueFree();
    bool isQueuedFree() const;

    std::shared_ptr<RealtimeScene> scene() const;

    /** Returns whether this object uses a texture; that is, if its material's textureMap has isUsed set,
     * and if the texture file path was successfully loaded as an image.
     * @return true if this object uses a texture, false otherwise
     */
    bool usesTexture() const;
    void setMaterial(SceneMaterial& material);

    bool glTexAllocated() const;

    void allocateGLTex();

    void setTexture(GLuint textureID);

    GLuint glTexID() const;


    void finish();
private:
    std::weak_ptr<RealtimeScene> m_scene;
    bool m_shouldRender;
    bool m_queuedFree;
    std::shared_ptr<PrimitiveMesh> m_mesh;
    glm::mat4 m_ctm;
    glm::mat3 m_inverseOfTranspose3x3CTM;
    SceneMaterial m_material;
    PrimitiveType m_type;
    /// nullptr if this object does not use a texture
    std::unique_ptr<Image> m_texture;
    GLuint m_glTexID;
    bool m_glTexAllocated = false;
};


#pragma clang diagnostic pop
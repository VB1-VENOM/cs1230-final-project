#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include "glew/include/GL/glew.h"
#include "glm/glm.hpp"
#include <memory>
#include <map>
#include "utils/sceneparser.h"
#include "aabb.h"

/// Base class for all mesh objects; used to tessellate each primitive shape and generate the respective vertex data
class PrimitiveMesh {
public:
    /// Creates one of each primitive mesh type with the given parameters
    static std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> initMeshes(int param1, int param2);
    /// Sets the parameters for the mesh and regenerates the vertex data accordingly
    void setParams(int param1, int param2);
    /// Sets up vao and vbo (allocating if they have not been already).
    /// On first call, will also generate the vertex data via generateVertexData();
    /// On subsequent calls, it assumes `generateVertexData()` has already been called as of the last `setParams()`
    void updateBuffers();
    /// Deletes the vao and vbo using glDeleteBuffers and glDeleteVertexArrays
    void deleteBuffers();
    /// Returns the vao of the mesh
    GLuint vao() const;
    /// Returns the vbo of the mesh
    GLuint vbo() const;
    /// Returns the vertex data of the mesh
    const std::vector<float>& vertexData() const;
    /// Computes the AABB of the mesh in world space, given the CTM
    /// Default implementation uses the mesh bounds; optionally can be overridden in subclasses to use the ideal object bounds
    virtual AABB computeAABB(const glm::mat4& ctm) const;
protected:
    /// Generic constructor for a primitive mesh; called by subclasses' constructors
    PrimitiveMesh(int param1, int param2);
    // different shapes will have different min values of these parameters
    /// Returns the minimum valid value for param1 for the shape (so that the shape doesn't disappear on too low of a value)
    virtual int getMinParam1() const = 0;
    /// Returns the minimum valid value for param2 for the shape (so that the shape doesn't disappear on too low of a value)
    virtual int getMinParam2() const = 0;
    /// Returns the expected size of the vertex data vector given the current parameters,
    /// based on the number of triangles that should be generated for the shape (used to reserve space in the vector)
    virtual int getExpectedVectorSize() = 0;
    /// Uses lab 8's tessellation process to generate the vertex data for the shape
    /// Assumes that m_vertexData has been cleared and reserved to the correct size
    virtual void generateVertexData() = 0;
    /// Returns the current value of param1
    int param1() const;
    /// Returns the current value of param2
    int param2() const;
    /// Returns whether the vao and vbo have been allocated
    bool glAllocated() const;
    /// Pushes a vertex and normal to the vertex data vector
    void pushVertex(glm::vec3 v, glm::vec3 n);
    std::vector<float> m_vertexData;
private:
    /// Allocates the vao and vbo, and sets up the attributes in the vao
    void allocateBuffers();
    bool m_glAllocated = false;
    int m_param1;
    int m_param2;
    GLuint m_vao;
    GLuint m_vbo;
};
#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

#include <iostream>
#include "primitivemesh.h"
#include "cubemesh.h"
#include "spheremesh.h"
#include "cylindermesh.h"
#include "conemesh.h"

std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>
PrimitiveMesh::initMeshes(int param1, int param2) {
    std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes;
    meshes[PrimitiveType::PRIMITIVE_CUBE] = std::make_shared<CubeMesh>(param1, param2);
    meshes[PrimitiveType::PRIMITIVE_SPHERE] = std::make_shared<SphereMesh>(param1, param2);
    meshes[PrimitiveType::PRIMITIVE_CYLINDER] = std::make_shared<CylinderMesh>(param1, param2);
    meshes[PrimitiveType::PRIMITIVE_CONE] = std::make_shared<ConeMesh>(param1, param2);
    return meshes;
}

PrimitiveMesh::PrimitiveMesh(int param1, int param2): m_param1(param1), m_param2(param2) {}

void PrimitiveMesh::setParams(int param1, int param2) {
    // maybe we should just update the buffers here (and call makeCurrent() before)
    //   edit: eh i don't feel like it, what i have works
    if (param1 < getMinParam1()) {
        param1 = getMinParam1();
    }
    if (param2 < getMinParam2()) {
        param2 = getMinParam2();
    }
    m_param1 = param1;
    m_param2 = param2;
    m_vertexData.clear();
    m_vertexData.reserve(getExpectedVectorSize());
    generateVertexData();
}

int PrimitiveMesh::param1() const {
    return m_param1;
}

int PrimitiveMesh::param2() const {
    return m_param2;
}

bool PrimitiveMesh::glAllocated() const {
    return m_glAllocated;
}

void PrimitiveMesh::pushVertex(glm::vec3 v, glm::vec3 n) {
    m_vertexData.push_back(v.x);
    m_vertexData.push_back(v.y);
    m_vertexData.push_back(v.z);
    m_vertexData.push_back(n.x);
    m_vertexData.push_back(n.y);
    m_vertexData.push_back(n.z);
}

void PrimitiveMesh::allocateBuffers() {
    if (m_glAllocated) {
        std::cerr << "Warning: trying to allocate buffers when already allocated" << std::endl;
        return;
    }
    // generate vao
    glGenVertexArrays(1, &m_vao);
    // generate vbo
    glGenBuffers(1, &m_vbo);
    // bind vbo + vao
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);
    // vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_glAllocated = true;
}


void PrimitiveMesh::updateBuffers() {
    if (m_vertexData.empty()) {
        m_vertexData.reserve(getExpectedVectorSize());
        // can't call this during init for C++ reasons, so this to account for that
        generateVertexData();
    }
    if (!m_glAllocated) {
        allocateBuffers();
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (m_vertexData.size() * sizeof(float)), m_vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PrimitiveMesh::deleteBuffers() {
    if (!m_glAllocated) {
        std::cerr << "Warning: trying to delete buffers when not allocated" << std::endl;
        return;
    }
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    m_glAllocated = false;
}

GLuint PrimitiveMesh::vao() const {
    if (!m_glAllocated) {
        throw std::runtime_error("Trying to get VAO when not allocated");
    }
    return m_vao;
}

GLuint PrimitiveMesh::vbo() const {
    if (!m_glAllocated) {
        throw std::runtime_error("Trying to get VBO when not allocated");
    }
    return m_vbo;
}

const std::vector<float>& PrimitiveMesh::vertexData() const {
    if (m_vertexData.empty()) {
        throw std::runtime_error("Trying to get vertex data when empty");
    }
    return m_vertexData;
}

#pragma clang diagnostic pop
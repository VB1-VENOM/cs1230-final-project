#include "cylindermesh.h"

#include <glm/ext/scalar_constants.hpp>

#define HEIGHT 1.0f
#define RADIUS 0.5f

CylinderMesh::CylinderMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

void CylinderMesh::generateVertexData() {
    makeCaps();
    makeSides();
}

void CylinderMesh::makeCapCenterTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight, CylinderFaceType face, float leftTheta, float rightTheta) {
    glm::vec3 normal = glm::normalize(center);
    pushVertex(center, normal, getUV(center, face, (leftTheta + rightTheta) / 2));
    pushVertex(bottomLeft, normal, getUV(bottomLeft, face, leftTheta));
    pushVertex(bottomRight, normal, getUV(bottomRight, face, rightTheta));
}

void CylinderMesh::makeCapTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight,
                           CylinderFaceType face,
                           float leftTheta,
                           float rightTheta) {
    glm::vec3 normal = glm::normalize(glm::vec3(0, topLeft.y, 0));
    pushVertex(topLeft, normal, getUV(topLeft, face, leftTheta));
    pushVertex(bottomLeft, normal, getUV(bottomLeft, face, leftTheta));
    pushVertex(topRight, normal, getUV(topRight, face, rightTheta));

    pushVertex(topRight, normal, getUV(topRight, face, rightTheta));
    pushVertex(bottomLeft, normal, getUV(bottomLeft, face, leftTheta));
    pushVertex(bottomRight, normal, getUV(bottomRight, face, rightTheta));
}

void CylinderMesh::makeCapWedge(float currentTheta, float nextTheta, float y, CylinderFaceType face, float leftTheta, float rightTheta) {
    for (int i = 0; i < param1() - 1; i++) {
        float currentDistFromCenter = RADIUS - (RADIUS / (float)param1() * (float)i);
        float nextDistFromCenter = RADIUS - (RADIUS / (float)param1() * (float)(i + 1));
        glm::vec3 bottomLeft = glm::vec3(
                currentDistFromCenter * glm::cos(currentTheta),
                y,
                currentDistFromCenter * glm::sin(currentTheta)
        );
        glm::vec3 bottomRight = glm::vec3(
                currentDistFromCenter * glm::cos(nextTheta),
                y,
                currentDistFromCenter * glm::sin(nextTheta)
        );
        glm::vec3 topLeft = glm::vec3(
                nextDistFromCenter * glm::cos(currentTheta),
                y,
                nextDistFromCenter * glm::sin(currentTheta)
        );
        glm::vec3 topRight = glm::vec3(
                nextDistFromCenter * glm::cos(nextTheta),
                y,
                nextDistFromCenter * glm::sin(nextTheta)
        );
        makeCapTile(topLeft, topRight, bottomLeft, bottomRight, face, currentTheta, nextTheta);
    }
    // center piece of this wedge
    float centerPieceEdgeLength = RADIUS / (float)param1();
    glm::vec3 center = glm::vec3(0, y, 0);
    glm::vec3 bottomLeft = glm::vec3(
            centerPieceEdgeLength * glm::cos(currentTheta),
            y,
            centerPieceEdgeLength * glm::sin(currentTheta)
    );
    glm::vec3 bottomRight = glm::vec3(
            centerPieceEdgeLength * glm::cos(nextTheta),
            y,
            centerPieceEdgeLength * glm::sin(nextTheta)
    );
    makeCapCenterTile(center, bottomLeft, bottomRight, face, currentTheta, nextTheta);
}

void CylinderMesh::makeCaps() {
    float cylinderHeight = 1.0;
    float thetaStep = glm::radians(360.f / (float) param2());
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    for (int i = 0; i < param2(); i++) {
        // there's something weird going on that causes me to have to swap currentTheta and nextTheta
        // from what is intuitive to me... idk and i dont feel like figuring it out right now
        makeCapWedge(nextTheta, currentTheta, cylinderHeight / 2, CylinderFaceType::TOP_CAP, currentTheta, nextTheta);
        makeCapWedge(currentTheta, nextTheta, -cylinderHeight / 2, CylinderFaceType::BOTTOM_CAP, currentTheta, nextTheta);
        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}

void CylinderMesh::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight,
                            float leftTheta,
                            float rightTheta) {
    glm::vec3 leftNormal = glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z));
    glm::vec3 rightNormal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    pushVertex(topLeft, leftNormal, getUV(topLeft, CylinderFaceType::SIDE, leftTheta));
    pushVertex(bottomRight, rightNormal, getUV(bottomRight, CylinderFaceType::SIDE, rightTheta));
    pushVertex(topRight, rightNormal, getUV(topRight, CylinderFaceType::SIDE, rightTheta));

    pushVertex(topLeft, leftNormal, getUV(topLeft, CylinderFaceType::SIDE, leftTheta));
    pushVertex(bottomLeft, leftNormal, getUV(bottomLeft, CylinderFaceType::SIDE, leftTheta));
    pushVertex(bottomRight, rightNormal, getUV(bottomRight, CylinderFaceType::SIDE, rightTheta));
}

void CylinderMesh::makeSide(float currentTheta, float nextTheta) {
    for (int j = 0; j < param1(); j++) {
        float currentDistFromBottom = HEIGHT / (float) param1() * (float) j;
        float nextDistFromBottom = HEIGHT / (float) param1() * (float) (j + 1);
        float currentY = -HEIGHT / 2.0f + currentDistFromBottom;
        float nextY = -HEIGHT / 2.0f + nextDistFromBottom;
        glm::vec3 bottomLeft = glm::vec3(
                RADIUS * glm::cos(currentTheta),
                currentY,
                RADIUS * glm::sin(currentTheta)
        );
        glm::vec3 bottomRight = glm::vec3(
                RADIUS * glm::cos(nextTheta),
                currentY,
                RADIUS * glm::sin(nextTheta)
        );
        glm::vec3 topLeft = glm::vec3(
                RADIUS * glm::cos(currentTheta),
                nextY,
                RADIUS * glm::sin(currentTheta)
        );
        glm::vec3 topRight = glm::vec3(
                RADIUS * glm::cos(nextTheta),
                nextY,
                RADIUS * glm::sin(nextTheta)
        );
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight, currentTheta, nextTheta);
    }
}

void CylinderMesh::makeSides() {
    float thetaStep = glm::radians(360.f / (float) param2());
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    for (int i = 0; i < param2(); i++) {
        makeSide(nextTheta, currentTheta);
        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}

int CylinderMesh::getMinParam1() const {
    return 1;
}

int CylinderMesh::getMinParam2() const {
    return 3;
}

int CylinderMesh::getExpectedVectorSize() {
    return param2() // number of sides
        * param1() // number of tiles per side
        * 2 // number of triangles per tile
        * 3 // number of vertices per triangle
        * FLOATS_PER_VERTEX // number of floats per vertex
        + param2() // number of center triangles on one cap
        * 2 // number of caps
        * 3 // number of vertices triangle
        * FLOATS_PER_VERTEX // number of floats per vertex
        + param2() // number of wedges on cap
        * 2 // number of caps
        * (std::max(param1() - 1, 0)) // number of tiles per wedge
        * 2 // number of triangles per tile
        * 3 // number of vertices per triangle
        * FLOATS_PER_VERTEX; // number of floats per vertex
}

glm::vec2 CylinderMesh::getUV(glm::vec3 pos, CylinderFaceType face, float theta) {
    switch (face) {
        case CylinderFaceType::TOP_CAP:
            return {pos.x + 0.5f, -pos.z + 0.5f};
        case CylinderFaceType::BOTTOM_CAP:
            return {pos.x + 0.5f, pos.z + 0.5f};
        case CylinderFaceType::SIDE: {
            // for some reason we need a 1 - theta / (2 * M_PIf) here instead of theta / (2 * M_PIf)???  idk why
            float u = 1 - theta / (2 *  glm::pi<float>());
            float v = 0.5f + pos.y;
            return {u, v};
        }
        default:
            throw std::runtime_error("Invalid cylinder intersection type");
    }
}

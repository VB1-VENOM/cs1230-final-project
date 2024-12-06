#include "cylindermesh.h"

#define HEIGHT 1.0f
#define RADIUS 0.5f

CylinderMesh::CylinderMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

void CylinderMesh::generateVertexData() {
    makeCaps();
    makeSides();
}

void CylinderMesh::makeCapCenterTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight) {
    glm::vec3 normal = glm::normalize(center);
    pushVertex(center, normal);
    pushVertex(bottomLeft, normal);
    pushVertex(bottomRight, normal);
}

void CylinderMesh::makeCapTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight) {
    glm::vec3 normal = glm::normalize(glm::vec3(0, topLeft.y, 0));
    pushVertex(topLeft, normal);
    pushVertex(bottomLeft, normal);
    pushVertex(topRight, normal);

    pushVertex(topRight, normal);
    pushVertex(bottomLeft, normal);
    pushVertex(bottomRight, normal);
}

void CylinderMesh::makeCapWedge(float currentTheta, float nextTheta, float y) {
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
        makeCapTile(topLeft, topRight, bottomLeft, bottomRight);
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
    makeCapCenterTile(center, bottomLeft, bottomRight);
}

void CylinderMesh::makeCaps() {
    float cylinderHeight = 1.0;
    float thetaStep = glm::radians(360.f / (float) param2());
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    for (int i = 0; i < param2(); i++) {
        // there's something weird going on that causes me to have to swap currentTheta and nextTheta
        // from what is intuitive to me... idk and i dont feel like figuring it out right now
        makeCapWedge(nextTheta, currentTheta, cylinderHeight / 2);
        makeCapWedge(currentTheta, nextTheta, -cylinderHeight / 2);
        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}

void CylinderMesh::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {
    glm::vec3 leftNormal = glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z));
    glm::vec3 rightNormal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    pushVertex(topLeft, leftNormal);
    pushVertex(bottomRight, rightNormal);
    pushVertex(topRight, rightNormal);

    pushVertex(topLeft, leftNormal);
    pushVertex(bottomLeft, leftNormal);
    pushVertex(bottomRight, rightNormal);
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
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
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
        * 6 // number of floats per vertex
        + param2() // number of center triangles on one cap
        * 2 // number of caps
        * 3 // number of vertices triangle
        * 6 // number of floats per vertex
        + param2() // number of wedges on cap
        * 2 // number of caps
        * (std::max(param1() - 1, 0)) // number of tiles per wedge
        * 2 // number of triangles per tile
        * 3 // number of vertices per triangle
        * 6; // number of floats per vertex
}
#include "spheremesh.h"

SphereMesh::SphereMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

void SphereMesh::generateVertexData() {
    float thetaStep = glm::radians(360.f / (float) param2());
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    for (int i = 0; i < param2(); i++) {
        // why did i have to swap currentTheta and nextTheta? that doesn't make sense...
        //    edit: looks like theta is moving clockwise??? for some reason???
        makeWedge(nextTheta, currentTheta);
        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}

void SphereMesh::makeTopTipTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight) {
    pushVertex(center, glm::normalize(center));
    pushVertex(bottomLeft, glm::normalize(bottomLeft));
    pushVertex(bottomRight, glm::normalize(bottomRight));
}

void SphereMesh::makeBottomTipTile(glm::vec3 center, glm::vec3 topLeft, glm::vec3 topRight) {
    pushVertex(center, glm::normalize(center));
    pushVertex(topRight, glm::normalize(topRight));
    pushVertex(topLeft, glm::normalize(topLeft));
}

void SphereMesh::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    pushVertex(topLeft, glm::normalize(topLeft)); // sphere normal is just direction from origin
    pushVertex(bottomRight, glm::normalize(bottomRight));
    pushVertex(topRight, glm::normalize(topRight));

    pushVertex(topLeft, glm::normalize(topLeft));
    pushVertex(bottomLeft, glm::normalize(bottomLeft));
    pushVertex(bottomRight, glm::normalize(bottomRight));
}

void SphereMesh::makeWedge(float currentTheta, float nextTheta) {
    float phiStep = glm::radians(180.f / (float) param1());
    float currentPhi = 0 * phiStep;
    float nextPhi = 1 * phiStep;
    float r = 0.5;
    // x = r * sin(phi) * cos(theta)
    // y = r * cos(phi)
    // z = r * sin(phi) * sin(theta)
    for (int i = 0; i < param1(); i++) {
        glm::vec3 topLeft = glm::vec3(
                r * glm::sin(currentPhi) * glm::cos(currentTheta),
                r * glm::cos(currentPhi),
                r * glm::sin(currentPhi) * glm::sin(currentTheta)
        );
        glm::vec3 topRight = glm::vec3(
                r * glm::sin(currentPhi) * glm::cos(nextTheta),
                r * glm::cos(currentPhi),
                r * glm::sin(currentPhi) * glm::sin(nextTheta)
        );
        glm::vec3 bottomLeft = glm::vec3(
                r * glm::sin(nextPhi) * glm::cos(currentTheta),
                r * glm::cos(nextPhi),
                r * glm::sin(nextPhi) * glm::sin(currentTheta)
        );
        glm::vec3 bottomRight = glm::vec3(
                r * glm::sin(nextPhi) * glm::cos(nextTheta),
                r * glm::cos(nextPhi),
                r * glm::sin(nextPhi) * glm::sin(nextTheta)
        );
        if (i == 0) {
            glm::vec3 center = glm::vec3(0, 0.5, 0);
            makeTopTipTile(center, bottomLeft, bottomRight);
        } else if (i == param1() - 1) {
            glm::vec3 center = glm::vec3(0, -0.5, 0);
            makeBottomTipTile(center, topLeft, topRight);
        } else {
            makeTile(topLeft, topRight, bottomLeft, bottomRight);
        }
        currentPhi += phiStep;
        nextPhi += phiStep;
    }
}

int SphereMesh::getMinParam1() const {
    return 2;
}

int SphereMesh::getMinParam2() const {
    return 3;
}

int SphereMesh::getExpectedVectorSize() {
    return param2() // number of wedges
        * std::max(param1() - 2, 0) // number of full tiles per wedge
        * 2 // number of triangles per tile
        * 3 // number of vertices per triangle
        * 6 // number of floats per vertex
        + param2() // number of wedges
        * 2 // number of caps
        * 3 // number of vertices per triangle
        * 6; // number of floats per vertex
}

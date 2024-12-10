#include "skycubemesh.h"

#include <glm/ext/scalar_constants.hpp>

SkyCubeMesh::SkyCubeMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

void SkyCubeMesh::generateVertexData() {
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

void SkyCubeMesh::makeTopTipTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight, float leftTheta, float rightTheta) {
    pushVertex(center, glm::normalize(center), getUV(center, (leftTheta + rightTheta) / 2));
    pushVertex(bottomRight, glm::normalize(bottomRight), getUV(bottomRight, rightTheta));
    pushVertex(bottomLeft, glm::normalize(bottomLeft), getUV(bottomLeft, leftTheta));
}

void SkyCubeMesh::makeBottomTipTile(glm::vec3 center, glm::vec3 topLeft, glm::vec3 topRight, float leftTheta, float rightTheta) {
    pushVertex(center, glm::normalize(center), getUV(center, (leftTheta + rightTheta) / 2));
    pushVertex(topLeft, glm::normalize(topLeft), getUV(topLeft, leftTheta));
    pushVertex(topRight, glm::normalize(topRight), getUV(topRight, rightTheta));
}

void SkyCubeMesh::makeTile(glm::vec3 topLeft,
                          glm::vec3 topRight,
                          glm::vec3 bottomLeft,
                          glm::vec3 bottomRight,
                          float leftTheta,
                          float rightTheta) {
    pushVertex(topLeft, glm::normalize(topLeft), getUV(topLeft, leftTheta)); // sphere normal is just direction from origin
    pushVertex(topRight, glm::normalize(topRight), getUV(topRight, rightTheta));
    pushVertex(bottomRight, glm::normalize(bottomRight), getUV(bottomRight, rightTheta));

    pushVertex(topLeft, glm::normalize(topLeft), getUV(topLeft, leftTheta));
    pushVertex(bottomRight, glm::normalize(bottomRight), getUV(bottomRight, rightTheta));
    pushVertex(bottomLeft, glm::normalize(bottomLeft), getUV(bottomLeft, leftTheta));
}

void SkyCubeMesh::makeWedge(float currentTheta, float nextTheta) {
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
            makeTopTipTile(center, bottomLeft, bottomRight, currentTheta, nextTheta);
        } else if (i == param1() - 1) {
            glm::vec3 center = glm::vec3(0, -0.5, 0);
            makeBottomTipTile(center, topLeft, topRight, currentTheta, nextTheta);
        } else {
            makeTile(topLeft, topRight, bottomLeft, bottomRight, currentTheta, nextTheta);
        }
        currentPhi += phiStep;
        nextPhi += phiStep;
    }
}

int SkyCubeMesh::getMinParam1() const {
    return 2;
}

int SkyCubeMesh::getMinParam2() const {
    return 3;
}

int SkyCubeMesh::getExpectedVectorSize() {
    return param2() // number of wedges
           * std::max(param1() - 2, 0) // number of full tiles per wedge
           * 2 // number of triangles per tile
           * 3 // number of vertices per triangle
           * FLOATS_PER_VERTEX // number of floats per vertex
           + param2() // number of wedges
             * 2 // number of caps
             * 3 // number of vertices per triangle
             * FLOATS_PER_VERTEX; // number of floats per vertex
}

glm::vec2 SkyCubeMesh::getUV(glm::vec3 pos, float theta) {
    // from lecture: phi = asin(y/r) = asin(2y) => phi in range [-pi/2, pi/2]
    // v = phi/2 + 1/2
    float asin = std::asin(2.f * pos.y);
    float v = asin /  glm::pi<float>() + 0.5f;
    // for some reason we need a 1 - theta / (2 * M_PIf) here instead of theta / (2 * M_PIf)???  idk why
    float u = 1 - theta / (2 *  glm::pi<float>());

    return {u, v};
}

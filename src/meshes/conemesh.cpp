#include "conemesh.h"

#define HEIGHT 1.0f
#define RADIUS_BOTTOM 0.5f

ConeMesh::ConeMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

void ConeMesh::generateVertexData() {
    makeCap();
    makeSides();
}

void ConeMesh::makeCapCenterTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight, float leftTheta, float rightTheta) {
    glm::vec3 normal = glm::normalize(center);
    pushVertex(center, normal, getUV(center, ConeFaceType::BASE, (leftTheta + rightTheta) / 2));
    pushVertex(bottomLeft, normal, getUV(bottomLeft, ConeFaceType::BASE, leftTheta));
    pushVertex(bottomRight, normal, getUV(bottomRight, ConeFaceType::BASE, rightTheta));
}

void ConeMesh::makeCapTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight,
                       float leftTheta,
                       float rightTheta) {
    glm::vec3 normal = glm::normalize(glm::vec3(0, topLeft.y, 0));
    pushVertex(topLeft, normal, getUV(topLeft, ConeFaceType::BASE, leftTheta));
    pushVertex(bottomLeft, normal, getUV(bottomLeft, ConeFaceType::BASE, leftTheta));
    pushVertex(topRight, normal, getUV(topRight, ConeFaceType::BASE, rightTheta));

    pushVertex(topRight, normal, getUV(topRight, ConeFaceType::BASE, rightTheta));
    pushVertex(bottomLeft, normal, getUV(bottomLeft, ConeFaceType::BASE, leftTheta));
    pushVertex(bottomRight, normal, getUV(bottomRight, ConeFaceType::BASE, rightTheta));
}

void ConeMesh::makeCapWedge(float currentTheta, float nextTheta, float y) {
    for (int i = 0; i < param1() - 1; i++) {
        float currentDistFromCenter = RADIUS_BOTTOM - (RADIUS_BOTTOM / (float)param1() * (float)i);
        float nextDistFromCenter = RADIUS_BOTTOM - (RADIUS_BOTTOM / (float)param1() * (float)(i + 1));
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
        makeCapTile(topLeft, topRight, bottomLeft, bottomRight, currentTheta, nextTheta);
    }
    // center piece of this wedge
    float centerPieceEdgeLength = RADIUS_BOTTOM / (float)param1();
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
    makeCapCenterTile(center, bottomLeft, bottomRight, currentTheta, nextTheta);
}

void ConeMesh::makeCap() {
    float thetaStep = glm::radians(360.f / (float) param2());
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    for (int i = 0; i < param2(); i++) {
        // there's something weird going on that causes me to have to swap currentTheta and nextTheta
        // from what is intuitive to me... idk and i dont feel like figuring it out right now
        makeCapWedge(currentTheta, nextTheta, -HEIGHT / 2);
        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}

void ConeMesh::makeTipTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight, float leftTheta, float rightTheta) {
    // all credit to the anonymous student on #290 (https://edstem.org/us/courses/65180/discussion/5660567)
    // for this tip normal calculation
    glm::vec3 tipNormal = getSideTileNormal(bottomLeft) + getSideTileNormal(bottomRight);
    tipNormal = glm::vec3(tipNormal.x, 0, tipNormal.z);
    tipNormal = glm::normalize(tipNormal);
    tipNormal = glm::vec3(tipNormal.x, 0.5, tipNormal.z);
    tipNormal = glm::normalize(tipNormal);
    pushVertex(center, glm::normalize(tipNormal), getUV(center, ConeFaceType::SIDE, (leftTheta + rightTheta) / 2));
    pushVertex(bottomLeft, getSideTileNormal(bottomLeft), getUV(bottomLeft, ConeFaceType::SIDE, leftTheta));
    pushVertex(bottomRight, getSideTileNormal(bottomRight), getUV(bottomRight, ConeFaceType::SIDE, rightTheta));
}

glm::vec3 ConeMesh::getSideTileNormal(glm::vec3 point) {
    return glm::normalize(glm::vec3(2.f * point.x, -0.5f * point.y + 0.25f, 2.f * point.z));
}

void ConeMesh::makeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight,
                        float leftTheta,
                        float rightTheta) {
    // these vertices should be on the implicit cone, so the normal can just be computed the same way
    pushVertex(topLeft, getSideTileNormal(topLeft), getUV(topLeft, ConeFaceType::SIDE, leftTheta));
    pushVertex(bottomRight, getSideTileNormal(bottomRight), getUV(bottomRight, ConeFaceType::SIDE, rightTheta));
    pushVertex(topRight, getSideTileNormal(topRight), getUV(topRight, ConeFaceType::SIDE, rightTheta));

    pushVertex(topLeft, getSideTileNormal(topLeft), getUV(topLeft, ConeFaceType::SIDE, leftTheta));
    pushVertex(bottomLeft, getSideTileNormal(bottomLeft), getUV(bottomLeft, ConeFaceType::SIDE, leftTheta));
    pushVertex(bottomRight, getSideTileNormal(bottomRight), getUV(bottomRight, ConeFaceType::SIDE, rightTheta));
}

void ConeMesh::makeSide(float currentTheta, float nextTheta) {
    for (int j = 0; j < param1(); j++) {
        float currentDistFromBottom = HEIGHT / (float) param1() * (float) j;
        float nextDistFromBottom = HEIGHT / (float) param1() * (float) (j + 1);
        float currentY = -HEIGHT / 2.0f + currentDistFromBottom;
        float nextY = -HEIGHT / 2.0f + nextDistFromBottom;
        float currentPercentToBottom = 1.f - (float) j / (float) param1();
        float nextPercentToBottom = 1.f - (float) (j + 1) / (float) param1();
        glm::vec3 bottomLeft = glm::vec3(
                currentPercentToBottom * RADIUS_BOTTOM * glm::cos(currentTheta),
                currentY,
                currentPercentToBottom * RADIUS_BOTTOM * glm::sin(currentTheta)
        );
        glm::vec3 bottomRight = glm::vec3(
                currentPercentToBottom * RADIUS_BOTTOM * glm::cos(nextTheta),
                currentY,
                currentPercentToBottom * RADIUS_BOTTOM * glm::sin(nextTheta)
        );
        if (j == param1() - 1) {
            makeTipTile(glm::vec3(0, nextY, 0), bottomLeft, bottomRight, currentTheta, nextTheta);
        } else {
            glm::vec3 topLeft = glm::vec3(
                    nextPercentToBottom * RADIUS_BOTTOM * glm::cos(currentTheta),
                    nextY,
                    nextPercentToBottom * RADIUS_BOTTOM * glm::sin(currentTheta)
            );
            glm::vec3 topRight = glm::vec3(
                    nextPercentToBottom * RADIUS_BOTTOM * glm::cos(nextTheta),
                    nextY,
                    nextPercentToBottom * RADIUS_BOTTOM * glm::sin(nextTheta)
            );
            makeSideTile(topLeft, topRight, bottomLeft, bottomRight, currentTheta, nextTheta);
        }
    }
}

void ConeMesh::makeSides() {
    float thetaStep = glm::radians(360.f / (float) param2());
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    for (int i = 0; i < param2(); i++) {
        makeSide(nextTheta, currentTheta);
        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}

int ConeMesh::getMinParam1() const {
    return 1;
}

int ConeMesh::getMinParam2() const {
    return 3;
}

int ConeMesh::getExpectedVectorSize() {
    return param2() // number of sides
           * std::max(param1() - 1, 0) // number of normal tiles per side
           * 2 // number of triangles per tile
           * 3 // number of vertices per triangle
           * FLOATS_PER_VERTEX // number of floats per vertex
           + param2() // number of sides
           * 1 // number of tip tiles per side
           * 3 // number of vertices per triangle
           * FLOATS_PER_VERTEX // number of floats per vertex
           + param2() // number of center triangles on one cap
             * 1 // number of caps
             * 3 // number of vertices triangle
             * FLOATS_PER_VERTEX // number of floats per vertex
           + param2() // number of wedges on cap
             * 1 // number of caps
             * (std::max(param1() - 1, 0)) // number of tiles per wedge
             * 2 // number of triangles per tile
             * 3 // number of vertices per triangle
             * FLOATS_PER_VERTEX; // number of floats per vertex
}

glm::vec2 ConeMesh::getUV(glm::vec3 pos, ConeFaceType face, float theta) {
    switch (face) {
        case ConeFaceType::BASE:
            return {pos.x + 0.5f, pos.z + 0.5f}; // use bottom cap cylinder formula
        case ConeFaceType::SIDE: {
            float v = 0.5f + pos.y;
            // for some reason we need a 1 - theta / (2 * M_PIf) here instead of theta / (2 * M_PIf)???  idk why
            float u = 1 - theta / (2 * M_PIf);
            return {u, v};
        }
        default:
            throw std::runtime_error("Invalid cone intersection type");
    }
}

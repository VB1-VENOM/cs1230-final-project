#include "cubemesh.h"
#include "primitivemesh.h"

CubeMesh::CubeMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

int CubeMesh::getMinParam1() const {
    return 1;
}

int CubeMesh::getMinParam2() const {
    // doesn't really matter for cube, but we'll set it to 1 for now
    return 1;
}

int CubeMesh::getExpectedVectorSize() {
    return (param1() * param1()) // number of squares per side
           * 6 // number of sides
           * 2 // number of triangles per square
           * 3 // number of vertices per triangle
           * 6; // number of floats per vertex
}

void CubeMesh::generateVertexData() {
    // pos z face
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));

    // i just copy and pasted and swapped things until it worked... hopefully if it looks okay, that means it's good

    // neg z face
    makeFace(glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f));

    // pos x face
    makeFace(glm::vec3(0.5f, -0.5f,  0.5f),
             glm::vec3(0.5f,  0.5f,  0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f));

    // neg x face
    makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f));

    // pos y face
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3( 0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f));

    // neg y face
    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));


}


void CubeMesh::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft) {
    float totalWidth = glm::length(topRight - topLeft);
    float tileWidth = totalWidth / (float) param1();
    float totalHeight = glm::length(bottomLeft - topLeft);
    float tileHeight = totalHeight / (float) param1();
    glm::vec3 rightVec = glm::normalize(topRight - topLeft);
    glm::vec3 downVec = glm::normalize(bottomLeft - topLeft);
    for (int i = 0; i < param1(); i++) {
        for (int j = 0; j < param1(); j++) {
            glm::vec3 tileTopLeft = topLeft + (float) i * tileWidth * rightVec + (float) j * tileHeight * downVec;
            makeTile(
                    tileTopLeft,
                    tileTopLeft + tileWidth * rightVec,
                    tileTopLeft + tileHeight * downVec,
                    tileTopLeft + tileWidth * rightVec + tileHeight * downVec
            );
        }
    }
}

void CubeMesh::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    pushVertex(topLeft, glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft)));
    pushVertex(bottomRight, glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight)));
    pushVertex(topRight, glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight)));

    pushVertex(topLeft, glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft)));
    pushVertex(bottomLeft, glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));
    pushVertex(bottomRight, glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight)));
}
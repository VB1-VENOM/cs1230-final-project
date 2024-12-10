#include "cubemesh.h"
#include "primitivemesh.h"
#include <optional>

CubeMesh::CubeMesh(int param1, int param2) : PrimitiveMesh(param1, param2) {}

AABB CubeMesh::computeAABB(const glm::mat4& ctm) const {
    // the most extreme points are the corners of the cube, so we can transform those and find the min and max
    std::optional<glm::vec3> min = std::nullopt;
    std::optional<glm::vec3> max = std::nullopt;
    for (float x : {-0.5f, 0.5f}) {
        for (float y : {-0.5f, 0.5f}) {
            for (float z : {-0.5f, 0.5f}) {
                glm::vec3 transformed = glm::vec3(ctm * glm::vec4(x, y, z, 1));
                if (!min) {
                    min = transformed;
                }
                if (!max) {
                    max = transformed;
                }
                if (transformed.x < min->x) {
                    min->x = transformed.x;
                }
                if (transformed.y < min->y) {
                    min->y = transformed.y;
                }
                if (transformed.z < min->z) {
                    min->z = transformed.z;
                }
                if (transformed.x > max->x) {
                    max->x = transformed.x;
                }
                if (transformed.y > max->y) {
                    max->y = transformed.y;
                }
                if (transformed.z > max->z) {
                    max->z = transformed.z;
                }
            }
        }
    }
    return {min.value(), max.value()};
}

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
           * FLOATS_PER_VERTEX; // number of floats per vertex
}

void CubeMesh::generateVertexData() {
    // pos z face
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             CubeFaceType::POS_Z);

    // i just copy and pasted and swapped things until it worked... hopefully if it looks okay, that means it's good

    // neg z face
    makeFace(glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             CubeFaceType::NEG_Z);

    // pos x face
    makeFace(glm::vec3(0.5f, -0.5f,  0.5f),
             glm::vec3(0.5f,  0.5f,  0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             CubeFaceType::POS_X);

    // neg x face
    makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             CubeFaceType::NEG_X);

    // pos y face
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3( 0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             CubeFaceType::POS_Y);

    // neg y face
    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             CubeFaceType::NEG_Y);


}


void CubeMesh::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    CubeFaceType face) {
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
                    tileTopLeft + tileWidth * rightVec + tileHeight * downVec,
                    face
            );
        }
    }
}

void CubeMesh::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    CubeFaceType face) {
    pushVertex(topLeft, glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft)), getUV(topLeft, face));
    pushVertex(bottomRight, glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight)), getUV(bottomRight, face));
    pushVertex(topRight, glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight)), getUV(topRight, face));

    pushVertex(topLeft, glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft)), getUV(topLeft, face));
    pushVertex(bottomLeft, glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)), getUV(bottomLeft, face));
    pushVertex(bottomRight, glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight)), getUV(bottomRight, face));
}

glm::vec2 CubeMesh::getUV(glm::vec3 pos, CubeFaceType face) {
    switch (face) {
        case CubeFaceType::POS_X:
            return {-pos.z + 0.5f, pos.y + 0.5f};
        case CubeFaceType::NEG_X:
            return {pos.z + 0.5f, pos.y + 0.5f};
        case CubeFaceType::POS_Y:
            return {pos.x + 0.5f, -pos.z + 0.5f};
        case CubeFaceType::NEG_Y:
            return {pos.x + 0.5f, pos.z + 0.5f};
        case CubeFaceType::POS_Z:
            return {pos.x + 0.5f, pos.y + 0.5f};
        case CubeFaceType::NEG_Z:
            return {-pos.x + 0.5f, pos.y + 0.5f};
        default:
            throw std::runtime_error("Invalid cube intersection type");
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "primitivemesh.h"

enum class CylinderFaceType {
    TOP_CAP,
    BOTTOM_CAP,
    SIDE,
};

class CylinderMesh : public PrimitiveMesh {
public:
    CylinderMesh(int param1, int param2);
protected:
    int getMinParam1() const override;
    int getMinParam2() const override;
    int getExpectedVectorSize() override;
    void generateVertexData() override;
private:
    static glm::vec2 getUV(glm::vec3 pos, CylinderFaceType face, float theta);

    void makeCapCenterTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight, CylinderFaceType face, float leftTheta, float rightTheta);
    void makeCapTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight,
                     CylinderFaceType face,
                     float leftTheta,
                     float rightTheta);
    void makeCapWedge(float currentTheta, float nextTheta, float z, CylinderFaceType face, float leftTheta, float rightTheta);
    void makeCaps();

    void makeSideTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight, float leftTheta, float rightTheta);
    void makeSide(float currentTheta, float nextTheta);
    void makeSides();
};


#pragma clang diagnostic pop

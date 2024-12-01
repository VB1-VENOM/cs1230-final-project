#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "primitivemesh.h"

class CylinderMesh : public PrimitiveMesh {
public:
    CylinderMesh(int param1, int param2);
protected:
    int getMinParam1() const override;
    int getMinParam2() const override;
    int getExpectedVectorSize() override;
    void generateVertexData() override;
private:
    void makeCapCenterTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeCapTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight);
    void makeCapWedge(float currentTheta, float nextTheta, float z);
    void makeCaps();

    void makeSideTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeSide(float currentTheta, float nextTheta);
    void makeSides();
};


#pragma clang diagnostic pop

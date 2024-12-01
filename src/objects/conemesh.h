#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "primitivemesh.h"

class ConeMesh : public PrimitiveMesh {
public:
    ConeMesh(int param1, int param2);
protected:
    int getMinParam1() const override;
    int getMinParam2() const override;
    int getExpectedVectorSize() override;
    void generateVertexData() override;
private:
    void makeCapTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeCapCenterTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeCapWedge(float currentTheta, float nextTheta, float y);
    void makeCap();

    static glm::vec3 getSideTileNormal(glm::vec3 point);
    void makeSideTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeTipTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeSide(float currentTheta, float nextTheta);
    void makeSides();
};


#pragma clang diagnostic pop

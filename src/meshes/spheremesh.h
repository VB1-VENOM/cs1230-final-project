#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "primitivemesh.h"

class SphereMesh : public PrimitiveMesh {
public:
    SphereMesh(int param1, int param2);
protected:
    int getMinParam1() const override;
    int getMinParam2() const override;
    int getExpectedVectorSize() override;
    void generateVertexData() override;
private:
    void makeTopTipTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeBottomTipTile(glm::vec3 center, glm::vec3 topLeft, glm::vec3 topRight);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
};


#pragma clang diagnostic pop

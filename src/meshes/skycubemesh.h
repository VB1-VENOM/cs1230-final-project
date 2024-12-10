#pragma once
#include "primitivemesh.h"

class SkyCubeMesh : public PrimitiveMesh {
public:
    SkyCubeMesh(int param1, int param2);
protected:
    int getMinParam1() const override;
    int getMinParam2() const override;
    int getExpectedVectorSize() override;
    void generateVertexData() override;
private:
    static glm::vec2 getUV(glm::vec3 pos, float theta);
    void makeTopTipTile(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight, float leftTheta, float rightTheta);
    void makeBottomTipTile(glm::vec3 center, glm::vec3 topLeft, glm::vec3 topRight, float leftTheta, float rightTheta);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  float leftTheta,
                  float rightTheta);
    void makeWedge(float currTheta, float nextTheta);
};

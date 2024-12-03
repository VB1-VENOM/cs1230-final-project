#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "primitivemesh.h"

class CubeMesh : public PrimitiveMesh {
public:
    CubeMesh(int param1, int param2);
    AABB computeAABB(const glm::mat4& ctm) const override;
protected:
    int getMinParam1() const override;
    int getMinParam2() const override;
    int getExpectedVectorSize() override;
    void generateVertexData() override;
private:

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    // bottom right can be derived since we know these are all squares!
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft);
};


#pragma clang diagnostic pop

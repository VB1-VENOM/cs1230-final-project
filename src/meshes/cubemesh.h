#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "primitivemesh.h"

enum class CubeFaceType {
    POS_X,
    NEG_X,
    POS_Y,
    NEG_Y,
    POS_Z,
    NEG_Z,
};

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
    static glm::vec2 getUV(glm::vec3 pos, CubeFaceType face);

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  CubeFaceType face);
    // bottom right can be derived since we know these are all squares!
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  CubeFaceType face);
};


#pragma clang diagnostic pop

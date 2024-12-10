#pragma once
#include "realtimeobject.h"
#include "camera.h"

class SkyboxObject : public RealtimeObject {
public:
    SkyboxObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene, std::shared_ptr<Camera> camera);

    void tick(double elapsedSeconds) override;
private:
    std::shared_ptr<Camera> m_camera;
    // java-like super
    typedef RealtimeObject super;
};
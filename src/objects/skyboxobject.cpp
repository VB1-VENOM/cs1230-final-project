#include "skyboxobject.h"
#include "realtimescene.h"

SkyboxObject::SkyboxObject(const RenderShapeData& data,
                           const std::shared_ptr<RealtimeScene>& scene, std::shared_ptr<Camera> camera) :
        super(data, scene), m_camera(std::move(camera)) {}

void SkyboxObject::tick(double elapsedSeconds) {
    translate(m_camera->pos() - pos());
}


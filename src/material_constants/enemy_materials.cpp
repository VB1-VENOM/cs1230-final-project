//
// Created by bcher on 12/8/2024.
//

#include "enemy_materials.h"
#include <utils/scenedata.h>

namespace enemy_materials {
    SceneMaterial enemyMaterial = SceneMaterial{
            SceneColor{0.5f, 0.5f, 0.5f, 1.f}, //ambient
            SceneColor{0.5f, 0.75f, 0.5f, 1.f}, //diffuse
            SceneColor{0.5f, 0.75f, 0.5f, 1.f}, //specular
            1.f, //shininesss
            SceneColor{0.5f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
            SceneColor{0.5f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
            1.f, //index of refraction
            SceneFileMap{},
            1.f //blend
    };
}
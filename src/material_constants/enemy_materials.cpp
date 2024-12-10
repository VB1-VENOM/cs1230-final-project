//
// Created by bcher on 12/8/2024.
//

#include "enemy_materials.h"
#include <utils/scenedata.h>

namespace enemy_materials {
    SceneMaterial enemyMaterial = SceneMaterial{
            SceneColor{glm::vec4(0.f, 0.f, 0.f, 1.f)}, //ambient
            SceneColor{glm::vec4(0.f, 0.f, 0.f, 1.f)}, //diffuse
            SceneColor{glm::vec4(0.f, 0.f, 0.f, 1.f)}, //specular
            0.f, //shininesss
            SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
            SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
            0.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/balloon.png"), 1.f, 1.f},
            1.f //blend
    };
    SceneMaterial damagedEnemyMaterial = SceneMaterial{
        SceneColor{1.f, 0.5f, 0.5f, 1.f}, //ambient
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //diffuse
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //specular
        1.f, //shininesss
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        1.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/balloon.png"), 1.f, 1.f},
        1.f //blend
};
}
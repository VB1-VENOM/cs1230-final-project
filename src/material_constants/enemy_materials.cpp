//
// Created by bcher on 12/8/2024.
//

#include "enemy_materials.h"

#include <random>
#include <utils/scenedata.h>

namespace enemy_materials {

    SceneMaterial enemyMaterial1 = SceneMaterial{
            SceneColor(0.f, 0.f, 0.f, 1.0f), // Gray color
            SceneColor(0.0f, 0.1f, 0.0f, 1.0f),
            SceneColor(0.5f, 0.5f, 0.5f, 1.0f),
            10.f, //shininesss
            SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
            SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
            0.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/alien.png"), 1.f, 1.f},
            0.5f //blend
    };
    SceneMaterial damagedEnemyMaterial1 = SceneMaterial{
        SceneColor{1.f, 0.5f, 0.5f, 1.f}, //ambient
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //diffuse
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //specular
        1.f, //shininesss
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        1.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/alien.png"), 1.f, 1.f},
        1.f //blend
};
    SceneMaterial enemyMaterial2 = SceneMaterial{
        SceneColor(0.f, 0.f, 0.f, 1.0f), // Gray color
        SceneColor(0.0f, 0.1f, 0.0f, 1.0f),
        SceneColor(0.5f, 0.5f, 0.5f, 1.0f),
        10.f, //shininesss
        SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
        0.f, //index of refraction
        SceneFileMap{true, std::string("scenefiles/moretextures/goblin.png"), 1.f, 1.f},
        0.5f //blend
};
    SceneMaterial damagedEnemyMaterial2 = SceneMaterial{
        SceneColor{1.f, 0.5f, 0.5f, 1.f}, //ambient
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //diffuse
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //specular
        1.f, //shininesss
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        1.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/goblin.png"), 1.f, 1.f},
        1.f //blend
};

    SceneMaterial enemyMaterial3 = SceneMaterial{
        SceneColor(0.f, 0.f, 0.f, 1.0f), // Gray color
        SceneColor(0.0f, 0.1f, 0.0f, 1.0f),
        SceneColor(0.5f, 0.5f, 0.5f, 1.0f),
        10.f, //shininesss
        SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
        0.f, //index of refraction
        SceneFileMap{true, std::string("scenefiles/moretextures/slime.png"), 1.f, 1.f},
        0.5f //blend
};
    SceneMaterial damagedEnemyMaterial3 = SceneMaterial{
        SceneColor{1.f, 0.5f, 0.5f, 1.f}, //ambient
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //diffuse
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //specular
        1.f, //shininesss
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        1.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/slime.png"), 1.f, 1.f},
        1.f //blend
};

    SceneMaterial enemyMaterial4 = SceneMaterial{
        SceneColor(0.f, 0.f, 0.f, 1.0f), // Gray color
        SceneColor(0.0f, 0.1f, 0.0f, 1.0f),
        SceneColor(0.5f, 0.5f, 0.5f, 1.0f),
        10.f, //shininesss
        SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{glm::vec4(1.f)}, // Used to weight contribution of reflected ray lighting (via multiplication)
        0.f, //index of refraction
        SceneFileMap{true, std::string("scenefiles/moretextures/ufo.png"), 1.f, 1.f},
        0.5f //blend
};
    SceneMaterial damagedEnemyMaterial4 = SceneMaterial{
        SceneColor{1.f, 0.5f, 0.5f, 1.f}, //ambient
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //diffuse
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, //specular
        1.f, //shininesss
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        SceneColor{1.f, 0.75f, 0.5f, 1.f}, // Used to weight contribution of reflected ray lighting (via multiplication)
        1.f, //index of refraction
            SceneFileMap{true, std::string("scenefiles/moretextures/ufo.png"), 1.f, 1.f},
        1.f //blend
};

    SceneMaterial getRandomEnemyMaterial() {
        static std::mt19937 gen(std::random_device{}());
        static std::vector<SceneMaterial> materials = {
            enemyMaterial1,
            enemyMaterial2,
            enemyMaterial3,
            enemyMaterial4
        };
        std::uniform_int_distribution<int> dist(0, materials.size() - 1);
        int index = dist(gen);
        return materials[index];
    }
}
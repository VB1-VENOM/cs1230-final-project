#include <optional>
#include <iostream>
#include <algorithm>
#include "realtimescene.h"
#include "objects/realtimeobject.h"
#include "objects/staticobject.h"
#include "glm/ext/matrix_transform.hpp"

#include "objects/playerobject.h"
#include <random>
#include <unordered_set>
#include <utility> // For std::pair
#include <functional> // For std::hash
#include "utils/helpers.h"
#include "material_constants/enemy_materials.h"
#include "objects/skyboxobject.h"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantParameter"
#define MAX_LIGHTS 16

const unsigned int SHADER_LIGHT_POINT       = 0x1u;
const unsigned int SHADER_LIGHT_DIRECTIONAL = 0x2u;
const unsigned int SHADER_LIGHT_SPOT        = 0x4u;


void printActiveGrids(const std::unordered_set<std::pair<int, int>, pair_hash>& activeGrids) {
    std::cout << "Active Grids:" << std::endl;
    for (const auto& grid : activeGrids) {
        std::cout << "(" << grid.first << ", " << grid.second << ")" << std::endl;
    }
}

std::unordered_set<std::pair<int, int>, pair_hash> RealtimeScene::m_activeGrids;



// GLuint loadCubemap(std::vector<std::string> faces) {
//     GLuint cubemapID;
//     glGenTextures(1, &cubemapID);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
//
//     for (GLuint i = 0; i < faces.size(); i++) {
//         // Load each face image using your custom loadImageFromFile function
//         std::unique_ptr<Image> img = loadImageFromFile(faces[i]);
//         if (!img) {
//             std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//             continue;
//         }
//
//         // Upload the image to the corresponding face of the cube map
//         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                      0, GL_RGBA, img->width, img->height,
//                      0, GL_RGBA, GL_UNSIGNED_BYTE, img->data.data());
//     }
//
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//     return cubemapID;
// }


std::shared_ptr<RealtimeScene> RealtimeScene::init(int width, int height, const std::string& sceneFilePath,
                                                 float nearPlane, float farPlane,
                                                 std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes, std::shared_ptr<bool> taken_damage) {

    RenderData renderData;
    if (!SceneParser::parse(sceneFilePath, renderData)) {
        std::cerr << "Failed to initialize scene: failed to parse scene file" << std::endl;
        return {nullptr};
    }
    if (renderData.lights.size() > MAX_LIGHTS) {
        std::cerr << "Failed to initialize scene: too many lights" << std::endl;
        return {nullptr};
    }


    SceneCameraData cameraData;

    // Position the camera high above the map
    cameraData.pos = glm::vec4(1.5, 10, 1, 1.0f);

    // Look straight down at the center of the map (or slightly forward)
    cameraData.look = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f); // Direction vector pointing downward

    // Up vector to maintain proper orientation
    cameraData.up = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f); // "Up" is forward relative to the map

    // Set the camera's field of view
    cameraData.heightAngle = glm::radians(60.0f); // 60-degree vertical field of view

    // Optional: Depth of field parameters
    cameraData.aperture = 0.0f;    // No depth of field effect
    cameraData.focalLength = 50.0f; // Default focal length

    // all this below initialization must be done in this factory function, not the constructor, due to needing to pass a shared_ptr
    // to this scene to the objects


    auto newScene = std::shared_ptr<RealtimeScene>(new RealtimeScene(width, height, nearPlane, farPlane, renderData.globalData, cameraData, std::move(meshes)));
    newScene->m_taken_damage = taken_damage;
    newScene->m_enemy_spawn_start = std::chrono::steady_clock::now() + std::chrono::milliseconds(GRACE_PERIOD_MS);
    // All initialization must be done here since a shared_ptr to this scene is required.
    newScene->m_lights->reserve(MAX_LIGHTS);

    // Add static collidable objects from the parsed scene.
    newScene->m_objects.reserve(renderData.shapes.size() + 1);
    newScene->m_collisionObjects.reserve(renderData.shapes.size() + 1);

    // for (const auto& shape : renderData.shapes) {
    //     auto staticObject = std::make_shared<StaticObject>(shape, newScene);
    //     auto collisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(staticObject));
    //     auto object = std::static_pointer_cast<RealtimeObject>(staticObject);
    //     newScene->m_objects.push_back(object);
    //     newScene->m_collisionObjects.push_back(collisionObject);
    // }

    // Create player object
    SceneMaterial defaultMaterial; // Ensure SceneMaterial has a default constructor
    ScenePrimitive playerPrimitive{PrimitiveType::PRIMITIVE_CUBE, defaultMaterial};

    // Start player scaled up by 1 unit and centered at the camera position
    glm::mat4 playerCTM = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.f)), newScene->m_camera->pos());
    RenderShapeData playerShapeData = RenderShapeData{playerPrimitive, playerCTM};

    newScene->m_playerObject = std::make_shared<PlayerObject>(playerShapeData, newScene, newScene->m_camera, newScene->m_lights);
    auto playerCollisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(newScene->m_playerObject));
    auto playerRealtimeObject = std::static_pointer_cast<RealtimeObject>(newScene->m_playerObject);
    newScene->m_objects.push_back(playerRealtimeObject);
    newScene->m_collisionObjects.push_back(playerCollisionObject);


  
    // Generate and add the procedural city to the scene
    int cityRows = 3;   // Number of rows for the city grid
    int cityCols = 3;   // Number of columns for the city grid
    float citySpacing = 5.0f; // Spacing between buildings
    std::cout<<"init"<<std::endl;
    if (RealtimeScene::m_activeGrids.find({0,0}) == m_activeGrids.end())
    {

    newScene->generateProceduralCity(0,0,cityRows, cityCols, citySpacing);

    }

    RealtimeScene::m_activeGrids.insert({0, 0});

    for (const auto& light : renderData.lights) {
        // normalizing is important (and faster to do here than on the gpu for every fragment)
        newScene->m_lights->push_back({light.id, light.type, light.color, light.function, light.pos, glm::normalize(light.dir),
                              light.penumbra, light.angle, light.width, light.height});
    }
    //Create skybox object
    ScenePrimitive skyboxPrimitive{PrimitiveType::PRIMITIVE_SKYBOX,
        SceneMaterial{SceneColor{0.f, 0.1f, 0.1f, 1.f}, SceneColor{1.f, 1.f, 1.f, 1.f}}};
    skyboxPrimitive.material.textureMap.isUsed = true;
    skyboxPrimitive.material.textureMap.filename = "scenefiles/moretextures/stars.png";

    skyboxPrimitive.material.blend = 0.5f;  // Adjust blend factor as needed
    skyboxPrimitive.material.textureMap.repeatU = 1.0f;  // Set U repeat value
    skyboxPrimitive.material.textureMap.repeatV = 1.0f;  // Set V repeat value

    glm::mat4 skyboxCTM = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));  // Scale skybox to surround the scene
    RenderShapeData skyboxShapeData = RenderShapeData{skyboxPrimitive, skyboxCTM};
    auto skyboxObject = std::make_shared<SkyboxObject>(skyboxShapeData, newScene, newScene->m_camera);
    auto skyboxObjectRealtime = std::static_pointer_cast<RealtimeObject>(skyboxObject);
    newScene->m_objects.push_back(skyboxObjectRealtime);
    //Add texture for skybox
    return newScene;
}

std::vector<std::shared_ptr<EnemyObject>> RealtimeScene::createEnemies(std::vector<glm::vec3> enemy_positions,
    std::shared_ptr<RealtimeScene> scene)
{
    std::vector<std::shared_ptr<EnemyObject>> enemies;
    for(glm::vec3 position : enemy_positions)
    {
        ScenePrimitive enemyPrimitive{PrimitiveType::PRIMITIVE_CYLINDER, enemy_materials::enemyMaterial};
        // start player scaled up by 1 (i.e. 1 unit wide); start player centered at camera
        glm::mat4 enemyCTM = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.f,1.f,1.f)), position);
        RenderShapeData enemyShapeData = RenderShapeData{enemyPrimitive, enemyCTM};

        enemies.push_back(std::make_shared<EnemyObject>(enemyShapeData, scene, m_camera, m_taken_damage));

    }
    return enemies;
}



RealtimeScene::RealtimeScene(int width, int height, float nearPlane, float farPlane, SceneGlobalData globalData, SceneCameraData cameraData,
                             std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes) :
    m_width(width), m_height(height), m_globalData(globalData),
    m_camera(std::make_shared<Camera>(width, height, cameraData, nearPlane, farPlane)),
    m_nearPlane(nearPlane), m_farPlane(farPlane),
    m_meshes(std::move(meshes)), m_lights(std::make_shared<std::vector<SceneLightData>>(0)) {}

void RealtimeScene::tick(double elapsedSeconds) {
    //static double accumulatedTime = 0.0;
    //super::tick(elapsedSeconds);
    size_t currentSize = m_objects.size();
    for (int i = 0; i < currentSize; i++) {
         m_objects[i]->tick(elapsedSeconds);
        // size of the vector may change during the tick, so we need to check if the object is still valid
        currentSize = m_objects.size();
    }

    // https://stackoverflow.com/a/7958447
    m_objects.erase(
        std::remove_if(m_objects.begin(), m_objects.end(),
                       [](const std::shared_ptr<RealtimeObject>& o) { return o->isQueuedFree(); }),
        m_objects.end());
    // simply remove all empty elements from collisionObjects
    m_collisionObjects.erase(
        std::remove_if(m_collisionObjects.begin(), m_collisionObjects.end(),
                       [](const std::weak_ptr<CollisionObject>& o) { return o.expired(); }),
        m_collisionObjects.end());
    //size_t currentSize = m_objects.size();
    for (int i = 0; i < currentSize; i++) {
        m_objects[i]->tick(elapsedSeconds);
        currentSize = m_objects.size();
    }

    m_objects.erase(
        std::remove_if(m_objects.begin(), m_objects.end(),
                       [](const std::shared_ptr<RealtimeObject>& o) { return o->isQueuedFree(); }),
        m_objects.end());
    m_collisionObjects.erase(
        std::remove_if(m_collisionObjects.begin(), m_collisionObjects.end(),
                       [](const std::weak_ptr<CollisionObject>& o) { return o.expired(); }),
        m_collisionObjects.end());

    // Update the city dynamically based on the player's position


    updateDynamicCity(m_camera->pos(), 2);

    //logic for determining when to spawn
    if (std::chrono::steady_clock::now() - m_time_last_spawn > std::chrono::milliseconds(TIME_BETWEEN_SPAWNS_MS))
    {
        m_time_last_spawn = std::chrono::steady_clock::now();
        spawnEnemiesInGrids();
    }
    //logic for difficulty scaling over time
    if (std::chrono::steady_clock::now() > m_enemy_spawn_start  && std::chrono::steady_clock::now() - m_time_last_scaling > std::chrono::seconds(TIME_TO_INCREMENT_SPAWN_S))
    {
        current_difficulty_scaling += 1;
        m_time_last_scaling = std::chrono::steady_clock::now();
        if (current_difficulty_scaling * INCREMENT + PROBABILITY_OF_SPAWN < 1) {
            std::cout << "Things are heating up!" << std::endl;
        }
        else
        {
            std::cout << "Max difficulty reached. Let's see how long you survive..." << std::endl;
        }
    }
}

void RealtimeScene::paintObjects() {
    if (!shaderInitialized()) {
        std::cerr << "Failed to paint objects: shader not initialized" << std::endl;
        return;
    }

    glUseProgram(*m_phongShader);
    passUniformMat4("view", m_camera->viewMatrix());
    passUniformMat4("proj", m_camera->projectionMatrix());
    passUniformInt("numLights", (int) m_lights->size());
    passUniformLightArray("lights", m_lights);
    passUniformVec3("cameraPosWS", m_camera->pos().xyz());
    passUniformFloat("ka", m_globalData.ka);
    passUniformFloat("kd", m_globalData.kd);
    passUniformFloat("ks", m_globalData.ks);
    // set texture slot
    glActiveTexture(GL_TEXTURE0);
    for (const auto& object : m_objects) {
        if (!object->shouldRender()) {
            continue;
        }
        const SceneMaterial& material = object->material();
        if (object->usesTexture()) {
            if (!object->glTexAllocated()) {
                object->allocateGLTex();
            }
            glBindTexture(GL_TEXTURE_2D, object->glTexID());
            passUniformInt("usesTexture", 1);
            // TODO is it okay to sometimes not pass these uniforms?
            passUniformFloat("blend", material.blend);
            passUniformFloat("repeatU", material.textureMap.repeatU);
            passUniformFloat("repeatV", material.textureMap.repeatV);
        } else {
            passUniformInt("usesTexture", 0);
        }
        passUniformMat4("model", object->CTM());
        passUniformMat3("inverseTransposeModel", object->inverseTransposeCTM());
        passUniformVec3("cAmbient", material.cAmbient.xyz());
        passUniformVec3("cDiffuse", material.cDiffuse.xyz());
        passUniformVec3("cSpecular", material.cSpecular.xyz());
        passUniformFloat("shininess", material.shininess);
        glBindVertexArray(object->mesh()->vao());
        if (object->type() == PrimitiveType::PRIMITIVE_SKYBOX)
        {
            passUniformInt("isSkybox", 1);
        } else
        {
            passUniformInt("isSkybox", 0);
        }
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (object->mesh()->vertexData().size() / 3));
        glBindVertexArray(0);
        if (object->usesTexture()) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glUseProgram(0);
}




void RealtimeScene::passUniformMat4(const char* name, const glm::mat4& mat) {
    helpers::passUniformMat4(m_phongShader.value(), name, mat);
}

void RealtimeScene::passUniformMat3(const char* name, const glm::mat3& mat) {
    helpers::passUniformMat3(m_phongShader.value(), name, mat);
}

void RealtimeScene::passUniformFloat(const char* name, float value) {
    helpers::passUniformFloat(m_phongShader.value(), name, value);
}

void RealtimeScene::passUniformInt(const char* name, int value) {
    helpers::passUniformInt(m_phongShader.value(), name, value);
}

void RealtimeScene::passUniformVec3(const char* name, const glm::vec3& vec) {
    helpers::passUniformVec3(m_phongShader.value(), name, vec);
}

void RealtimeScene::passUniformVec3Array(const char* name, const std::vector<glm::vec3>& vecs) {
    helpers::passUniformVec3Array(m_phongShader.value(), name, vecs);
}

void RealtimeScene::passUniformLightArray(const char* name, std::shared_ptr<std::vector<SceneLightData>> lights) {
    // hmm this will cause allocations every time TODO maybe optimize
    std::string lightName = std::string(name);
    for (int i = 0; i < lights->size(); i++) {
        passUniformLight((lightName + "[" + std::to_string(i) + "]").c_str(), (*lights)[i]);
    }
}

void RealtimeScene::passUniformLight(const char* name, SceneLightData type) {
    // hmm this will cause allocations every time TODO maybe optimize
    std::string lightName = std::string(name);
    glUniform1ui(getUniformLocation((lightName + ".type").c_str()), lightTypeToUniform(type.type));
    glUniform3fv(getUniformLocation((lightName + ".color").c_str()), 1, &type.color.xyz()[0]);
    glUniform3fv(getUniformLocation((lightName + ".pos").c_str()), 1, &type.pos.xyz()[0]);
    glUniform3fv(getUniformLocation((lightName + ".dir").c_str()), 1, &type.dir.xyz()[0]);
    glUniform3fv(getUniformLocation((lightName + ".function").c_str()), 1, &type.function[0]);
    glUniform1f(getUniformLocation((lightName + ".penumbra").c_str()), type.penumbra);
    glUniform1f(getUniformLocation((lightName + ".angle").c_str()), type.angle);
}

GLint RealtimeScene::getUniformLocation(const char* name, bool checkValidLoc) const {
    return helpers::getUniformLocation(m_phongShader.value(), name, checkValidLoc);
}

void RealtimeScene::setDimensions(int width, int height) {
    m_width = width;
    m_height = height;
    m_camera->setSceneDimensions(width, height);
}


void RealtimeScene::updateSettings(float nearPlane, float farPlane) {
    if (farPlane != m_farPlane || nearPlane != m_nearPlane) {
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        m_camera->setNearAndFar(nearPlane, farPlane);
    }
}

void RealtimeScene::initShader(GLuint phongShader) {
    m_phongShader = phongShader;
}

bool RealtimeScene::shaderInitialized() const {
    return m_phongShader.has_value();
}


int RealtimeScene::width() const {
    return m_width;
}

int RealtimeScene::height() const {
    return m_height;
}

GLuint RealtimeScene::lightTypeToUniform(LightType type) {
    switch (type) {
    case LightType::LIGHT_POINT:
        return SHADER_LIGHT_POINT;
    case LightType::LIGHT_DIRECTIONAL:
        return SHADER_LIGHT_DIRECTIONAL;
    case LightType::LIGHT_SPOT:
        return SHADER_LIGHT_SPOT;
    default:
        throw std::runtime_error("Invalid light type");
    }
}

void RealtimeScene::keyPressEvent(int key) {
    m_playerObject->keyPressEvent(key);
}

void RealtimeScene::keyReleaseEvent(int key) {
    m_playerObject->keyReleaseEvent(key);
}

void RealtimeScene::mousePressEvent(int button) {
    m_playerObject->mousePressEvent(button);

}

void RealtimeScene::mouseReleaseEvent(int button) {
    m_playerObject->mouseReleaseEvent(button);
}

void RealtimeScene::mouseMoveEvent(double xpos, double ypos) {
    m_playerObject->mouseMoveEvent(xpos, ypos);
}

std::shared_ptr<RealtimeObject> RealtimeScene::addObject(PrimitiveType type, const glm::mat4& ctm, const SceneMaterial& material,
                                                         RealtimeObjectType objType) {
    switch (objType) {

        case RealtimeObjectType::OBJECT:
            return addObject(std::make_unique<RealtimeObject>(RenderShapeData{ScenePrimitive{type, material}, ctm}, shared_from_this()));
        case RealtimeObjectType::STATIC:
            return addObject(std::make_unique<StaticObject>(RenderShapeData{ScenePrimitive{type, material}, ctm}, shared_from_this()));

    }
    throw std::runtime_error("Invalid object type");
}

void RealtimeScene::addEnemy(glm::vec3 position) {
    // std::cout <<" addenemy" <<std::endl;
    ScenePrimitive enemyPrimitive{PrimitiveType::PRIMITIVE_CYLINDER, enemy_materials::enemyMaterial};
    // start player scaled up by 1 (i.e. 1 unit wide); start player centered at camera
    glm::mat4 enemyCTM = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.f,1.f,1.f)), position);
    RenderShapeData enemyShapeData = RenderShapeData{enemyPrimitive, enemyCTM};

    auto enemy = new EnemyObject(enemyShapeData, shared_from_this(), m_camera, m_taken_damage);

    auto enemy_obj = static_cast<RealtimeObject*>(enemy);
    addObject(std::unique_ptr<RealtimeObject>(enemy_obj));
}

std::shared_ptr<RealtimeObject> RealtimeScene::addObject(std::unique_ptr<RealtimeObject> object) {
    std::shared_ptr<RealtimeObject> objectShared = std::move(object);
    if (!objectShared) {
        std::cerr << "Failed to add object to scene: object is null" << std::endl;
        return {nullptr};
    }
    // tests if object is a subclass of CollisionObject
    // if so, we have to add it to the collision objects list
    std::shared_ptr<CollisionObject> maybeCollisionObject = std::dynamic_pointer_cast<CollisionObject>(objectShared);
    if (maybeCollisionObject) {
        std::weak_ptr<CollisionObject> collisionObject = std::weak_ptr<CollisionObject>(maybeCollisionObject);
        m_collisionObjects.push_back(collisionObject);
    }
    m_objects.push_back(objectShared);
    return objectShared;
}


const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& RealtimeScene::meshes() const {
    return m_meshes;
}

const std::vector<std::weak_ptr<CollisionObject>>& RealtimeScene::collisionObjects() const {
    return m_collisionObjects;
}




void RealtimeScene::generateProceduralCity(int gridX, int gridZ, int rows, int cols, float spacing) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> heightDist(1.0f, 15.0f);
    std::uniform_real_distribution<float> widthDist(1.0f, 3.0f);
    std::uniform_real_distribution<float> depthDist(1.0f, 3.0f);

    float baseX = gridX * cols * spacing;
    float baseZ = gridZ * rows * spacing;

    // find location to spawn enemy
    if (std::chrono::steady_clock::now() > m_enemy_spawn_start) {
        // std::cout << "adding " << gridX << ", " << gridZ << std::endl;
        m_enemy_spawn_locations[glm::vec2(gridX,gridZ)] = glm::vec3(baseX, 0.0f, baseZ);
    }

    float floorWidth = cols * spacing;
    float floorDepth = rows * spacing;
    float floorHeight = 0.1f;

    glm::vec3 floorPosition(
        baseX + floorWidth / 2.0f - spacing / 2.0f,
        -floorHeight / 2.0f - 3.f,
        baseZ + floorDepth / 2.0f - spacing / 2.0f
        );

    glm::mat4 floorTransform = glm::translate(glm::mat4(1.0f), floorPosition) *
                               glm::scale(glm::mat4(1.0f), glm::vec3(floorWidth, floorHeight, floorDepth));

    SceneMaterial floorMaterial;
    floorMaterial.cDiffuse = SceneColor(0.2f, 0.2f, 0.2f, 1.0f);
    floorMaterial.cAmbient = SceneColor(0.1f, 0.1f, 0.1f, 1.0f);
    floorMaterial.cSpecular = SceneColor(0.2f, 0.2f, 0.2f, 1.0f);
    floorMaterial.shininess = 5.0f;

    floorMaterial.textureMap.isUsed = true;
    floorMaterial.textureMap.filename = "scenefiles/moretextures/doomfloor.jpg";

    floorMaterial.blend = 0.5f;
    floorMaterial.textureMap.repeatU = 1.0f;
    floorMaterial.textureMap.repeatV = 1.0f;

    // add floor
    addObject(PrimitiveType::PRIMITIVE_CUBE, floorTransform, floorMaterial, RealtimeObjectType::STATIC);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int globalX = gridX * rows + i;
            int globalZ = gridZ * cols + j;
            std::pair<int, int> gridCoord = {globalX, globalZ};

            if (existingBuildings.find(gridCoord) != existingBuildings.end()) {
                continue;
            }

            float height = heightDist(generator);
            float width = widthDist(generator);
            float depth = depthDist(generator);

            glm::vec3 position(
                baseX + i * spacing,
                height / 2.0f - 3,
                baseZ + j * spacing
                );
            //std::cout << "Building position: (" << position.x << ", " << position.y << ", " << position.z << ")\n";

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(width, height, depth));

            SceneMaterial material;
            material.cDiffuse = SceneColor(0.3f, 0.3f, 0.3f, 1.0f);
            material.cAmbient = SceneColor(0.1f, 0.1f, 0.1f, 1.0f);
            material.cSpecular = SceneColor(0.5f, 0.5f, 0.5f, 1.0f);
            material.shininess = 10.0f;
            material.textureMap.isUsed = true;
            material.textureMap.filename = "scenefiles/moretextures/city.jpg";

            material.blend = 0.5f;
            material.textureMap.repeatU = ((width + depth) / 2.f) / 5.f;
            material.textureMap.repeatV = (height) / 5.f;

            // add building
            // std::cout << "Adding building at " << position.x << ", " << position.y << ", " << position.z << std::endl;
            // std::cout << "Total existing buildings: " << existingBuildings.size() << std::endl;
            // std::cout << "Total active grids: " << m_activeGrids.size() << std::endl;
            // std::cout << "Total objects: " << m_objects.size() << std::endl;
            addObject(PrimitiveType::PRIMITIVE_CUBE, transform, material, RealtimeObjectType::STATIC);

            existingBuildings.insert(gridCoord);
        }
    }
}
void RealtimeScene::spawnEnemiesInGrids()
{
    //some junk to get a random float between 0 and 1
    std::mt19937 gen(std::random_device{}());
    // std::cout << m_enemy_spawn_locations.size() << std::endl;

    for (auto& pair : m_enemy_spawn_locations) {
        if (std::uniform_real_distribution<float>(0.0, 1.0)(gen) < PROBABILITY_OF_SPAWN + (current_difficulty_scaling * INCREMENT))
        {
            addEnemy(pair.second); //add enemy at 3
        }
    }
}

void RealtimeScene::removeGridObjects(int gridX, int gridZ, int rows, int cols) {
    float spacing=5.f;
    float baseX = gridX * cols * spacing;
    float baseZ = gridZ * rows * spacing;

    for (auto it = m_objects.begin(); it != m_objects.end();) {
        glm::vec3 objPos = (*it)->pos();
        if (objPos.x >= baseX && objPos.x < baseX + cols * spacing &&
            objPos.z >= baseZ && objPos.z < baseZ + rows * spacing) {
            (*it)->queueFree();
            it = m_objects.erase(it);
        } else {
            ++it;
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            existingBuildings.erase({gridX * rows + i, gridZ * cols + j});
        }
    }
}

void RealtimeScene::updateDynamicCity(const glm::vec3& playerPosition, int gridCellUpdateDist) {
    // std::cout << m_activeGrids.size() << std::endl;
    // printActiveGrids(m_activeGrids);

    float spacing = 5.f;
    int rows = 3;
    int cols = 3;

    // determine which grid the player is in
    int playerGridX = static_cast<int>(playerPosition.x / (cols * spacing));
    int playerGridZ = static_cast<int>(playerPosition.z / (rows * spacing));

    // std::cout << "player is in " << playerGridX  << " ," << playerGridZ << std::endl;

    int startDelta = -gridCellUpdateDist;
    int endDelta = gridCellUpdateDist;

    for (int dx = startDelta; dx <= endDelta; dx++) {
        for (int dz = startDelta; dz <= endDelta; dz++) {
            int neighborGridX = playerGridX + dx;
            int neighborGridZ = playerGridZ + dz;

            // float gridCenterX = neighborGridX * cols * spacing + cols * spacing / 2.0f;
            // float gridCenterZ = neighborGridZ * rows * spacing + rows * spacing / 2.0f;

            // float dist = glm::distance(playerPosition, glm::vec3(gridCenterX, 0, gridCenterZ));

            if (m_activeGrids.find({neighborGridX, neighborGridZ}) == m_activeGrids.end()) {
                generateProceduralCity(neighborGridX, neighborGridZ, rows, cols, spacing);
                m_activeGrids.insert({neighborGridX, neighborGridZ});
                // std::cout << "Activated grid: (" << neighborGridX << ", " << neighborGridZ << ", dist: " << dist << ")\n";
            }
        }
    }

    // remove grids that are too far from the player
    auto it = m_activeGrids.begin();
    while (it != m_activeGrids.end()) {
        int gridX = it->first;
        int gridZ = it->second;

        int manhanttanGridDistFromPlayer = std::abs(playerGridX - gridX) + std::abs(playerGridZ - gridZ);

        // deactivate the grid if it is too far
        if (manhanttanGridDistFromPlayer > gridCellUpdateDist * 2) {
            removeGridObjects(gridX, gridZ, rows, cols);
            it = m_activeGrids.erase(it);

            //remove the grid's coordinates from the vector of spawn locations
            // std::cout << "removing " << gridX << ", " << gridZ << std::endl;
            // m_enemy_spawn_locations.erase(glm::vec2(gridX, gridZ));
            // std::cout << "Deactivated grid: (" << gridX << ", " << gridZ << ", dist: " << dist << ", updateRadius: " << updateRadius << ")\n";
        } else {
            ++it;
        }
    }
}

void RealtimeScene::finish() {
    for (const auto& object : m_objects) {
        object->finish();
    }
}




#pragma clang diagnostic pop

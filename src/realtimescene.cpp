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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantParameter"
#define MAX_LIGHTS 8

const unsigned int SHADER_LIGHT_POINT       = 0x1u;
const unsigned int SHADER_LIGHT_DIRECTIONAL = 0x2u;
const unsigned int SHADER_LIGHT_SPOT        = 0x4u;

void printActiveGrids(const std::unordered_set<std::pair<int, int>, pair_hash>& activeGrids) {
    std::cout << "Active Grids:" << std::endl;
    for (const auto& grid : activeGrids) {
        std::cout << "(" << grid.first << ", " << grid.second << ")" << std::endl;
    }
}

// std::shared_ptr<RealtimeScene> RealtimeScene::init(int width, int height, const std::string& sceneFilePath,
//                                                  float nearPlane, float farPlane,
//                                                  std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes) {
//     RenderData renderData;
//     if (!SceneParser::parse(sceneFilePath, renderData)) {
//         std::cerr << "Failed to initialize scene: failed to parse scene file" << std::endl;
//         return {nullptr};
//     }
//     if (renderData.lights.size() > MAX_LIGHTS) {
//         std::cerr << "Failed to initialize scene: too many lights" << std::endl;
//         return {nullptr};
//     }
//     auto newScene = std::shared_ptr<RealtimeScene>(new RealtimeScene(width, height, nearPlane, farPlane, renderData.globalData, renderData.cameraData, std::move(meshes)));

//     // all this below initialization must be done in this factory function, not the constructor, due to needing to pass a shared_ptr
//     // to this scene to the objects

//     // TODO currently all objects imported from the scene file will be static collidable objects.
//     //  ideally we'd extend the scene format to allow it to specify what physics properties an object has
//     newScene->m_objects.reserve(renderData.shapes.size() + 1);
//     newScene->m_collisionObjects.reserve(renderData.shapes.size() + 1);
//     for (const auto& shape : renderData.shapes) {
//         auto staticObject = std::make_shared<StaticObject>(shape, newScene);
//         auto collisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(staticObject));
//         auto object = std::static_pointer_cast<RealtimeObject>(staticObject);
//         newScene->m_objects.push_back(object);
//         newScene->m_collisionObjects.push_back(collisionObject);
//     }
//     // create player object TODO better way of doing this
//     SceneMaterial defaultMaterial; // Ensure SceneMaterial has a default constructor
//     ScenePrimitive playerPrimitive{PrimitiveType::PRIMITIVE_CUBE, defaultMaterial};

//     // start player scaled up by 1 (i.e. 1 unit wide); start player centered at camera
//     glm::mat4 playerCTM = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.f)), newScene->m_camera->pos());
//     RenderShapeData playerShapeData = RenderShapeData(playerPrimitive,playerCTM);
//     newScene->m_playerObject = std::make_shared<PlayerObject>(playerShapeData, newScene, newScene->m_camera);
//     auto playerCollisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(newScene->m_playerObject));
//     auto playerRealtimeObject = std::static_pointer_cast<RealtimeObject>(newScene->m_playerObject);
//     newScene->m_objects.push_back(playerRealtimeObject);
//     newScene->m_collisionObjects.push_back(playerCollisionObject);

//     newScene->m_lights.reserve(MAX_LIGHTS);
//     for (const auto& light : renderData.lights) {
//         // normalizing is important (and faster to do here than on the gpu for every fragment)
//         newScene->m_lights.emplace_back(light.id, light.type, light.color, light.function, light.pos, glm::normalize(light.dir),
//                               light.penumbra, light.angle, light.width, light.height);
//     }
//     //newScene=generateProceduralCity();
//     return newScene;
// }
 //std::unordered_set<std::pair<int, int>, pair_hash> RealtimeScene::activeGrids;
std::unordered_set<std::pair<int, int>, pair_hash> RealtimeScene::m_activeGrids;

std::shared_ptr<RealtimeScene> RealtimeScene::init(int width, int height, const std::string& sceneFilePath,
                                                   float nearPlane, float farPlane,
                                                   std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes) {
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
    cameraData.pos = glm::vec4(1, 10, 1, 1.0f);

    // Look straight down at the center of the map (or slightly forward)
    cameraData.look = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f); // Direction vector pointing downward

    // Up vector to maintain proper orientation
    cameraData.up = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f); // "Up" is forward relative to the map

    // Set the camera's field of view
    cameraData.heightAngle = glm::radians(45.0f); // 45-degree vertical field of view

    // Optional: Depth of field parameters
    cameraData.aperture = 0.0f;    // No depth of field effect
    cameraData.focalLength = 50.0f; // Default focal length

    auto newScene = std::shared_ptr<RealtimeScene>(new RealtimeScene(width, height, nearPlane, farPlane, renderData.globalData, cameraData, std::move(meshes)));

    // All initialization must be done here since a shared_ptr to this scene is required.

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
    RenderShapeData playerShapeData = RenderShapeData(playerPrimitive, playerCTM);
    newScene->m_playerObject = std::make_shared<PlayerObject>(playerShapeData, newScene, newScene->m_camera);
    auto playerCollisionObject = std::weak_ptr<CollisionObject>(std::static_pointer_cast<CollisionObject>(newScene->m_playerObject));
    auto playerRealtimeObject = std::static_pointer_cast<RealtimeObject>(newScene->m_playerObject);
    newScene->m_objects.push_back(playerRealtimeObject);
    newScene->m_collisionObjects.push_back(playerCollisionObject);

    // Add lights to the scene
    newScene->m_lights.reserve(MAX_LIGHTS);
    for (const auto& light : renderData.lights) {
        // Normalize light direction for performance and accuracy
        newScene->m_lights.emplace_back(light.id, light.type, light.color, light.function, light.pos, glm::normalize(light.dir),
                                        light.penumbra, light.angle, light.width, light.height);
    }

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

    return newScene;
}



RealtimeScene::RealtimeScene(int width, int height, float nearPlane, float farPlane, SceneGlobalData globalData, SceneCameraData cameraData,
                             std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>> meshes) :
    m_width(width), m_height(height), m_globalData(globalData),
    m_camera(std::make_shared<Camera>(width, height, cameraData, nearPlane, farPlane)),
    m_nearPlane(nearPlane), m_farPlane(farPlane),
    m_meshes(std::move(meshes)) {}

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

    // Add elapsed time to the accumulator

    updateDynamicCity(m_camera->pos(), 10.0f);




}

void RealtimeScene::paintObjects() {
    if (!shaderInitialized()) {
        std::cerr << "Failed to paint objects: shader not initialized" << std::endl;
        return;
    }
    glUseProgram(*m_shader);
    passUniformMat4("view", m_camera->viewMatrix());
    passUniformMat4("proj", m_camera->projectionMatrix());
    passUniformInt("numLights", (int) m_lights.size());
    passUniformLightArray("lights", m_lights);
    passUniformVec3("cameraPosWS", m_camera->pos().xyz());
    passUniformFloat("ka", m_globalData.ka);
    passUniformFloat("kd", m_globalData.kd);
    passUniformFloat("ks", m_globalData.ks);
    for (const auto& object : m_objects) {
        if (!object->shouldRender()) {
            continue;
        }
        const SceneMaterial& material = object->material();
        passUniformMat4("model", object->CTM());
        passUniformMat3("inverseTransposeModel", object->inverseTransposeCTM());
        passUniformVec3("cAmbient", material.cAmbient.xyz());
        passUniformVec3("cDiffuse", material.cDiffuse.xyz());
        passUniformVec3("cSpecular", material.cSpecular.xyz());
        passUniformFloat("shininess", material.shininess);
        glBindVertexArray(object->mesh()->vao());
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (object->mesh()->vertexData().size() / 3));
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

void RealtimeScene::passUniformMat4(const char* name, const glm::mat4& mat) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void RealtimeScene::passUniformMat3(const char* name, const glm::mat3& mat) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void RealtimeScene::passUniformFloat(const char* name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void RealtimeScene::passUniformInt(const char* name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void RealtimeScene::passUniformVec3(const char* name, const glm::vec3& vec) {
    glUniform3fv(getUniformLocation(name), 1, &vec[0]);
}

void RealtimeScene::passUniformVec3Array(const char* name, const std::vector<glm::vec3>& vecs) {
    glUniform3fv(getUniformLocation(name), (GLint) vecs.size(), &vecs[0][0]);
}

void RealtimeScene::passUniformLightArray(const char* name, const std::vector<SceneLightData>& lights) {
    // hmm this will cause allocations every time TODO maybe optimize
    std::string lightName = std::string(name);
    for (int i = 0; i < lights.size(); i++) {
        passUniformLight((lightName + "[" + std::to_string(i) + "]").c_str(), lights[i]);
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantConditionsOC"
GLint RealtimeScene::getUniformLocation(const char* name, bool checkValidLoc) const {
    GLint loc = glGetUniformLocation(*m_shader, name);
    if (checkValidLoc && loc == -1) {
        std::cerr << "Could not find uniform " << name << " in shader program" << std::endl;
    }
    return loc;
}
#pragma clang diagnostic pop

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

void RealtimeScene::initShader(GLuint shader) {
    m_shader = shader;
}

bool RealtimeScene::shaderInitialized() const {
    return m_shader.has_value();
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
        return addObject(std::make_unique<RealtimeObject>(RenderShapeData(ScenePrimitive{type, material}, ctm), shared_from_this()));
    case RealtimeObjectType::STATIC:
        return addObject(std::make_unique<StaticObject>(RenderShapeData(ScenePrimitive{type, material}, ctm), shared_from_this()));
    }
    throw std::runtime_error("Invalid object type");
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
    std::uniform_real_distribution<float> heightDist(1.0f, 15.0f); // Random heights
    std::uniform_real_distribution<float> widthDist(1.0f, 3.0f);  // Random widths
    std::uniform_real_distribution<float> depthDist(1.0f, 3.0f);  // Random depths

    // Calculate base position for the grid
    float baseX = gridX * cols * spacing;
    float baseZ = gridZ * rows * spacing;

    // Create the floor for the grid
    float floorWidth = cols * spacing; // Width spans the entire grid
    float floorDepth = rows * spacing; // Depth spans the entire grid
    float floorHeight = 0.1f;          // Thin floor

    glm::vec3 floorPosition(
        baseX + floorWidth / 2.0f - spacing / 2.0f,  // Center the floor in the X-axis
        -floorHeight / 2.0f - 3,                    // Position floor at ground level
        baseZ + floorDepth / 2.0f - spacing / 2.0f  // Center the floor in the Z-axis
        );

    glm::mat4 floorTransform = glm::translate(glm::mat4(1.0f), floorPosition) *
                               glm::scale(glm::mat4(1.0f), glm::vec3(floorWidth, floorHeight, floorDepth));

    SceneMaterial floorMaterial; // Material for the floor
    floorMaterial.cDiffuse = SceneColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray color
    floorMaterial.cAmbient = SceneColor(0.1f, 0.1f, 0.1f, 1.0f);
    floorMaterial.cSpecular = SceneColor(0.2f, 0.2f, 0.2f, 1.0f);
    floorMaterial.shininess = 5.0f;

    // Add the floor to the scene
    addObject(PrimitiveType::PRIMITIVE_CUBE, floorTransform, floorMaterial, RealtimeObjectType::STATIC);

    // Create buildings in the grid
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Calculate global grid coordinates
            int globalX = gridX * rows + i;
            int globalZ = gridZ * cols + j;
            std::pair<int, int> gridCoord = {globalX, globalZ};

            // Check if the building already exists
            if (existingBuildings.find(gridCoord) != existingBuildings.end()) {
                continue;
            }

            float height = heightDist(generator);
            float width = widthDist(generator);
            float depth = depthDist(generator);

            glm::vec3 position(
                baseX + i * spacing,
                height / 2.0f - 3, // Place the building at half its height to align it with the ground
                baseZ + j * spacing
                );
            //std::cout << "Building position: (" << position.x << ", " << position.y << ", " << position.z << ")\n";

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(width, height, depth));

            SceneMaterial material; // Default material for buildings
            material.cDiffuse = SceneColor(0.3f, 0.3f, 0.3f, 1.0f); // Gray color
            material.cAmbient = SceneColor(0.1f, 0.1f, 0.1f, 1.0f);
            material.cSpecular = SceneColor(0.5f, 0.5f, 0.5f, 1.0f);
            material.shininess = 10.0f;

            // Add building to the scene
            addObject(PrimitiveType::PRIMITIVE_CUBE, transform, material, RealtimeObjectType::STATIC);

            // Mark the grid coordinate as having a building
            existingBuildings.insert(gridCoord);
        }
    }
}


void RealtimeScene::removeGridObjects(int gridX, int gridZ, int rows, int cols) {
    float spacing=5.f;
    float baseX = gridX * cols * spacing;
    float baseZ = gridZ * rows * spacing;

    for (auto it = m_objects.begin(); it != m_objects.end();) {
        glm::vec3 objPos = (*it)->pos(); // Ensure `pos()` gives object position
        if (objPos.x >= baseX && objPos.x < baseX + cols * spacing &&
            objPos.z >= baseZ && objPos.z < baseZ + rows * spacing) {
            (*it)->queueFree();
            it = m_objects.erase(it);
        } else {
            ++it;
        }
    }

    // Remove grid coordinates from existingBuildings
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            existingBuildings.erase({gridX * rows + i, gridZ * cols + j});
        }
    }
}




// void RealtimeScene::updateDynamicCity(const glm::vec3& playerPosition, float updateRadius) {
//     printActiveGrids(m_activeGrids);
//     float spacing = 5.f;
//     int rows = 3;
//     int cols = 3;

//     // Determine which grid the player is in
//     int playerGridX = static_cast<int>(playerPosition.x / (cols * spacing));
//     int playerGridZ = static_cast<int>(playerPosition.z / (rows * spacing));

//     // Calculate the grid's boundaries
//     float gridStartX = playerGridX * cols * spacing;
//     float gridEndX = gridStartX + cols * spacing;
//     float gridStartZ = playerGridZ * rows * spacing;
//     float gridEndZ = gridStartZ + rows * spacing;

//     // Check if the player is near the edge of the current grid
//     float edgeThreshold = spacing/2; // Threshold distance to trigger new grid loading

//     bool nearEdgeX = (playerPosition.x - gridStartX < edgeThreshold) || (gridEndX - playerPosition.x < edgeThreshold);
//     bool nearEdgeZ = (playerPosition.z - gridStartZ < edgeThreshold) || (gridEndZ - playerPosition.z < edgeThreshold);
//     //std::cout << playerPosition.z - gridStartZ  << ","<<gridEndZ - playerPosition.z<<std::endl;
//     std::cout<<playerPosition.x<<","<< playerPosition.z<<std::endl;
//     std::cout << "Player grid:" << std::endl;
//     std::cout << playerGridX << ","<<playerGridZ<<std::endl;
// //std::cout << playerPosition.z - gridStartZ << ","<<gridEndZ - playerPosition.z<<std::endl;
//     if (abs(playerPosition.z - gridStartZ) < edgeThreshold)
//     {
//         if (m_activeGrids.find({playerGridX, playerGridZ - 1}) == m_activeGrids.end())
//         {
//             std::cout << "z-" << std::endl;
//             generateProceduralCity(playerGridX, playerGridZ - 1, rows, cols, spacing);
//             m_activeGrids.insert({playerGridX, playerGridZ - 1});
//             //std::cout << "z-" << std::endl;
//         }
//     }
//     else if (abs(playerPosition.x - gridStartX) < edgeThreshold)
//     {
//         // Check if the grid location already exists
//         if (m_activeGrids.find({playerGridX - 1, playerGridZ}) == m_activeGrids.end())
//         {
//             generateProceduralCity(playerGridX - 1, playerGridZ, rows, cols, spacing);

//             std::cout << "x-" << std::endl;
//             m_activeGrids.insert({playerGridX - 1, playerGridZ});
//         }
//     }

//     else if (abs(gridEndX - playerPosition.x) < edgeThreshold)
//     {
//         if (m_activeGrids.find({playerGridX + 1, playerGridZ}) == m_activeGrids.end())
//         {
//             generateProceduralCity(playerGridX + 1, playerGridZ, rows, cols, spacing);
//             m_activeGrids.insert({playerGridX + 1, playerGridZ});
//             std::cout << "x+" << std::endl;
//         }
//     }

//     else if (abs(gridEndZ - playerPosition.z) < edgeThreshold)
//     {
//         if (m_activeGrids.find({playerGridX, playerGridZ + 1}) == m_activeGrids.end())
//         {
//             std::cout << "z+" << std::endl;
//             generateProceduralCity(playerGridX, playerGridZ + 1, rows, cols, spacing);
//             m_activeGrids.insert({playerGridX, playerGridZ + 1});

//         }
//     }



//     //Remove grids that are too far from the player
//     auto it = m_activeGrids.begin();
//     while (it != m_activeGrids.end()) {
//         int gridX = it->first;
//         int gridZ = it->second;
//         //std::cout<<m_activeGrids.first<<","<<m_activeGrids.second<<std::endl;

//         float gridCenterX = gridX * cols * spacing + cols * spacing / 2.0f;
//         float gridCenterZ = gridZ * rows * spacing + rows * spacing / 2.0f;
//         float dist = glm::distance(playerPosition, glm::vec3(gridCenterX, 0, gridCenterZ));

//         if (dist > updateRadius * 2) { // Remove if too far
//             // Remove all objects in this grid
//             removeGridObjects(gridX, gridZ, rows, cols);
//             it = m_activeGrids.erase(it);
//             std::cout << "Deactivated grid: (" << gridX << ", " << gridZ <<","<<dist<<","<<updateRadius<< ")\n";
//         } else {
//             ++it;
//         }
//     }
// }
void RealtimeScene::updateDynamicCity(const glm::vec3& playerPosition, float updateRadius) {
    printActiveGrids(m_activeGrids);

    float spacing = 5.f;
    int rows = 3;
    int cols = 3;

    // Determine which grid the player is in
    int playerGridX = static_cast<int>(playerPosition.x / (cols * spacing));
    int playerGridZ = static_cast<int>(playerPosition.z / (rows * spacing));

    // Activate grids within the updateRadius
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dz = -1; dz <= 1; ++dz) {
            int neighborGridX = playerGridX + dx;
            int neighborGridZ = playerGridZ + dz;

            // Calculate the center of the neighbor grid
            float gridCenterX = neighborGridX * cols * spacing + cols * spacing / 2.0f;
            float gridCenterZ = neighborGridZ * rows * spacing + rows * spacing / 2.0f;

            float dist = glm::distance(playerPosition, glm::vec3(gridCenterX, 0, gridCenterZ));

            // Activate the grid if within updateRadius and not already active
            if (dist <= updateRadius && m_activeGrids.find({neighborGridX, neighborGridZ}) == m_activeGrids.end()) {
                generateProceduralCity(neighborGridX, neighborGridZ, rows, cols, spacing);
                m_activeGrids.insert({neighborGridX, neighborGridZ});
                std::cout << "Activated grid: (" << neighborGridX << ", " << neighborGridZ << ", dist: " << dist << ")\n";
            }
        }
    }

    // Remove grids that are too far from the player
    auto it = m_activeGrids.begin();
    while (it != m_activeGrids.end()) {
        int gridX = it->first;
        int gridZ = it->second;

        // Calculate the center of the current grid
        float gridCenterX = gridX * cols * spacing + cols * spacing / 2.0f;
        float gridCenterZ = gridZ * rows * spacing + rows * spacing / 2.0f;

        float dist = glm::distance(playerPosition, glm::vec3(gridCenterX, 0, gridCenterZ));

        // Deactivate the grid if it is too far
        if (dist > updateRadius * 2) {
            removeGridObjects(gridX, gridZ, rows, cols);
            it = m_activeGrids.erase(it);
            std::cout << "Deactivated grid: (" << gridX << ", " << gridZ << ", dist: " << dist << ", updateRadius: " << updateRadius << ")\n";
        } else {
            ++it;
        }
    }
}








#pragma clang diagnostic pop

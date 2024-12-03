#include "scenefilereader.h"
#include "scenedata.h"

#include "glm/gtc/type_ptr.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <cassert>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <fstream>

#define ERROR_AT(e) "error at line " << e.lineNumber() << " col " << e.columnNumber() << ": "
#define PARSE_ERROR(e) std::cout << ERROR_AT(e) << "could not parse <" << e.tagName() \
                                 << ">" << std::endl
#define UNSUPPORTED_ELEMENT(e) std::cout << ERROR_AT(e) << "unsupported element <" \
                                         << e.tagName() << ">" << std::endl;

// Students, please ignore this file.
ScenefileReader::ScenefileReader(const std::string &name) {
    file_name = name;

    memset(&m_cameraData, 0, sizeof(SceneCameraData));
    memset(&m_globalData, 0, sizeof(SceneGlobalData));

    m_root = new SceneNode;

    m_templates.clear();
    m_nodes.clear();

    m_nodes.push_back(m_root);
}

ScenefileReader::~ScenefileReader() {
    // Delete all Scene Nodes
    for (unsigned int node = 0; node < m_nodes.size(); node++) {
        for (size_t i = 0; i < (m_nodes[node])->transformations.size(); i++)
        {
            delete (m_nodes[node])->transformations[i];
        }
        for (size_t i = 0; i < (m_nodes[node])->primitives.size(); i++)
        {
            delete (m_nodes[node])->primitives[i];
        }
        (m_nodes[node])->transformations.clear();
        (m_nodes[node])->primitives.clear();
        (m_nodes[node])->children.clear();
        delete m_nodes[node];
    }

    m_nodes.clear();
    m_templates.clear();
}

SceneGlobalData ScenefileReader::getGlobalData() const {
    return m_globalData;
}

SceneCameraData ScenefileReader::getCameraData() const {
    return m_cameraData;
}

SceneNode *ScenefileReader::getRootNode() const {
    return m_root;
}

// This is where it all goes down...
bool ScenefileReader::readJSON() {
    // Read the file
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cout << "could not open " << file_name << std::endl;
        return false;
    }
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();


    json scenefile;
    try {
        scenefile = json::parse(fileContents);
    } catch (json::parse_error& e) {
        std::cout << "could not parse " << file_name << std::endl;
        std::cout << "parse error at byte " << e.byte << ": " << e.what() << std::endl;
        return false;
    }
    if (!scenefile.is_object()) {
        std::cout << "document is not an object" << std::endl;
        return false;
    }

    if (!scenefile.contains("globalData")) {
        std::cout << "missing required field \"globalData\" on root object" << std::endl;
        return false;
    }
    if (!scenefile.contains("cameraData")) {
        std::cout << "missing required field \"cameraData\" on root object" << std::endl;
        return false;
    }

    std::vector<std::string> requiredFields = {"globalData", "cameraData"};
    std::vector<std::string> optionalFields = {"name", "groups", "templateGroups"};
    // If other fields are present, raise an error
    std::vector<std::string> allFields;
    std::copy(requiredFields.begin(), requiredFields.end(), std::back_inserter(allFields));
    std::copy(optionalFields.begin(), optionalFields.end(), std::back_inserter(allFields));
    for (auto& [key, _] : scenefile.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on root object" << std::endl;
            return false;
        }
    }

    // Parse the global data
    if (!parseGlobalData(scenefile["globalData"])) {
        std::cout << "could not parse \"globalData\"" << std::endl;
        return false;
    }

    // Parse the camera data
    if (!parseCameraData(scenefile["cameraData"])) {
        std::cout << "could not parse \"cameraData\"" << std::endl;
        return false;
    }

    // Parse the template groups
    if (scenefile.contains("templateGroups")) {
        if (!parseTemplateGroups(scenefile["templateGroups"])) {
            return false;
        }
    }

    // Parse the groups
    if (scenefile.contains("groups")) {
        if (!parseGroups(scenefile["groups"], m_root)) {
            return false;
        }
    }

    std::cout << "Finished reading " << file_name << std::endl;
    return true;
}

/**
 * Parse a globalData field and fill in m_globalData.
 */
bool ScenefileReader::parseGlobalData(const json &globalData) {
    std::vector<std::string> requiredFields = {"ambientCoeff", "diffuseCoeff", "specularCoeff"};
    std::vector<std::string> optionalFields = {"transparentCoeff"};
    std::vector<std::string> allFields;
    std::copy(requiredFields.begin(), requiredFields.end(), std::back_inserter(allFields));
    std::copy(optionalFields.begin(), optionalFields.end(), std::back_inserter(allFields));
    for (auto &[key, _] : globalData.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on globalData object" << std::endl;
            return false;
        }
    }

    for (const auto& field : requiredFields) {
        if (!globalData.contains(field)) {
            std::cout << "missing required field \"" << field << "\" on globalData object" << std::endl;
            return false;
        }
    }

    // Parse the global data
    if (globalData["ambientCoeff"].is_number()) {
        m_globalData.ka = globalData["ambientCoeff"].get<float>();
    }
    else {
        std::cout << "globalData ambientCoeff must be a floating-point value" << std::endl;
        return false;
    }
    if (globalData["diffuseCoeff"].is_number()) {
        m_globalData.kd = globalData["diffuseCoeff"].get<float>();
    }
    else {
        std::cout << "globalData diffuseCoeff must be a floating-point value" << std::endl;
        return false;
    }
    if (globalData["specularCoeff"].is_number()) {
        m_globalData.ks = globalData["specularCoeff"].get<float>();
    }
    else {
        std::cout << "globalData specularCoeff must be a floating-point value" << std::endl;
        return false;
    }
    if (globalData.contains("transparentCoeff")) {
        if (globalData["transparentCoeff"].is_number()) {
            m_globalData.kt = globalData["transparentCoeff"].get<float>();
        }
        else {
            std::cout << "globalData transparentCoeff must be a floating-point value" << std::endl;
            return false;
        }
    }

    return true;
}

/**
 * Parse a Light and add a new CS123SceneLightData to m_lights.
 */
bool ScenefileReader::parseLightData(const json &lightData, SceneNode *node) {
    std::vector<std::string> requiredFields = {"type", "color"};
    std::vector<std::string> optionalFields = {"name", "attenuationCoeff", "direction", "penumbra", "angle"};
    std::vector<std::string> allFields;
    std::copy(requiredFields.begin(), requiredFields.end(), std::back_inserter(allFields));
    std::copy(optionalFields.begin(), optionalFields.end(), std::back_inserter(allFields));
    for (auto &[key, _] : lightData.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on light object" << std::endl;
            return false;
        }
    }

    for (auto &field : requiredFields) {
        if (!lightData.contains(field)) {
            std::cout << "missing required field \"" << field << "\" on light object" << std::endl;
            return false;
        }
    }

    // Create a default light
    SceneLight *light = new SceneLight();
    memset(light, 0, sizeof(SceneLight));
    node->lights.push_back(light);

    light->dir = glm::vec4(0.f, 0.f, 0.f, 0.f);
    light->function = glm::vec3(1, 0, 0);

    // parse the color
    if (!lightData["color"].is_array()) {
        std::cout << "light color must be of type array" << std::endl;
        return false;
    }
    json colorArray = lightData["color"];
    if (colorArray.size() != 3) {
        std::cout << "light color must be of size 3" << std::endl;
        return false;
    }
    if (!colorArray[0].is_number() || !colorArray[1].is_number() || !colorArray[2].is_number()) {
        std::cout << "light color must contain floating-point values" << std::endl;
        return false;
    }
    light->color.r = colorArray[0].get<float>();
    light->color.g = colorArray[1].get<float>();
    light->color.b = colorArray[2].get<float>();

    // parse the type
    if (!lightData["type"].is_string()) {
        std::cout << "light type must be of type string" << std::endl;
        return false;
    }
    std::string lightType = lightData["type"].get<std::string>();

    // parse directional light
    if (lightType == "directional") {
        light->type = LightType::LIGHT_DIRECTIONAL;

        // parse direction
        if (!lightData.contains("direction")) {
            std::cout << "directional light must contain field \"direction\"" << std::endl;
            return false;
        }
        if (!lightData["direction"].is_array()) {
            std::cout << "directional light direction must be of type array" << std::endl;
            return false;
        }
        json directionArray = lightData["direction"];
        if (directionArray.size() != 3) {
            std::cout << "directional light direction must be of size 3" << std::endl;
            return false;
        }
        if (!directionArray[0].is_number() || !directionArray[1].is_number() || !directionArray[2].is_number()) {
            std::cout << "directional light direction must contain floating-point values" << std::endl;
            return false;
        }
        light->dir.x = directionArray[0].get<float>();
        light->dir.y = directionArray[1].get<float>();
        light->dir.z = directionArray[2].get<float>();
    }
    else if (lightType == "point") {
        light->type = LightType::LIGHT_POINT;

        // parse the attenuation coefficient
        if (!lightData.contains("attenuationCoeff")) {
            std::cout << "point light must contain field \"attenuationCoeff\"" << std::endl;
            return false;
        }
        if (!lightData["attenuationCoeff"].is_array()) {
            std::cout << "point light attenuationCoeff must be of type array" << std::endl;
            return false;
        }
        json attenuationArray = lightData["attenuationCoeff"];
        if (attenuationArray.size() != 3) {
            std::cout << "point light attenuationCoeff must be of size 3" << std::endl;
            return false;
        }
        if (!attenuationArray[0].is_number() || !attenuationArray[1].is_number() || !attenuationArray[2].is_number()) {
            std::cout << "ppoint light attenuationCoeff must contain floating-point values" << std::endl;
            return false;
        }
        light->function.x = attenuationArray[0].get<float>();
        light->function.y = attenuationArray[1].get<float>();
        light->function.z = attenuationArray[2].get<float>();
    }
    else if (lightType == "spot") {
        std::vector<std::string> pointRequiredFields = {"direction", "penumbra", "angle", "attenuationCoeff"};
        for (auto &field : pointRequiredFields) {
            if (!lightData.contains(field)) {
                std::cout << "missing required field \"" << field << "\" on spotlight object" << std::endl;
                return false;
            }
        }
        light->type = LightType::LIGHT_SPOT;

        // parse direction
        if (!lightData["direction"].is_array()) {
            std::cout << "spotlight direction must be of type array" << std::endl;
            return false;
        }
        json directionArray = lightData["direction"];
        if (directionArray.size() != 3) {
            std::cout << "spotlight direction must be of size 3" << std::endl;
            return false;
        }
        if (!directionArray[0].is_number() || !directionArray[1].is_number() || !directionArray[2].is_number()) {
            std::cout << "spotlight direction must contain floating-point values" << std::endl;
            return false;
        }
        light->dir.x = directionArray[0].get<float>();
        light->dir.y = directionArray[1].get<float>();
        light->dir.z = directionArray[2].get<float>();

        // parse attenuation coefficient
        if (!lightData["attenuationCoeff"].is_array()) {
            std::cout << "spotlight attenuationCoeff must be of type array" << std::endl;
            return false;
        }
        json attenuationArray = lightData["attenuationCoeff"];
        if (attenuationArray.size() != 3) {
            std::cout << "spotlight attenuationCoeff must be of size 3" << std::endl;
            return false;
        }
        if (!attenuationArray[0].is_number() || !attenuationArray[1].is_number() || !attenuationArray[2].is_number()) {
            std::cout << "spotlight direction must contain floating-point values" << std::endl;
            return false;
        }
        light->function.x = attenuationArray[0].get<float>();
        light->function.y = attenuationArray[1].get<float>();
        light->function.z = attenuationArray[2].get<float>();

        // parse penumbra
        if (!lightData["penumbra"].is_number()) {
            std::cout << "spotlight penumbra must be of type float" << std::endl;
            return false;
        }
        light->penumbra = lightData["penumbra"].get<float>() * M_PI / 180.f;

        // parse angle
        if (!lightData["angle"].is_number()) {
            std::cout << "spotlight angle must be of type float" << std::endl;
            return false;
        }
        light->angle = lightData["angle"].get<float>() * M_PI / 180.f;
    }
    else {
        std::cout << "unknown light type \"" << lightType << "\"" << std::endl;
        return false;
    }

    return true;
}

/**
 * Parse cameraData and fill in m_cameraData.
 */
bool ScenefileReader::parseCameraData(const json &cameradata) {
    std::vector<std::string>  requiredFields = {"position", "up", "heightAngle"};
    std::vector<std::string>  optionalFields = {"aperture", "focalLength", "look", "focus"};
    std::vector<std::string>  allFields;
    std::copy(requiredFields.begin(), requiredFields.end(), std::back_inserter(allFields));
    std::copy(optionalFields.begin(), optionalFields.end(), std::back_inserter(allFields));
    for (auto &[key, _] : cameradata.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on cameraData object" << std::endl;
            return false;
        }
    }

    for (auto &field : requiredFields) {
        if (!cameradata.contains(field)) {
            std::cout << "missing required field \"" << field << "\" on cameraData object" << std::endl;
            return false;
        }
    }

    // Must have either look or focus, but not both
    if (cameradata.contains("look") && cameradata.contains("focus")) {
        std::cout << "cameraData cannot contain both \"look\" and \"focus\"" << std::endl;
        return false;
    }

    // Parse the camera data
    if (cameradata["position"].is_array()) {
        json position = cameradata["position"];
        if (position.size() != 3) {
            std::cout << "cameraData position must have 3 elements" << std::endl;
            return false;
        }
        if (!position[0].is_number() || !position[1].is_number() || !position[2].is_number()) {
            std::cout << "cameraData position must be a floating-point value" << std::endl;
            return false;
        }
        m_cameraData.pos.x = position[0].get<float>();
        m_cameraData.pos.y = position[1].get<float>();
        m_cameraData.pos.z = position[2].get<float>();
        m_cameraData.pos.w = 1.f;
    }
    else {
        std::cout << "cameraData position must be an array" << std::endl;
        return false;
    }

    if (cameradata["up"].is_array()) {
        json up = cameradata["up"];
        if (up.size() != 3) {
            std::cout << "cameraData up must have 3 elements" << std::endl;
            return false;
        }
        if (!up[0].is_number() || !up[1].is_number() || !up[2].is_number()) {
            std::cout << "cameraData up must be a floating-point value" << std::endl;
            return false;
        }
        m_cameraData.up.x = up[0].get<float>();
        m_cameraData.up.y = up[1].get<float>();
        m_cameraData.up.z = up[2].get<float>();
        m_cameraData.up.w = 0.f;
    }
    else {
        std::cout << "cameraData up must be an array" << std::endl;
        return false;
    }

    if (cameradata["heightAngle"].is_number()) {
        m_cameraData.heightAngle = cameradata["heightAngle"].get<float>() * M_PI / 180.f;
    }
    else {
        std::cout << "cameraData heightAngle must be a floating-point value" << std::endl;
        return false;
    }

    if (cameradata.contains("aperture")) {
        if (cameradata["aperture"].is_number()) {
            m_cameraData.aperture = cameradata["aperture"].get<float>();
        }
        else {
            std::cout << "cameraData aperture must be a floating-point value" << std::endl;
            return false;
        }
    }

    if (cameradata.contains("focalLength")) {
        if (cameradata["focalLength"].is_number()) {
            m_cameraData.focalLength = cameradata["focalLength"].get<float>();
        }
        else {
            std::cout << "cameraData focalLength must be a floating-point value" << std::endl;
            return false;
        }
    }

    // Parse the look or focus
    // if the focus is specified, we will convert it to a look vector later
    if (cameradata.contains("look")) {
        if (cameradata["look"].is_array()) {
            json look = cameradata["look"];
            if (look.size() != 3) {
                std::cout << "cameraData look must have 3 elements" << std::endl;
                return false;
            }
            if (!look[0].is_number() || !look[1].is_number() || !look[2].is_number()) {
                std::cout << "cameraData look must be a floating-point value" << std::endl;
                return false;
            }
            m_cameraData.look.x = look[0].get<float>();
            m_cameraData.look.y = look[1].get<float>();
            m_cameraData.look.z = look[2].get<float>();
            m_cameraData.look.w = 0.f;
        }
        else {
            std::cout << "cameraData look must be an array" << std::endl;
            return false;
        }
    }
    else if (cameradata.contains("focus")) {
        if (cameradata["focus"].is_array()) {
            json focus = cameradata["focus"];
            if (focus.size() != 3) {
                std::cout << "cameraData focus must have 3 elements" << std::endl;
                return false;
            }
            if (!focus[0].is_number() || !focus[1].is_number() || !focus[2].is_number()) {
                std::cout << "cameraData focus must be a floating-point value" << std::endl;
                return false;
            }
            m_cameraData.look.x = focus[0].get<float>();
            m_cameraData.look.y = focus[1].get<float>();
            m_cameraData.look.z = focus[2].get<float>();
            m_cameraData.look.w = 1.f;
        }
        else {
            std::cout << "cameraData focus must be an array" << std::endl;
            return false;
        }
    }

    // Convert the focus point (stored in the look vector) into a
    // look vector from the camera position to that focus point.
    if (cameradata.contains("focus")) {
        m_cameraData.look -= m_cameraData.pos;
    }

    return true;
}

bool ScenefileReader::parseTemplateGroups(const json &templateGroups) {
    if (!templateGroups.is_array()) {
        std::cout << "templateGroups must be an array" << std::endl;
        return false;
    }

    json templateGroupsArray = templateGroups;
    for (auto templateGroup : templateGroupsArray) {
        if (!templateGroup.is_object()) {
            std::cout << "templateGroup items must be of type object" << std::endl;
            return false;
        }

        if (!parseTemplateGroupData(templateGroup)) {
            return false;
        }
    }

    return true;
}

bool ScenefileReader::parseTemplateGroupData(const json &templateGroup) {
    std::vector<std::string> requiredFields = {"name"};
    std::vector<std::string> optionalFields = {"translate", "rotate", "scale", "matrix", "lights", "primitives", "groups"};
    std::vector<std::string> allFields;
    std::copy(requiredFields.begin(), requiredFields.end(), std::back_inserter(allFields));

    for (auto &[key, _] : templateGroup.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on templateGroup object" << std::endl;
            return false;
        }
    }

    for (auto &field : requiredFields) {
        if (!templateGroup.contains(field)) {
            std::cout << "missing required field \"" << field << "\" on templateGroup object" << std::endl;
            return false;
        }
    }

    if (!templateGroup["name"].is_string()) {
        std::cout << "templateGroup name must be a string" << std::endl;
    }
    if (m_templates.contains(templateGroup["name"].get<std::string>())) {
        std::cout << "templateGroups cannot have the same" << std::endl;
    }

    SceneNode *templateNode = new SceneNode;
    m_nodes.push_back(templateNode);
    m_templates[templateGroup["name"].get<std::string>()] = templateNode;

    return parseGroupData(templateGroup, templateNode);
}

/**
 * Parse a group object and create a new CS123SceneNode in m_nodes.
 * NAME OF NODE CANNOT REFERENCE TEMPLATE NODE
 */
bool ScenefileReader::parseGroupData(const json &object, SceneNode *node) {
    std::vector<std::string> optionalFields = {"name", "translate", "rotate", "scale", "matrix", "lights", "primitives", "groups"};
    std::vector<std::string> allFields = optionalFields;

    for (auto &[key, _] : object.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on group object" << std::endl;
            return false;
        }
    }

    // parse translation if defined
    if (object.contains("translate")) {
        if (!object["translate"].is_array()) {
            std::cout << "group translate must be of type array" << std::endl;
            return false;
        }

        json translateArray = object["translate"];
        if (translateArray.size() != 3) {
            std::cout << "group translate must have 3 elements" << std::endl;
            return false;
        }
        if (!translateArray[0].is_number() || !translateArray[1].is_number() || !translateArray[2].is_number()) {
            std::cout << "group translate must contain floating-point values" << std::endl;
            return false;
        }

        SceneTransformation *translation = new SceneTransformation();
        translation->type = TransformationType::TRANSFORMATION_TRANSLATE;
        translation->translate.x = translateArray[0].get<float>();
        translation->translate.y = translateArray[1].get<float>();
        translation->translate.z = translateArray[2].get<float>();

        node->transformations.push_back(translation);
    }

    // parse rotation if defined
    if (object.contains("rotate")) {
        if (!object["rotate"].is_array()) {
            std::cout << "group rotate must be of type array" << std::endl;
            return false;
        }

        json rotateArray = object["rotate"];
        if (rotateArray.size() != 4) {
            std::cout << "group rotate must have 4 elements" << std::endl;
            return false;
        }
        if (!rotateArray[0].is_number() || !rotateArray[1].is_number() || !rotateArray[2].is_number() || !rotateArray[3].is_number()) {
            std::cout << "group rotate must contain floating-point values" << std::endl;
            return false;
        }

        SceneTransformation *rotation = new SceneTransformation();
        rotation->type = TransformationType::TRANSFORMATION_ROTATE;
        rotation->rotate.x = rotateArray[0].get<float>();
        rotation->rotate.y = rotateArray[1].get<float>();
        rotation->rotate.z = rotateArray[2].get<float>();
        rotation->angle = rotateArray[3].get<float>() * M_PI / 180.f;

        node->transformations.push_back(rotation);
    }

    // parse scale if defined
    if (object.contains("scale")) {
        if (!object["scale"].is_array()) {
            std::cout << "group scale must be of type array" << std::endl;
            return false;
        }

        json scaleArray = object["scale"];
        if (scaleArray.size() != 3) {
            std::cout << "group scale must have 3 elements" << std::endl;
            return false;
        }
        if (!scaleArray[0].is_number() || !scaleArray[1].is_number() || !scaleArray[2].is_number()) {
            std::cout << "group scale must contain floating-point values" << std::endl;
            return false;
        }

        SceneTransformation *scale = new SceneTransformation();
        scale->type = TransformationType::TRANSFORMATION_SCALE;
        scale->scale.x = scaleArray[0].get<float>();
        scale->scale.y = scaleArray[1].get<float>();
        scale->scale.z = scaleArray[2].get<float>();

        node->transformations.push_back(scale);
    }

    // parse matrix if defined
    if (object.contains("matrix")) {
        if (!object["matrix"].is_array()) {
            std::cout << "group matrix must be of type array of array" << std::endl;
            return false;
        }

        json matrixArray = object["matrix"];
        if (matrixArray.size() != 4) {
            std::cout << "group matrix must be 4x4" << std::endl;
            return false;
        }

        SceneTransformation *matrixTransformation = new SceneTransformation();
        matrixTransformation->type = TransformationType::TRANSFORMATION_MATRIX;

        float *matrixPtr = glm::value_ptr(matrixTransformation->matrix);
        int rowIndex = 0;
        for (auto row : matrixArray) {
            if (!row.is_array()) {
                std::cout << "group matrix must be of type array of array" << std::endl;
                return false;
            }

            json rowArray = row;
            if (rowArray.size() != 4) {
                std::cout << "group matrix must be 4x4" << std::endl;
                return false;
            }

            int colIndex = 0;
            for (auto val : rowArray) {
                if (!val.is_number()) {
                    std::cout << "group matrix must contain all floating-point values" << std::endl;
                    return false;
                }

                // fill in column-wise
                matrixPtr[colIndex * 4 + rowIndex] = (float)val.get<float>();
                colIndex++;
            }
            rowIndex++;
        }

        node->transformations.push_back(matrixTransformation);
    }

    // parse lights if any
    if (object.contains("lights")) {
        if (!object["lights"].is_array()) {
            std::cout << "group lights must be of type array" << std::endl;
            return false;
        }
        json lightsArray = object["lights"];
        for (auto light : lightsArray) {
            if (!light.is_object()) {
                std::cout << "light must be of type object" << std::endl;
                return false;
            }

            if (!parseLightData(light, node)) {
                return false;
            }
        }
    }

    // parse primitives if any
    if (object.contains("primitives")) {
        if (!object["primitives"].is_array()) {
            std::cout << "group primitives must be of type array" << std::endl;
            return false;
        }
        json primitivesArray = object["primitives"];
        for (auto primitive : primitivesArray) {
            if (!primitive.is_object()) {
                std::cout << "primitive must be of type object" << std::endl;
                return false;
            }

            if (!parsePrimitive(primitive, node)) {
                return false;
            }
        }
    }

    // parse children groups if any
    if (object.contains("groups")) {
        if (!parseGroups(object["groups"], node)) {
            return false;
        }
    }

    return true;
}

bool ScenefileReader::parseGroups(const json &groups, SceneNode *parent) {
    if (!groups.is_array()) {
        std::cout << "groups must be of type array" << std::endl;
        return false;
    }

    json groupsArray = groups;
    for (auto group : groupsArray) {
        if (!group.is_object()) {
            std::cout << "group items must be of type object" << std::endl;
            return false;
        }

        json groupData = group;
        if (groupData.contains("name")) {
            if (!groupData["name"].is_string()) {
                std::cout << "group name must be of type string" << std::endl;
                return false;
            }

            // if its a reference to a template group append it
            std::string groupName = groupData["name"].get<std::string>();
            if (m_templates.contains(groupName)) {
                parent->children.push_back(m_templates[groupName]);
                continue;
            }
        }

        SceneNode *node = new SceneNode;
        m_nodes.push_back(node);
        parent->children.push_back(node);

        if (!parseGroupData(group, node)) {
            return false;
        }
    }

    return true;
}

/**
 * Parse an <object type="primitive"> tag into node.
 */
bool ScenefileReader::parsePrimitive(const json &prim, SceneNode *node) {
    std::vector<std::string> requiredFields = {"type"};
    std::vector<std::string> optionalFields = {
            "meshFile", "ambient", "diffuse", "specular", "reflective", "transparent", "shininess", "ior",
            "blend", "textureFile", "textureU", "textureV", "bumpMapFile", "bumpMapU", "bumpMapV"};

    std::vector<std::string> allFields;
    std::copy(requiredFields.begin(), requiredFields.end(), std::back_inserter(allFields));
    std::copy(optionalFields.begin(), optionalFields.end(), std::back_inserter(allFields));

    for (auto &[key, _] : prim.items()) {
        if (std::find(allFields.begin(), allFields.end(), key) == allFields.end()) {
            std::cout << "unknown field \"" << key << "\" on primitive object" << std::endl;
            return false;
        }
    }

    for (auto field : requiredFields) {
        if (!prim.contains(field)) {
            std::cout << "missing required field \"" << field << "\" on primitive object" << std::endl;
            return false;
        }
    }

    if (!prim["type"].is_string()) {
        std::cout << "primitive type must be of type string" << std::endl;
        return false;
    }
    std::string primType = prim["type"].get<std::string>();

    // Default primitive
    ScenePrimitive *primitive = new ScenePrimitive();
    SceneMaterial &mat = primitive->material;
    mat.clear();
    primitive->type = PrimitiveType::PRIMITIVE_CUBE;
    mat.textureMap.isUsed = false;
    mat.bumpMap.isUsed = false;
    mat.cDiffuse.r = mat.cDiffuse.g = mat.cDiffuse.b = 1;
    node->primitives.push_back(primitive);

    std::filesystem::path basepath = std::filesystem::path(file_name).parent_path().parent_path();
    if (primType == "sphere")
        primitive->type = PrimitiveType::PRIMITIVE_SPHERE;
    else if (primType == "cube")
        primitive->type = PrimitiveType::PRIMITIVE_CUBE;
    else if (primType == "cylinder")
        primitive->type = PrimitiveType::PRIMITIVE_CYLINDER;
    else if (primType == "cone")
        primitive->type = PrimitiveType::PRIMITIVE_CONE;
    else if (primType == "mesh") {
        primitive->type = PrimitiveType::PRIMITIVE_MESH;
        if (!prim.contains("meshFile")) {
            std::cout << "primitive type mesh must contain field meshFile" << std::endl;
            return false;
        }
        if (!prim["meshFile"].is_string()) {
            std::cout << "primitive meshFile must be of type string" << std::endl;
            return false;
        }

        std::filesystem::path relativePath(prim["meshFile"].get<std::string>());
        primitive->meshfile = (basepath / relativePath).string();
    }
    else {
        std::cout << "unknown primitive type \"" << primType << "\"" << std::endl;
        return false;
    }

    if (prim.contains("ambient")) {
        if (!prim["ambient"].is_array()) {
            std::cout << "primitive ambient must be of type array" << std::endl;
            return false;
        }
        json ambientArray = prim["ambient"];
        if (ambientArray.size() != 3) {
            std::cout << "primitive ambient array must be of size 3" << std::endl;
            return false;
        }

        for (int i = 0; i < 3; i++) {
            if (!ambientArray[i].is_number()) {
                std::cout << "primitive ambient must contain floating-point values" << std::endl;
                return false;
            }

            mat.cAmbient[i] = ambientArray[i].get<float>();
        }
    }

    if (prim.contains("diffuse")) {
        if (!prim["diffuse"].is_array()) {
            std::cout << "primitive diffuse must be of type array" << std::endl;
            return false;
        }
        json diffuseArray = prim["diffuse"];
        if (diffuseArray.size() != 3) {
            std::cout << "primitive diffuse array must be of size 3" << std::endl;
            return false;
        }

        for (int i = 0; i < 3; i++) {
            if (!diffuseArray[i].is_number()) {
                std::cout << "primitive diffuse must contain floating-point values" << std::endl;
                return false;
            }

            mat.cDiffuse[i] = diffuseArray[i].get<float>();
        }
    }

    if (prim.contains("specular")) {
        if (!prim["specular"].is_array()) {
            std::cout << "primitive specular must be of type array" << std::endl;
            return false;
        }
        json specularArray = prim["specular"];
        if (specularArray.size() != 3) {
            std::cout << "primitive specular array must be of size 3" << std::endl;
            return false;
        }

        for (int i = 0; i < 3; i++) {
            if (!specularArray[i].is_number()) {
                std::cout << "primitive specular must contain floating-point values" << std::endl;
                return false;
            }

            mat.cSpecular[i] = specularArray[i].get<float>();
        }
    }

    if (prim.contains("reflective")) {
        if (!prim["reflective"].is_array()) {
            std::cout << "primitive reflective must be of type array" << std::endl;
            return false;
        }
        json reflectiveArray = prim["reflective"];
        if (reflectiveArray.size() != 3) {
            std::cout << "primitive reflective array must be of size 3" << std::endl;
            return false;
        }

        for (int i = 0; i < 3; i++) {
            if (!reflectiveArray[i].is_number()) {
                std::cout << "primitive reflective must contain floating-point values" << std::endl;
                return false;
            }

            mat.cReflective[i] = reflectiveArray[i].get<float>();
        }
    }

    if (prim.contains("transparent")) {
        if (!prim["transparent"].is_array()) {
            std::cout << "primitive transparent must be of type array" << std::endl;
            return false;
        }
        json transparentArray = prim["transparent"];
        if (transparentArray.size() != 3) {
            std::cout << "primitive transparent array must be of size 3" << std::endl;
            return false;
        }

        for (int i = 0; i < 3; i++) {
            if (!transparentArray[i].is_number()) {
                std::cout << "primitive transparent must contain floating-point values" << std::endl;
                return false;
            }

            mat.cTransparent[i] = transparentArray[i].get<float>();
        }
    }

    if (prim.contains("shininess")) {
        if (!prim["shininess"].is_number()) {
            std::cout << "primitive shininess must be of type float" << std::endl;
            return false;
        }

        mat.shininess = (float) prim["shininess"].get<float>();
    }

    if (prim.contains("ior")) {
        if (!prim["ior"].is_number()) {
            std::cout << "primitive ior must be of type float" << std::endl;
            return false;
        }

        mat.ior = (float) prim["ior"].get<float>();
    }

    if (prim.contains("blend")) {
        if (!prim["blend"].is_number()) {
            std::cout << "primitive blend must be of type float" << std::endl;
            return false;
        }

        mat.blend = (float)prim["blend"].get<float>();
    }

    if (prim.contains("textureFile")) {
        if (!prim["textureFile"].is_string()) {
            std::cout << "primitive textureFile must be of type string" << std::endl;
            return false;
        }
        std::filesystem::path fileRelativePath(prim["textureFile"].get<std::string>());

        mat.textureMap.filename = (basepath / fileRelativePath).string();
        mat.textureMap.repeatU = prim.contains("textureU") && prim["textureU"].is_number() ? prim["textureU"].get<float>() : 1;
        mat.textureMap.repeatV = prim.contains("textureV") && prim["textureV"].is_number() ? prim["textureV"].get<float>() : 1;
        mat.textureMap.isUsed = true;
    }

    if (prim.contains("bumpMapFile")) {
        if (!prim["bumpMapFile"].is_string()) {
            std::cout << "primitive bumpMapFile must be of type string" << std::endl;
            return false;
        }
        std::filesystem::path fileRelativePath(prim["bumpMapFile"].get<std::string>());

        mat.bumpMap.filename = (basepath / fileRelativePath).string();
        mat.bumpMap.repeatU = prim.contains("bumpMapU") && prim["bumpMapU"].is_number() ? prim["bumpMapU"].get<float>() : 1;
        mat.bumpMap.repeatV = prim.contains("bumpMapV") && prim["bumpMapV"].is_number() ? prim["bumpMapV"].get<float>() : 1;
        mat.bumpMap.isUsed = true;
    }

    return true;
}

#version 330 core
#define MAX_LIGHTS 16

// https://stackoverflow.com/questions/34251648/enum-usage-for-bitwise-and-in-glsl
const uint LIGHT_POINT       = 0x1u;
const uint LIGHT_DIRECTIONAL = 0x2u;
const uint LIGHT_SPOT        = 0x4u;

// condensed light data struct for shader
struct LightData {
    uint type;
    vec3 color;
    vec3 pos;
    vec3 dir;
    vec3 function;
    float angle;
    float penumbra;
};

in vec3 positionWS;
in vec3 normalWS;
in vec2 uv;

out vec4 fragColor;

// ambient
uniform float ka;
uniform vec3 cAmbient;

// diffuse
uniform float kd;
uniform vec3 cDiffuse;
uniform int numLights;
uniform LightData lights[MAX_LIGHTS];
uniform bool usesTexture;
uniform float blend;
uniform float repeatU;
uniform float repeatV;
uniform sampler2D objTexture;

// specular
uniform float ks;
uniform vec3 cSpecular;
uniform float shininess;
uniform vec3 cameraPosWS;

//skybox
uniform bool isSkybox;

float getAttenuation(vec3 pos, LightData light) {
    switch (light.type) {
        case LIGHT_DIRECTIONAL:
            return 1.0;
        case LIGHT_SPOT:
        case LIGHT_POINT: {
            float distanceToLight = length(light.pos - pos);
            return min(1.0, 1.0 / (light.function.x + distanceToLight * light.function.y + distanceToLight * distanceToLight * light.function.z));
        }
    }
    // should never reach here assuming the light type is correct
    return 1.0;
}

vec3 getDirToLight(vec3 pos, LightData light) {
    switch (light.type) {
        case LIGHT_POINT:
        case LIGHT_SPOT:
            return normalize(light.pos - pos);
        case LIGHT_DIRECTIONAL:
            // return -normalize(light.dir);
            // normalization was done on the cpu
            return vec3(0.0) - light.dir;
    }
    // should never reach here assuming the light type is correct
    return vec3(0.0);
}

float getFalloffFactor(vec3 directionToLight, LightData light) {
    switch (light.type) {
        // point and directional lights do not have angular falloff
        case LIGHT_POINT:
        case LIGHT_DIRECTIONAL:
            return 1.0;
        case LIGHT_SPOT: {
            float thetaOuter = light.angle;
            float thetaInner = light.angle - light.penumbra;
            // we normalize light.dir on the cpu
            float thetaInput = acos(dot(vec3(0.0) - directionToLight, light.dir));
            if (thetaInput <= thetaInner) {
                return 1.0;
            } else if (thetaInput <= thetaOuter) {
                float inputToEaser = (thetaInput - thetaInner) / (thetaOuter - thetaInner);
                float falloff = -2.0 * inputToEaser * inputToEaser * inputToEaser + 3.0 * inputToEaser * inputToEaser;
                return 1.0 - falloff;
            } else {
                return 0.0;
            }
        }
    }
    // should never reach here assuming the light type is correct
    return 1.0;
}

vec3 getTextureColor() {
    if (!usesTexture) {
        // can't actually error in GLSL
        // throw std::runtime_error("No texture map available");
        return vec3(1.0);
    }
    float u = uv.x;
    float v = uv.y;
    if (repeatU <= 0 || repeatV <= 0) {
        // throw std::runtime_error("Invalid numRepeats");
        return vec3(1.0);
    }
    // i thinkkkk since texture sampling auto-wraps, we can just do this
    u = u * repeatU;
    v = (1- v) * repeatV;
    return texture(objTexture, vec2(u, v)).rgb;
}

void main() {
    if (isSkybox) {
        fragColor = vec4(getTextureColor(), 0);
        return;
    }
    vec3 normalWSNormalized = normalize(normalWS);

    // ambient
    fragColor = vec4(ka * cAmbient, 1.0);

    // texture stuff
    vec3 diffuseFactor = kd * cDiffuse;
    if (usesTexture) {
        diffuseFactor = (1.0 - blend) * diffuseFactor + blend * getTextureColor();
    }

    for (int i = 0; i < numLights; i++) {
        // diffuse
        vec3 dirToLight = getDirToLight(positionWS, lights[i]);
        float fAtt = getAttenuation(positionWS, lights[i]);
        float falloffFactor = getFalloffFactor(dirToLight, lights[i]);
        fragColor += vec4(fAtt * falloffFactor * max(dot(normalWSNormalized, dirToLight), 0.0) * diffuseFactor * lights[i].color, 0.0);

        // specular
        vec3 reflectedLight = reflect(vec3(0.0) - dirToLight, normalWSNormalized);
        vec3 dirToCamera = normalize(cameraPosWS - positionWS);
        float base = max(dot(reflectedLight, dirToCamera), 0.0);
        vec3 specularComponent;
        if (base == 0.0) {
            continue;
        } else {
            specularComponent = fAtt * falloffFactor * ks * lights[i].color * cSpecular * pow(base, shininess);
        }
        fragColor += vec4(specularComponent, 0.0);
    }

    // debug by showing normals
    // fragColor = vec4((normalWSNormalized + vec3(1.0)) * 0.5, 1.0);
    // debug with only white
    // fragColor = vec4(1.0);
}

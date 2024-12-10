#version 330 core

layout (location = 0) in vec3 positionOS;
layout (location = 1) in vec3 normalOS;
layout (location = 2) in vec2 uvLayout;

// outs are passed to the fragment shader
out vec3 positionWS;
out vec3 normalWS;
out vec2 uv;

uniform mat3 inverseTransposeModel;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    positionWS = (model * vec4(positionOS, 1.0)).xyz;
    normalWS = normalize(inverseTransposeModel * normalOS);
    uv = uvLayout;

    gl_Position = proj * view * vec4(positionWS, 1.0);
}

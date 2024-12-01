#version 330 core

layout (location = 0) in vec3 positionOS;
layout (location = 1) in vec3 normalOS;

// outs are passed to the fragment shader
out vec3 positionWS;
out vec3 normalWS;

uniform mat3 inverseTransposeModel;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    positionWS = (model * vec4(positionOS, 1.0)).xyz;
    normalWS = normalize(inverseTransposeModel * normalOS);

    gl_Position = proj * view * vec4(positionWS, 1.0);
}

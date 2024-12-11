#include <iostream>
#include "helpers.h"

GLint helpers::getUniformLocation(GLuint shader_program, const char* name, bool checkValidLoc) {
    GLint loc = glGetUniformLocation(shader_program, name);
    if (checkValidLoc && loc == -1) {
        std::cerr << "Could not find uniform " << name << " in shader program" << std::endl;
    }
    return loc;
}

void helpers::passUniformMat4(GLuint shader_program, const char* name, const glm::mat4& mat) {
    glUniformMatrix4fv(getUniformLocation(shader_program, name), 1, GL_FALSE, &mat[0][0]);
}

void helpers::passUniformMat3(GLuint shader_program, const char* name, const glm::mat3& mat) {
    glUniformMatrix3fv(getUniformLocation(shader_program, name), 1, GL_FALSE, &mat[0][0]);
}

void helpers::passUniformFloat(GLuint shader_program, const char* name, float value) {
    glUniform1f(getUniformLocation(shader_program, name), value);
}

void helpers::passUniformInt(GLuint shader_program, const char* name, int value) {
    glUniform1i(getUniformLocation(shader_program, name), value);
}

void helpers::passUniformVec3(GLuint shader_program, const char* name, const glm::vec3& vec) {
    glUniform3fv(getUniformLocation(shader_program, name), 1, &vec[0]);
}

void helpers::passUniformVec3Array(GLuint shader_program, const char* name, const std::vector<glm::vec3>& vecs) {
    glUniform3fv(getUniformLocation(shader_program, name), (GLint) vecs.size(), &vecs[0][0]);
}

glm::vec3 helpers::projectAontoB(const glm::vec3& a, const glm::vec3& b) {
    float dotBB = glm::dot(b, b);
    if (dotBB < EPSILON) {
        return glm::vec3(0.0f);
    }
    float dotAB = glm::dot(a, b);
    return (dotAB / dotBB) * b;
}
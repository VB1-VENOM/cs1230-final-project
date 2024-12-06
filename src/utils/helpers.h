#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace helpers {
    GLint getUniformLocation(GLuint shader_program, const char* name, bool checkValidLoc = true);
    void passUniformMat4(GLuint shader_program, const char* name, const glm::mat4& mat);
    void passUniformMat3(GLuint shader_program, const char* name, const glm::mat3& mat);
    void passUniformFloat(GLuint shader_program, const char* name, float value);
    void passUniformInt(GLuint shader_program, const char* name, int value);
    void passUniformVec3(GLuint shader_program, const char* name, const glm::vec3& vec);
    void passUniformVec3Array(GLuint shader_program, const char* name, const std::vector<glm::vec3>& vecs);
}
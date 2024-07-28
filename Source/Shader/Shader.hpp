#ifndef PA_SHADER_HPP
#define PA_SHADER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "file.h"

enum class EShaderType : uint8_t {
    VERTEX,
    FRAGMENT
};

class CShader {
private:
    uint32_t Id;
public:
    uint32_t Compile(const char *Source, EShaderType Type);
    void Create();
    uint8_t Load(const char *Filepath, EShaderType Type);
    void Activate();
    void Delete();

    uint32_t GetId() const { return Id; }
};

#endif
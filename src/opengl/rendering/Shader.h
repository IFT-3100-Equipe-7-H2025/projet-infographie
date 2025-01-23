#pragma once

#include "opengl/rendering/Renderer.h"

#include <string>

typedef unsigned int ProgramID;
typedef unsigned int ShaderID;

struct ShaderSource
{
    std::string vertexShader;
    std::string fragmentShader;
};

class Shader
{
public:
    explicit Shader(const std::string& filename);
    ~Shader();

    void Bind() const;

private:
    static ShaderID CompileShader(unsigned int type, const std::string& source);
    static ProgramID CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    static ShaderSource ParseShaders(const std::string& filename);

    RendererId id;
};

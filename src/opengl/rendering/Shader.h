#pragma once

#include "opengl/rendering/Renderer.h"

#include <string>
#include <unordered_map>

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

    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
    static ShaderID CompileShader(unsigned int type, const std::string& source);
    static ProgramID CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    static ShaderSource ParseShaders(const std::string& filename);

    unsigned int GetUniformLocation(const std::string& name);

    RendererId id;
    std::string filename;
    std::unordered_map<std::string, int> uniformLocationCache;
};

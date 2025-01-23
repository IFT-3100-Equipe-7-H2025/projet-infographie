#pragma once

#include "glm/glm.hpp"
#include "opengl/rendering/Renderer.h"

#include <string>
#include <unordered_map>

typedef unsigned int ProgramID;
typedef unsigned int ShaderID;
typedef unsigned int UniformLocation;

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
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniformMatrix4f(const std::string& name, glm::mat4 matrix);

private:
    static ShaderID CompileShader(unsigned int type, const std::string& source);
    static ProgramID CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    static ShaderSource ParseShaders(const std::string& filename);

    UniformLocation GetUniformLocation(const std::string& name);

    RendererId id;
    std::string filename;
    std::unordered_map<std::string, int> uniformLocationCache;
};

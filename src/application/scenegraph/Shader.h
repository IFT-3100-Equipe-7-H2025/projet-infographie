#pragma once

#include <ofShader.h>
#include <string>

class Shader : public ofShader
{
public:
    Shader() : ofShader(), name("Shader") {};

    explicit Shader(std::string name) : ofShader(), name(std::move(name)) {};

    explicit Shader(const ofShader& Shader) : ofShader(Shader), name("Shader") {};

    Shader(const ofShader& Shader, std::string name) : ofShader(Shader), name(std::move(name)) {};

    [[nodiscard]] std::string GetName() const { return this->name; }

private:
    std::string name;
};

inline auto DEFAULT_SHADER = Shader("Default");

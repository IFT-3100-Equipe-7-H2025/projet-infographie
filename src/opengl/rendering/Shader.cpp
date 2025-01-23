// Greatly inspired by TheCherno's opengl tutorial series: https://www.youtube.com/watch?v=W2gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
#include "Shader.h"
#include "opengl/Macros.h"

#include <fstream>
#include <iostream>
#include <sstream>


Shader::Shader(const std::string& filename) : id(0), filename(filename)
{
    auto [vertexShader, fragmentShader] = ParseShaders(filename);

    ProgramID program = CreateShader(vertexShader, fragmentShader);

    this->id = program;
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(this->id));
}


void Shader::Bind() const
{
    GLCall(glUseProgram(this->id));
}

ShaderID Shader::CompileShader(unsigned int type, const std::string& source)
{
    GLCall(const unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (!result)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = static_cast<char*>(alloca(length * sizeof(char)));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

ProgramID Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLCall(const unsigned int program = glCreateProgram());
    const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

ShaderSource Shader::ParseShaders(const std::string& filename)
{
    std::ifstream stream(filename);
    if (!stream.good())
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        std::cerr << "Returning empty shaders" << std::endl;
        return {};
    }

    std::string line;
    std::stringstream ss[2];
    ShaderSource shaders;

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    } type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[static_cast<int>(type)] << line << '\n';
        }
    }

    shaders.vertexShader = ss[0].str();
    shaders.fragmentShader = ss[1].str();

    return shaders;
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    unsigned int location = this->GetUniformLocation(name);
    GLCall(glUniform4f(location, v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
    if (const auto location = this->uniformLocationCache.find(name); location != this->uniformLocationCache.end())
    {
        return location->second;
    }

    GLCall(const int location = glGetUniformLocation(this->id, name.c_str()));

    if (location == -1)
    {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    }

    this->uniformLocationCache.insert({name, location});

    return location;
}

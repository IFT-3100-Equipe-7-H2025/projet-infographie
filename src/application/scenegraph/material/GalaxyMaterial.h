#pragma once

#include "Material.h"
#include "ofMain.h"
#include "ofShader.h"

class GalaxyMaterial : public ShaderMaterial
{
public:
    explicit GalaxyMaterial(std::string name) : ShaderMaterial(std::make_shared<Shader>(), std::move(name))
    {
        Shader shader;
        shader.load("shaders/galaxy.vert", "shaders/galaxy.frag");
        this->SetShader(std::make_shared<Shader>(shader));
    };

    void begin() override
    {
        ShaderMaterial::begin();
    }

    void end() override
    {
        ShaderMaterial::end();
    }

    void SetUniforms(const glm::mat4& mvp)
    {
        glActiveTexture(GL_TEXTURE0);
        material->setUniform1f("u_time", ofGetElapsedTimef());
        material->setUniformMatrix4f("modelViewProjection", mvp);
    }
};

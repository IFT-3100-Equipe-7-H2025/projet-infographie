#pragma once

#include "Material.h"
#include "ShaderMaterial.h"
#include "ofMain.h"
#include "ofShader.h"

class CubemapMaterial : public ShaderMaterial
{
public:
    explicit CubemapMaterial(GLuint cubemapid, std::string name) : ShaderMaterial(std::make_shared<Shader>(), std::move(name))
    {
        cubemapTexID = cubemapid;
        Shader shader;
        shader.load("shaders/cubemap_vs.glsl", "shaders/cubemap_fs.glsl");
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
        if (cubemapTexID == 0)
            ofLogError() << "Cubemap texture is not initialized!";

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexID);
        material->setUniform1i("skybox", 0);

        material->setUniformMatrix4f("modelViewProjection", mvp);
    }

private:
    GLuint cubemapTexID;
};

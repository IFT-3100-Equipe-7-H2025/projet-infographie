#pragma once

#include "Material.h"
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
       
/*
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content
        glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));// remove translation from the view matrix
        material->setUniformMatrix4f("view", view);
        material->setUniformMatrix4f("projection", projectionMatrix);
        // skybox cube

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexID);
        material->setUniform1i("skybox", 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);// set depth function back to default
 */
    }
    void SetCubemap(GLuint texID) { cubemapTexID = texID; }
    void SetCubemapType(const std::map<std::string, GLuint>& allCubemaps, std::string type)
    {
        auto it = allCubemaps.find(type);
        if (it != allCubemaps.end())
            cubemapTexID = it->second;
        else
            ofLogWarning() << "Cubemap type not found!";
    }

private:
    GLuint cubemapTexID;
};

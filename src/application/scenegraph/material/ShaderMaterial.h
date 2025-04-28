#include "Shader.h"
#include "material/Material.h"
#include <glm/glm.hpp>
#include <memory>
#include <shared_mutex>

class ShaderMaterial : public Material
{
public:
    ShaderMaterial(const std::shared_ptr<Shader>& material, std::string name) : Material(std::move(name)), material(material) {};

    void begin() override
    {
        material->begin();
    }

    void end() override
    {
        material->end();
    }

    void SetShader(const std::shared_ptr<Shader>& shader)
    {
        material = shader;
    }

    [[nodiscard]] const std::shared_ptr<Shader>& GetShader() const { return material; }

    [[nodiscard]] std::string GetName() const { return this->name; }

protected:
    std::string name;
    std::shared_ptr<Shader> material;
};

inline std::shared_ptr<Material> DEFAULT_SHADER_MATERIAL = std::make_shared<ShaderMaterial>(std::make_shared<Shader>(), "Default");

class PBRMaterial : public ShaderMaterial
{
public:
    explicit PBRMaterial(std::string name) : ShaderMaterial(std::make_shared<Shader>(), std::move(name))
    {
        Shader shader;
        shader.load("shaders/pbr.vert", "shaders/pbr.frag");
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

    void SetUniforms(const glm::vec3& lightPosition)
    {
        material->setUniform3f("material_color_ambient", material_color_ambient.r / 255.0f, material_color_ambient.g / 255.0f, material_color_ambient.b / 255.0f);
        material->setUniform3f("material_color_diffuse", material_color_diffuse.r / 255.0f, material_color_diffuse.g / 255.0f, material_color_diffuse.b / 255.0f);
        material->setUniform3f("material_color_specular", material_color_specular.r / 255.0f, material_color_specular.g / 255.0f, material_color_specular.b / 255.0f);

        material->setUniform1f("material_brightness", material_brightness);
        material->setUniform1f("material_metallic", material_metallic);
        material->setUniform1f("material_roughness", material_roughness);
        material->setUniform1f("material_occlusion", material_occlusion);

        material->setUniform3f("material_fresnel_ior", material_fresnel_ior);

        material->setUniform1f("light_intensity", light_intensity);
        material->setUniform3f("light_color", light_color.r / 255.0f, light_color.g / 255.0f, light_color.b / 255.0f);
        material->setUniform3f("light_position", lightPosition);

        material->setUniform1f("tone_mapping_exposure", tone_mapping_exposure);
        material->setUniform1f("tone_mapping_gamma", tone_mapping_gamma);
        material->setUniform1i("tone_mapping_toggle", tone_mapping_toggle);
    }

private:
    glm::vec3 material_color_ambient = glm::vec3(63.0f);
    glm::vec3 material_color_diffuse = glm::vec3(255.0f);
    glm::vec3 material_color_specular = glm::vec3(255.0f);

    float material_brightness = 0.5f;
    float material_metallic = 0.5f;
    float material_roughness = 1.0f;
    float material_occlusion = 1.0f;

    glm::vec3 material_fresnel_ior = glm::vec3(0.04f);

    float light_intensity = 1.0f;
    glm::vec3 light_color = glm::vec3(255.0f);

    float tone_mapping_exposure = 1.0f;
    float tone_mapping_gamma = 2.2f;
    bool tone_mapping_toggle = true;
};

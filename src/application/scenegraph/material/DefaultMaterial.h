#pragma once

#include "material/Material.h"
#include "ofMaterial.h"
#include <memory>
#include <string>

class DefaultMaterial : public Material
{
public:
    DefaultMaterial(const std::shared_ptr<ofMaterial>& material, std::string name) : Material(std::move(name)), material(material) {};

    void begin() override
    {
        material->begin();
    }

    void end() override
    {
        material->end();
    }

    [[nodiscard]] const std::shared_ptr<ofMaterial>& GetMaterial() const { return material; }

    [[nodiscard]] std::string GetName() const { return this->name; }

private:
    std::string name;
    std::shared_ptr<ofMaterial> material;
};

inline std::shared_ptr<Material> DEFAULT_MATERIAL = std::make_shared<DefaultMaterial>(std::make_shared<ofMaterial>(), "Default");

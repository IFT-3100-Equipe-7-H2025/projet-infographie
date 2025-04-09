#pragma once

#include "ofMaterial.h"
class Material : public ofMaterial
{
public:
    Material() : ofMaterial(), name("Material") {};
    explicit Material(std::string name) : ofMaterial(), name(std::move(name)) {};
    explicit Material(const ofMaterial& material) : ofMaterial(material), name("Material") {};
    Material(const ofMaterial& material, std::string name) : ofMaterial(material), name(std::move(name)) {};

    [[nodiscard]] std::string GetName() const { return this->name; }

private:
    std::string name;
};

inline Material DEFAULT_MATERIAL = Material("Default");

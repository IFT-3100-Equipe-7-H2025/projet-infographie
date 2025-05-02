#pragma once
#include "HitRecord.h"
#include "Ray.h"
#include "Utilities/Vec3.h"


enum matType
{
    MetalT,
    GlassT,
    LambertT,
    DiffuseLightT
};

constexpr inline const char* materialLabels[] = {
        "Metal",
        "Glass",
        "Lambert",
        "Emissive"
};

// avoids float precision issues with ray hitting the same surface it comes from
constexpr inline float EPSILON = 1e-1f;


class RayMaterial
{
public:

    RayMaterial(Vec3 color, float fuzz, float refraction, float intensity) : albedo(color), fuzz(fuzz), refraction_index(refraction), intensity(intensity)
    {
    }

    virtual ~RayMaterial() = default;

    virtual bool scatter(const Ray& ray_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
        //ofLog() << "Scattering wrong" << endl;
        return false;
    }

    virtual Vec3 emitted() const
    {
        return Vec3(0, 0, 0);
    }

    void setColor(Vec3 color) {
        albedo = color;
    }

    Vec3 getColor()
    {
        return albedo;
    }

    double getFuzz()
    {
        return fuzz;
    }

    double getRefractionIndex()
    {
        return refraction_index;
    }

    float getIntensity()
    {
        return intensity;
    }

    void setFuzz(double p_fuzz)
    {
        fuzz = p_fuzz;
    }

    void setRefractionIndex(double p_refraction_index)
    {
        refraction_index = p_refraction_index;
    }

    void setIntensity(float p_intensity)
    {
        intensity = p_intensity;
    }

    virtual std::shared_ptr<RayMaterial> clone() const = 0;

protected:
    Vec3 albedo = Vec3(1.0f, 1.0f, 1.0f);
    double fuzz = 0.0;
    double refraction_index = 1.0;
    float intensity = 1.0f;
};

class Lambert : public RayMaterial
{
    public:
    Lambert(const Vec3& p_albedo) : RayMaterial(p_albedo, 0.0, 1.0, 1.0f)
    {
    }

    bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override
    {
        Vec3 scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
        {
            scatter_direction = rec.normal;
        }

        scattered = Ray(rec.p + rec.normal * EPSILON, scatter_direction);
        attenuation = albedo;
        return true;
    }

    std::shared_ptr<RayMaterial> clone() const override
    {
        return std::make_shared<Lambert>(*this);
    }
};


class Metal : public RayMaterial
{
public:
    Metal(const Vec3& p_albedo, double p_fuzz) : RayMaterial(p_albedo, p_fuzz, 1.0, 1.0f) {}


    bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override
    {
        Vec3 reflected = reflect(r_in.getDirection(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = Ray(rec.p + rec.normal * EPSILON, reflected);
        attenuation = albedo;
        return (scattered.getDirection().dot(rec.normal) > 0);
    }
    std::shared_ptr<RayMaterial> clone() const override
    {
        return std::make_shared<Metal>(*this);
    }
};


class Dielectric : public RayMaterial
{
public:
    Dielectric(double p_refraction_index) : RayMaterial(Vec3(1.0, 1.0, 1.0), 0.0, p_refraction_index, 1.0f) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered)
            const override
    {
        Vec3 unit_direction = unit_vector(r_in.getDirection());

        double cos_theta = std::fmin(rec.normal.dot(-unit_direction), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        attenuation = Vec3(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        bool cannot_refract = ri * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        {
            //reflect
            direction = reflect(unit_direction, rec.normal);
            scattered = Ray(rec.p + rec.normal * EPSILON, direction);
        }
        else
        {
            //refract
            direction = refract(unit_direction, rec.normal, ri);
            scattered = Ray(rec.p - rec.normal * EPSILON, direction);
        }


        return true;
    }

    std::shared_ptr<RayMaterial> clone() const override
    {
        return std::make_shared<Dielectric>(*this);
    }

private:

    static double reflectance(double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

class DiffuseLight : public RayMaterial
{
public:
    DiffuseLight(Vec3 color, float intense) : RayMaterial(color, 0.0, 0.0f, intense) {}

    Vec3 emitted()  const override 
    {
        return albedo * intensity;
    }

    std::shared_ptr<RayMaterial> clone() const override
    {
        return std::make_shared<DiffuseLight>(*this);
    }

private:
};


inline matType getMaterialType(shared_ptr<RayMaterial> material)
{
    if (shared_ptr<Lambert> lambert = std::dynamic_pointer_cast<Lambert>(material); lambert)
    {
        return LambertT;
    }
    if (shared_ptr<Metal> metal = std::dynamic_pointer_cast<Metal>(material); metal)
    {
        return MetalT;
    }
    if (shared_ptr<Dielectric> glass = std::dynamic_pointer_cast<Dielectric>(material); glass)
    {
        return GlassT;
    }

    if (shared_ptr<DiffuseLight> glass = std::dynamic_pointer_cast<DiffuseLight>(material); glass)
    {
        return DiffuseLightT;
    }
    return LambertT;
}


struct MaterialContainer
{
    std::shared_ptr<RayMaterial> material;
    MaterialContainer(std::shared_ptr<RayMaterial> p_material) : material(p_material)
    {
    }

    shared_ptr<RayMaterial> getMaterial()
    {
        return material;
    }

    void setMaterial(shared_ptr<RayMaterial> p_material)
    {
        material = p_material;
    }

    std::shared_ptr<MaterialContainer> clone() const
    {
        shared_ptr<RayMaterial> newMat = material->clone();
        return std::make_shared<MaterialContainer>(newMat);
    }
};
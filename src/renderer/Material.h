#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "SceneObject.h"
#include "Vec3.h"


enum matType
{
    MetalT,
    GlassT,
    LambertT
};

const char* materialLabels[] = {
        "Metal",
        "Glass",
        "Lambert"};

//inline Material getMaterial(matType type, ofColor albedo, double fuzz, double refracion_index)
//{
//    switch (type)
//    {
//        case Metal:
//            return
//    }
//}
//

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& ray_in, const hit_record& rec, ofColor& attenuation, Ray& scattered) const {
        ofLog() << "Scattering wrong" << endl;
        return false;
    }

    void setColor(ofColor color) {
        albedo = color;
    }

    ofColor getColor()
    {
        return albedo;
    }

    virtual std::shared_ptr<Material> clone() const = 0;

protected:
    ofColor albedo;
};

class Lambert : public Material
{
    public:
    Lambert(const ofColor& p_albedo)
    {
        albedo = p_albedo;
    }

    bool scatter(const Ray& r_in, const hit_record& rec, ofColor& attenuation, Ray& scattered) const override
    {
        Vec3 scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
        {
            scatter_direction = rec.normal;
        }

        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

    std::shared_ptr<Material> clone() const override
    {
        return std::make_shared<Lambert>(*this);
    }


};


class Metal : public Material
{
public:
    Metal(const ofColor& p_albedo, double fuzz) : fuzz(fuzz)
    {
        albedo = p_albedo;
    }


    bool scatter(const Ray& r_in, const hit_record& rec, ofColor& attenuation, Ray& scattered) const override
    {
        Vec3 reflected = reflect(r_in.getDirection(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = Ray(rec.p, reflected);
        attenuation = albedo;
        return (scattered.getDirection().dot(rec.normal) > 0);
    }
    std::shared_ptr<Material> clone() const override
    {
        return std::make_shared<Metal>(*this);
    }


private:
    double fuzz;
};


class Dielectric : public Material
{
public:
    Dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const Ray& r_in, const hit_record& rec, ofColor& attenuation, Ray& scattered)
            const override
    {
        Vec3 unit_direction = unit_vector(r_in.getDirection());

        double cos_theta = std::fmin(rec.normal.dot(-unit_direction), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        attenuation = ofColor(255, 255, 255);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        bool cannot_refract = ri * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        {
            //reflect
            direction = reflect(unit_direction, rec.normal);
        }
        else {
            //refract
            direction = refract(unit_direction, rec.normal, ri);
        }


        scattered = Ray(rec.p, direction);
        return true;
    }

    std::shared_ptr<Material> clone() const override
    {
        return std::make_shared<Dielectric>(*this);
    }

private:
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

#endif
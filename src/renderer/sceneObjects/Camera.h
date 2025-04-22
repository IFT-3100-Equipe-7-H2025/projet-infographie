#pragma once

#pragma once

#include "SceneObject.h"
#include "ofMain.h"

using ViewPort = ofRectangle;

class Camera : public ofCamera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;




    Camera(): activated(false), drawFrustrum(false) {}


    ofImage render(const SceneGraph& sceneGraph) {
        initialize();

        //hitAnyPixel = false;
        ofImage rayImage;

        rayImage.allocate(image_width, image_height, OF_IMAGE_COLOR);
        ofPixels& pixels = rayImage.getPixels();
        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\r Scanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                ofVec3f pixel_center = pixel00_location + (i * pixel_delta_u) + (j * pixel_delta_v);
                ofVec3f ray_direction = (pixel_center - center).getNormalized();
                Ray ray(center, ray_direction);

                ofColor color = rayColor(ray, sceneGraph);
                static const Interval intensity(0.000, 255.000);
                //color.r = int(intensity.clamp(color.r));
                //color.g = int(intensity.clamp(color.g));
                //color.b = int(intensity.clamp(color.b));

                pixels.setColor(i, j, color);
            }
        }
        rayImage.update();
        //if (hitAnyPixel)
        //{
        //    ofLog() << "Hit something!";
        //}
        //else
        //{
        //    ofLog() << "Hit nothing!";
        //}
        //hitAnyPixel = false;
        return rayImage;
    }


    void setViewPort(ofRectangle p_viewport) {
        viewPort = p_viewport;
    }

    ViewPort getViewPort() {
        return viewPort;
    }
    bool& isActivated() {
        return activated;
    }

    void activate() {
        activated = true;
    }

    void deactivate() {
        activated = false;
    }

    void viewFrustrum() {
        drawFrustrum = true;
    }
    
    void hideFrustrum() {
        drawFrustrum = false;
    }

    bool& frustrumVisible() {
        return drawFrustrum;
    }

private:
    int image_height;
    ofVec3f center;
    ofVec3f pixel00_location;
    ofVec3f pixel_delta_u;
    ofVec3f pixel_delta_v;
    bool activated;
    bool drawFrustrum;
    ViewPort viewPort;

    void initialize() {
        aspect_ratio = 16.0 / 9.0;
        image_width = 400;

        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = getGlobalPosition();


        ofVec3f forward = -getZAxis();
        forward = forward.getNormalized();
        ofVec3f right = getXAxis();
        right = right.getNormalized();
        ofVec3f up = getYAxis();
        up = up.getNormalized();

        auto fov_rad = ofDegToRad(getFov());
        auto viewport_height = 2.0f * tan(fov_rad / 2.0f);
        auto viewport_width = viewport_height * aspect_ratio;

        ofVec3f camera_center = getGlobalPosition();

        // These are scaled world-space directions
        auto viewport_u = right * viewport_width;
        auto viewport_v = -up * viewport_height;

        // Focal distance is 1.0, since FOV already affects size
        float focal_length = 1.0f;

        ofVec3f viewport_upper_left = camera_center + forward * focal_length - viewport_u * 0.5f - viewport_v * 0.5f;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        pixel00_location = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);


    }

    ofColor rayColor(const Ray& r, const SceneGraph& sceneGraph) const
    {
        // If the ray hits the background, return a color based on the ray direction.
        hit_record rec;

        if (hitAnything(r, Interval(0, INFINITY), rec, sceneGraph))
        {
            ofColor normal_color = ofColor(
                    (rec.normal.x + 1) * 127.5f,
                    (rec.normal.y + 1) * 127.5f,
                    (rec.normal.z + 1) * 127.5f);
            return normal_color;
        }

        auto unit_direction = r.getDirection().getNormalized();
        auto a = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - a) * ofColor(255, 255, 255) + a * ofColor(127, 200, 255);
    }

    bool hitAnything(const Ray& r, Interval ray_t, hit_record& rec, const SceneGraph& sceneGraph) const
    {
        hit_record temp_rec;
        double closest_so_far = ray_t.max;
        bool hit_anything = false;

        for (const auto& node: sceneGraph.GetNodes())
        {

            if (auto object = std::dynamic_pointer_cast<SceneObject>(node->GetInner()))
            {
                if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
                {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }
        }
  /*      if (hit_anything)
        {
            hitAnyPixel = true;
        }*/
        return hit_anything;
    }

};

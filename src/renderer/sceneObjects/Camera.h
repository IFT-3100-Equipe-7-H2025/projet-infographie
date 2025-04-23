#pragma once

#pragma once

#include "SceneObject.h"
#include "SceneGraph.h"
#include "ofMain.h"
#include "Vec3.h"
#include "Material.h"




using ViewPort = ofRectangle;

class Camera : public ofCamera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 400;
    int new_width = 400;
    int samples_per_pixel = 2;
    int new_samples = 2;
    int max_depth = 4;
    int new_depth = 4;
    float screenCrop = 0.2;
    ofImage rayImage;
    ofImage prevImage;

    ofPixels* pixels = nullptr;
    bool rayTrace = true;



    Camera(): activated(false), drawFrustrum(false), rendering(false) {
        prevImage.allocate(image_width, image_height, OF_IMAGE_COLOR);
        initialize();
    }

    float portionDone() {
        
        return ((float) (image_width * (last_j + 1) + (last_i + 1))) / (float) (image_width * image_height);
    }

    bool doneRendering() {
        return !rendering;
    }

    void reset() {
        initialize();
    }

    ofImage& getRayImage()
    {
        return prevImage;
    }


    ofImage render(const SceneGraph& sceneGraph) {
        initialize();
        
        ofPixels& pixels = rayImage.getPixels();
        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\r Scanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                ofColor pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    Ray r = getRay(i, j);
                    ofColor color = rayColor(r, max_depth, sceneGraph);
                    pixel_color += color;
                }
                pixel_color /= samples_per_pixel;
                ofVec3f pixel_center = pixel00_location + (i * pixel_delta_u) + (j * pixel_delta_v);
                ofVec3f ray_direction = (pixel_center - center).getNormalized();
                Ray ray(center, ray_direction);


                static const Interval intensity(0.000, 255.000);
                //gamma corre
                auto r = linear_to_gamma(pixel_color.r);
                auto g = linear_to_gamma(pixel_color.g);
                auto b = linear_to_gamma(pixel_color.b);

                /*  r = int(intensity.clamp(r));
                g = int(intensity.clamp(g));
                b = int(intensity.clamp(b));*/

                //pixel_color.set(r, g, b);


                //color.r = int(intensity.clamp(color.r));
                //color.g = int(intensity.clamp(color.g));
                //color.b = int(intensity.clamp(color.b));
                //pixel_samples_scale* 
                pixels.setColor(i, j, pixel_color);
            }
        }
        rayImage.update();

        return rayImage;
    }

    ofImage renderPixels(const SceneGraph& sceneGraph)
    {
        initialize();

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\r Scanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                writePixel(i, j, sceneGraph);
            }
        }
        rayImage.update();

        return rayImage;
    }

    void renderPixel(const SceneGraph& sceneGraph) {
        if (!rendering) {
            ofLog() << "Initializing" << endl;
            rendering = true;
            initialize();
            snapshot = sceneGraph;
            //snapshot = sceneGraph.clone();
        }

        writePixel(last_i, last_j, snapshot);
        //ofLog() << "width  : " << image_width << " height : " << image_height << "i  : " << last_i << " j : " << last_j;
        last_i++;
        if (last_i == image_width && last_j == image_height - 1)
        {
            rendering = false;
            rayImage.update();
            //ofLog() << "Allocating from rayImage" << endl;
            prevImage.allocate(rayImage.getWidth(), rayImage.getHeight(), OF_IMAGE_COLOR);
            prevImage.setFromPixels(rayImage);

            //ofLog() << "After allocating from rayImage" << endl;
            last_i = 0;
            last_j = 0;
        }
        else if (last_i == image_width) {
            last_j++;
            last_i = 0;
            //ofLog() << "width  : " << image_width << " height : " << image_height << "i  : " << last_i << " j : " << last_j;
        }
    }

    void writePixel(int i, int j, const SceneGraph& sceneGraph)
    {
        ofColor pixel_color(0, 0, 0);
        float red = 0;
        float green = 0;
        float blue = 0;
        for (int sample = 0; sample < samples_per_pixel; sample++)
        {
            Ray r = getRay(i, j);
            ofColor color = rayColor(r, max_depth, sceneGraph);
            red += color.r;
            green += color.g;
            blue += color.b;
        }
        pixel_color = ofColor(red / samples_per_pixel, green / samples_per_pixel, blue / samples_per_pixel);
        ofVec3f pixel_center = pixel00_location + (i * pixel_delta_u) + (j * pixel_delta_v);
        ofVec3f ray_direction = (pixel_center - center).getNormalized();
        Ray ray(center, ray_direction);


        static const Interval intensity(0.000, 255.000);
        //gamma corre
        auto r = linear_to_gamma(pixel_color.r);
        auto g = linear_to_gamma(pixel_color.g);
        auto b = linear_to_gamma(pixel_color.b);

        /*  r = int(intensity.clamp(r));
                g = int(intensity.clamp(g));
                b = int(intensity.clamp(b));*/

        //pixel_color.set(r, g, b);


        //color.r = int(intensity.clamp(color.r));
        //color.g = int(intensity.clamp(color.g));
        //color.b = int(intensity.clamp(color.b));
        //pixel_samples_scale * 
        pixels->setColor(i, j, pixel_color);

    }


    void setViewPort(ofRectangle p_viewport) {
        viewPort = p_viewport;
        setAspect(viewPort.getWidth() / viewPort.getHeight());
    }


    ViewPort getViewPort() {
        return viewPort;
    }

    void setAspect(float aspect) {
        aspect_ratio = aspect;
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

    bool& isRayTracing()
    {
        return rayTrace;
    }

    
    int& getWidth()
    {
        return new_width;
    }

    int& getDepth()
    {
        return new_depth;
    }

    int& getSamples()
    {
        return new_samples;
    }

    float& getScreenCrop()
    {
        return screenCrop;
    }

private:
    int image_height = 100;
    double pixel_samples_scale;
    ofVec3f center;
    ofVec3f pixel00_location;
    ofVec3f pixel_delta_u;
    ofVec3f pixel_delta_v;
    bool activated;
    bool drawFrustrum;
    ViewPort viewPort;
    SceneGraph snapshot;

    bool rendering = false;
    int last_j = 0;
    int last_i = 0;

    void initialize() {
        last_i = 0;
        last_j = 0;

        image_width = new_width;
        samples_per_pixel = new_samples;
        max_depth = new_depth;

        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        //ofLog() << "Allocating" << endl;
        rayImage.allocate(image_width, image_height, OF_IMAGE_COLOR);
        //ofLog() << "After Allocating" << endl;
        pixels = &rayImage.getPixels();

        pixel_samples_scale = 1.0 / samples_per_pixel;

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


    Ray getRay(int i, int j) const {
        auto offset = sampleSquare();
        auto pixel_sample = pixel00_location + ((i + offset.x) * pixel_delta_u) + ((j + offset.y) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);

    }

    ofVec3f sampleSquare() const {
        return ofVec3f(random_double() - 0.5, random_double() - 0.5, 0);
    }

    ofColor rayColor(const Ray& r, int depth, const SceneGraph& sceneGraph) const
    {
        if (depth <= 0) {
            return ofColor(0, 0, 0);
        }

        // If the ray hits the background, return a color based on the ray direction.
        hit_record rec;

        if (hitAnything(r, Interval(0.0001, INFINITY), rec, sceneGraph))
        {
            Ray scattered;
            ofColor attenuation;

            if (rec.mat->scatter(r, rec, attenuation, scattered))
            {
                return attenuation * rayColor(scattered, depth - 1, sceneGraph);
            }
            return ofColor(0, 0, 0);
            ////Vec3 direction = random_on_hemisphere(rec.normal);
            //Vec3 direction = rec.normal + random_unit_vector();
            ///*ofColor normal_color = ofColor(
            //        (rec.normal.x + 1) * 127.5f,
            //        (rec.normal.y + 1) * 127.5f,
            //        (rec.normal.z + 1) * 127.5f);
            //return normal_color;*/
            //return 0.5 * rayColor(Ray(rec.p, direction), depth - 1, sceneGraph);
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
        return hit_anything;
    }

};




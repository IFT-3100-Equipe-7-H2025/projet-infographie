#pragma once

#pragma once

#include "SceneObject.h"
#include "SceneGraph.h"
#include "ComposedShape.h"
#include "ofMain.h"
#include "Vec3.h"
#include "Material.h"
#include "BvhNode.h"




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
    ofColor backgroundColor1 = ofColor(0, 77, 98);
    ofColor backgroundColor2 = ofColor(255, 200, 250);
    ofImage rayImage;
    ofImage prevImage;
    bool useBvh = true;
    bool drawBvh = false;
    ofPixels* pixels = nullptr;
    bool rayTrace = true;
    shared_ptr<ComposedShape> bvhRoot = make_shared<ComposedShape>();
    vector<shared_ptr<Node>> snapshotObjects;
    ofTrueTypeFont font;



    Camera(): activated(false), drawsFrustum(false), rendering(false) {
        prevImage.allocate(image_width, image_height, OF_IMAGE_COLOR);
        initialize();
        font.load("fonts/JetBrainsMono-Regular.ttf", 12, true, true);
    }


    ofColor getAmbient1()
    {
        return backgroundColor1;
    }

    ofColor getAmbient2()
    {
        return backgroundColor2;
    }

    void setAmbient1(ofColor color)
    {
        backgroundColor1 = color;
    }

    void setAmbient2(ofColor color)
    {
        backgroundColor2 = color;
    }

    float portionDone() {
        
        return ((float) (image_width * (last_j + 1) + (last_i + 1))) / (float) (image_width * image_height);
    }

    string timeLeft() {
        //return time_leftS;
        current_time = ofGetElapsedTimef();
        elapsed_time = current_time - time_last;
        time_since_start = current_time - time_start;
        if (elapsed_time < 1.0f) {
            return time_leftS;
        }
        float perc_dif = portionDone() - elapsed_percent;
        float perc_left = 1.0f - portionDone();

        if (perc_dif <= 1e-8f || portionDone() <= 1e-8f)
        {
            return time_leftS;
        }

        float second_per_perc_recent = elapsed_time / perc_dif;
        float second_per_perc_average = time_since_start / portionDone();
        float alpha = 0.7f; 
        //float second_per_perc = (second_per_perc_recent + second_per_perc_average) / 2.0f;
        //float second_per_perc = second_per_perc_average;
        float second_per_perc = 1.0f / ((alpha / second_per_perc_recent) + ((1.0f - alpha) / second_per_perc_average));

        time_last = current_time;
        elapsed_percent = portionDone();
        float time_left = std::fmax(0, perc_left * second_per_perc);
        float hours = int(time_left / 3600);
        float minutes = int(time_left / 60) - (hours * 60);
        float seconds = int(time_left) - (minutes * 60) - (hours * 3600);
        time_leftS = "";
        if (hours != 0)
        {
            time_leftS += ofToString(hours) + "h ";
            
        }

        time_leftS += ofToString(minutes) + "m ";
        time_leftS += ofToString(seconds) + "s";
       
        return time_leftS;

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

    void customDraw() override
    {
        //ofCamera::customDraw();
        if (drawsFrustum)
        {
            drawFrustum();
        }
        
        //drawBVH();
    }

    void drawBVH() {
        if (drawBvh)
        {
            ofPushStyle();
            ofSetColor(ofColor(255, 0, 0));
            bvhRoot->aabbDraw();
            ofPopStyle();
        }
    }

    void drawRayTrace() {
        if (isRayTracing())
        {
            //ofSetColor(clearColor);
            ofSetColor(ofColor(255, 255, 255));
            ViewPort viewPort = getViewPort();
            int new_width = viewPort.getWidth() * getScreenCrop();
            int new_height = viewPort.getHeight() * getScreenCrop();
            //rayImage = camera->getRayImage();
            ofDisableDepthTest();
            prevImage.draw(viewPort.getX() + viewPort.getWidth() - new_width, viewPort.getY(), new_width, new_height);
            float percent = portionDone() * 100;
            string time_left = timeLeft();
            string perc = std::format("Time Left: {} , %: {:.4f}", time_left, percent);
            ofPushStyle();
            ofSetColor(0, 255, 0);
            ofDisableDepthTest();

            ofPushStyle();
            ofRectangle rectangle(viewPort.getX() + viewPort.getWidth() - static_cast<float>(perc.size() - 1) * 12.0f, viewPort.getY() + new_height - 30.0f,
                                  static_cast<float>(perc.size() - 1) * 12.0f, 30.0f);
            ofFill();
            ofSetColor(ofColor(0, 0, 0));
            ofDrawRectangle(rectangle);
            ofPopStyle();

            font.drawString(perc, static_cast<float>(viewPort.getX() + viewPort.getWidth() - static_cast<float>(perc.size() - 1) * 12.0f), viewPort.getY() + new_height - 12.0f);
            ofEnableDepthTest();


            ofPopStyle();

            ofEnableDepthTest();
            ofSetColor(0, 255, 0);
        }
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
            initialize();
            ComposedShape shape(sceneGraph.GetNodes());
            //bvhRoot = make_shared<ComposedShape>(shape);
            bvhRoot = make_shared<ComposedShape>(make_shared<BvhNode>(shape), shape.getMaterialContainer());
            bvhRoot->update();
            snapshotObjects = sceneGraph.GetNodes();
            rendering = true;
            snapshot = sceneGraph;
        }

        writePixel(last_i, last_j, snapshot);
        last_i++;
        if (last_i == image_width && last_j == image_height - 1)
        {
            rendering = false;
            rayImage.update();
            prevImage.allocate(rayImage.getWidth(), rayImage.getHeight(), OF_IMAGE_COLOR);
            prevImage.setFromPixels(rayImage);

            last_i = 0;
            last_j = 0;
        }
        else if (last_i == image_width) {
            last_j++;
            last_i = 0;
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
        //ofVec3f pixel_center = pixel00_location + (i * pixel_delta_u) + (j * pixel_delta_v);
        //ofVec3f ray_direction = (pixel_center - center).getNormalized();
        //Ray ray(center, ray_direction);


        //static const Interval intensity(0.000, 255.000);
        //gamma corre
        //auto r = linear_to_gamma(pixel_color.r);
        //auto g = linear_to_gamma(pixel_color.g);
        //auto b = linear_to_gamma(pixel_color.b);

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

    void viewFrustum() {
        drawsFrustum = true;
    }
    
    void hideFrustum() {
        drawsFrustum = false;
    }

    bool& FrustumVisible() {
        return drawsFrustum;
    }

    bool& getUseBvh()
    {
        return useBvh;
    }

    bool& getDrawBvh()
    {
        return drawBvh;
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

    void saveImage()
    {
        int samples = getSamples();
        int depth = getDepth();
        ofImage rayTrace = getRayImage();
        int width = rayTrace.getWidth();
        int height = rayTrace.getHeight();
        string filename = ofToString(width) + "x" + ofToString(height) + "_D" + ofToString(depth) + "_R" + ofToString(samples) + ".png";
        ofFileDialogResult saveDialog = ofSystemSaveDialog("default.png", "Save your image");
        if (saveDialog.bSuccess)
        {
            rayTrace.save(saveDialog.getPath() + filename);
        }
    }

private:
    float time_since_start;
    float time_start;
    float time_last;
    float current_time;
    float elapsed_time;
    float elapsed_percent;
    string time_leftS = "00:00";
    int image_height = 100;
    double pixel_samples_scale;
    ofVec3f center;
    ofVec3f pixel00_location;
    ofVec3f pixel_delta_u;
    ofVec3f pixel_delta_v;
    bool activated;
    bool drawsFrustum;
    ViewPort viewPort;
    SceneGraph snapshot;

    bool rendering = false;
    int last_j = 0;
    int last_i = 0;

    void initialize() {
        rendering = false;
        time_start = time_last = ofGetElapsedTimef();
        last_i = 0;
        last_j = 0;
        elapsed_percent = 0.0f;

        image_width = new_width;
        samples_per_pixel = new_samples;
        max_depth = new_depth;

        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        rayImage.allocate(image_width, image_height, OF_IMAGE_COLOR);
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

    ofColor rayColor(const Ray& r, int depth, const SceneGraph& sceneGraph)
    {
        if (depth <= 0)
        {
            return ofColor(0, 0, 0);
        }

        // If the ray hits the background, return a color based on the ray direction.
        HitRecord rec;



        if (!hitAnything(r, Interval(0.0001, INFINITY), rec, sceneGraph))
        {
            //return backgroundColor;
            auto unit_direction = r.getDirection().getNormalized();
            auto a = 0.5 * (unit_direction.y + 1.0);
            // get the backgroundColor + a little brighter

            return (1.0 - a) * backgroundColor2 + a * backgroundColor1;
            //return (1.0 - a) * ofColor(255, 255, 255) + a * ofColor(127, 200, 255);
        }


        Ray scattered;
        ofColor attenuation;
        ofColor emitted = rec.mat->emitted();

        if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
            return emitted;
        }

        ofColor scatterColor = attenuation * rayColor(scattered, depth - 1, sceneGraph);
        return emitted * 5 + scatterColor;
        //return ofColor(0, 0, 0);
        ////Vec3 direction = random_on_hemisphere(rec.normal);
        //Vec3 direction = rec.normal + random_unit_vector();
        ///*ofColor normal_color = ofColor(
        //        (rec.normal.x + 1) * 127.5f,
        //        (rec.normal.y + 1) * 127.5f,
        //        (rec.normal.z + 1) * 127.5f);
        //return normal_color;*/
        //return 0.5 * rayColor(Ray(rec.p, direction), depth - 1, sceneGraph);

        
        
    }

    bool hitAnything(const Ray& r, Interval ray_t, HitRecord& rec, const SceneGraph& sceneGraph)
    {
        HitRecord temp_rec;
        double closest_so_far = ray_t.max;
        bool hit_anything = false;

        if (useBvh)
        {
            if (bvhRoot->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        else
        {
            for (const auto& node: snapshotObjects)
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
        }

        return hit_anything;
    }
};
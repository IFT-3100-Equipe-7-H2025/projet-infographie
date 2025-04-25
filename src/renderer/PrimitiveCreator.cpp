#include "PrimitiveCreator.h"
#include <cmath>
#include <numbers>
#include "RayMesh.h"


of3dPrimitive PrimitiveCreator::createTriangle()
{
    ofMesh triangle;
    triangle.setMode(OF_PRIMITIVE_TRIANGLES);
    auto p1 = ofPoint(0.5, 0, -0.5);
    auto p2 = ofPoint(0, 0.5, -0.5);
    auto p3 = ofPoint(-0.5, 0, -0.5);
    triangle.addVertex(p1);
    triangle.addVertex(p2);
    triangle.addVertex(p3);

    // calculate normals
    for (size_t i = 0; i < triangle.getNumVertices(); i++)
    {
        triangle.addNormal(normalize(triangle.getVertex(i)));
    }

    return of3dPrimitive{triangle};
}

of3dPrimitive PrimitiveCreator::createCube(float width, float height, float depth)
{
    ofMesh cube;
    cube.setMode(OF_PRIMITIVE_TRIANGLES);

    auto p1 = ofPoint(-0.5, -0.5, -0.5);// 1
    auto p2 = ofPoint(0.5, -0.5, -0.5); // 2
    auto p3 = ofPoint(0.5, 0.5, -0.5);  // 3
    auto p4 = ofPoint(-0.5, 0.5, -0.5); // 4
    auto p5 = ofPoint(-0.5, -0.5, 0.5); // 5
    auto p6 = ofPoint(0.5, -0.5, 0.5);  // 6
    auto p7 = ofPoint(0.5, 0.5, 0.5);   // 7
    auto p8 = ofPoint(-0.5, 0.5, 0.5);  // 8

    // FRONT FACE: vertices: p5, p6, p7, p8 with normal (0, 0, 1)
    ofVec3f frontNormal(0, 0, 1);
    size_t startIndex = cube.getNumVertices();
    cube.addVertex(p5);
    cube.addNormal(frontNormal);

    cube.addVertex(p6);
    cube.addNormal(frontNormal);

    cube.addVertex(p7);
    cube.addNormal(frontNormal);

    cube.addVertex(p8);
    cube.addNormal(frontNormal);

    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 1);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 3);

    // BACK FACE: vertices: p2, p1, p4, p3 with normal (0, 0, -1)
    ofVec3f backNormal(0, 0, -1);
    startIndex = cube.getNumVertices();
    cube.addVertex(p2);
    cube.addNormal(backNormal);

    cube.addVertex(p1);
    cube.addNormal(backNormal);

    cube.addVertex(p4);
    cube.addNormal(backNormal);

    cube.addVertex(p3);
    cube.addNormal(backNormal);

    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 1);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 3);

    // LEFT FACE: vertices: p1, p5, p8, p4 with normal (-1, 0, 0)
    ofVec3f leftNormal(-1, 0, 0);
    startIndex = cube.getNumVertices();
    cube.addVertex(p1);
    cube.addNormal(leftNormal);

    cube.addVertex(p5);
    cube.addNormal(leftNormal);

    cube.addVertex(p8);
    cube.addNormal(leftNormal);

    cube.addVertex(p4);
    cube.addNormal(leftNormal);

    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 1);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 3);

    // RIGHT FACE: vertices: p6, p2, p3, p7 with normal (1, 0, 0)
    ofVec3f rightNormal(1, 0, 0);
    startIndex = cube.getNumVertices();
    cube.addVertex(p6);
    cube.addNormal(rightNormal);

    cube.addVertex(p2);
    cube.addNormal(rightNormal);

    cube.addVertex(p3);
    cube.addNormal(rightNormal);

    cube.addVertex(p7);
    cube.addNormal(rightNormal);

    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 1);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 3);

    // TOP FACE: vertices: p4, p8, p7, p3 with normal (0, 1, 0)
    ofVec3f topNormal(0, 1, 0);
    startIndex = cube.getNumVertices();
    cube.addVertex(p4);
    cube.addNormal(topNormal);

    cube.addVertex(p8);
    cube.addNormal(topNormal);

    cube.addVertex(p7);
    cube.addNormal(topNormal);

    cube.addVertex(p3);
    cube.addNormal(topNormal);

    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 1);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 3);

    // BOTTOM FACE: vertices: p1, p2, p6, p5 with normal (0, -1, 0)
    ofVec3f bottomNormal(0, -1, 0);
    startIndex = cube.getNumVertices();
    cube.addVertex(p1);
    cube.addNormal(bottomNormal);

    cube.addVertex(p2);
    cube.addNormal(bottomNormal);

    cube.addVertex(p6);
    cube.addNormal(bottomNormal);

    cube.addVertex(p5);
    cube.addNormal(bottomNormal);

    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 1);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 0);
    cube.addIndex(startIndex + 2);
    cube.addIndex(startIndex + 3);

    // Scale all vertices to size
    for (size_t i = 0; i < cube.getNumVertices(); i++)
    {
        ofPoint p = cube.getVertex(i);
        p.x *= width;
        p.y *= height;
        p.z *= depth;
        cube.setVertex(i, p);
    }

    return of3dPrimitive{cube};
}

RayMesh PrimitiveCreator::createSphere(int lat, int longi, float radius, shared_ptr<Material> mat){
    return RayMesh(mat, createSphere(lat, longi, radius));
}

of3dPrimitive PrimitiveCreator::createSphere(int lat, int longi, float radius)
{
    ofMesh sphere;
    sphere.setMode(OF_PRIMITIVE_TRIANGLES);
    sphere.enableNormals();
    sphere.enableColors();

    float r = 1.0f;
    ofPoint top = ofPoint{0.0f, 1.0f, 0.0f};
    sphere.addVertex(top);
    assert(longi >= 3);
    assert(lat >= 1);

    float delta_phi = 180.0f / (float) (lat + 1);
    float delta_theta = 360.0f / (float) longi;

    std::vector<ofPoint> topLayer = std::vector<ofPoint>();
    float phi_tl = 90.0f - delta_phi;
    float y_tl = r * (float) sin(phi_tl * std::numbers::pi / 180.0f);

    // Adding top layer positions
    for (int i = 0; i < longi; i++)
    {
        float theta = delta_theta * (float) i;
        float hor_proj = r * (float) cos(phi_tl * std::numbers::pi / 180.0f);
        float x_tl = hor_proj * (float) cos(theta * std::numbers::pi / 180.0f);
        float z_tl = hor_proj * (float) sin(theta * std::numbers::pi / 180.0f);
        sphere.addVertex(ofPoint(x_tl, y_tl, z_tl));
    }

    // Adding top layer faces
    for (int i = 0; i < longi; i++)
    {
        int top = 0;
        int left = i + 1;
        int right = (i + 1) % longi + 1;

        sphere.addIndex(left);
        sphere.addIndex(right);
        sphere.addIndex(top);
    }

    //Middle layer
    for (int i = 1; i < lat; i++)
    {
        float phi_bl = phi_tl - delta_phi;
        for (int j = 0; j < longi; j++)
        {
            float theta = delta_theta * (float) j;
            float hor_proj = r * (float) cos(phi_bl * std::numbers::pi / 180.0f);
            float x_bl = hor_proj * (float) cos(theta * std::numbers::pi / 180.0f);
            float y_bl = r * (float) sin(phi_bl * std::numbers::pi / 180.0f);
            float z_bl = hor_proj * (float) sin(theta * std::numbers::pi / 180.0f);
            sphere.addVertex(ofPoint(x_bl, y_bl, z_bl));
        }


        for (int j = 0; j < longi; j++)
        {
            int top_left = (i - 1) * longi + j + 1;
            int top_right = (i - 1) * longi + (j + 1) % longi + 1;
            int bottom_left = i * longi + j + 1;
            int bottom_right = i * longi + (j + 1) % longi + 1;

            sphere.addIndex(top_left);
            sphere.addIndex(bottom_left);
            sphere.addIndex(bottom_right);

            sphere.addIndex(top_left);
            sphere.addIndex(bottom_right);
            sphere.addIndex(top_right);
        }

        phi_tl = phi_bl;
    }
    sphere.addVertex(ofPoint(0, -1, 0));

    //bottom layer
    for (int j = 0; j < longi; j++)
    {
        int top_left = (lat - 1) * longi + j + 1;
        int top_right = (lat - 1) * longi + (j + 1) % longi + 1;
        int bottom = lat * longi + 1;
        sphere.addIndex(top_left);
        sphere.addIndex(bottom);
        sphere.addIndex(top_right);
    }

    // scale points to radius
    for (size_t i = 0; i < sphere.getNumVertices(); i++)
    {
        sphere.setVertex(i, sphere.getVertex(i) * radius);
    }

    // calculate normals
    for (size_t i = 0; i < sphere.getNumVertices(); i++)
    {
        sphere.addNormal(normalize(sphere.getVertex(i)));
    }

    return of3dPrimitive{sphere};
}

of3dPrimitive PrimitiveCreator::createLasagna(float l_w_ratio, int periods, int resolution_l, int resolution_w, float w, float h, float d)
{
    assert(resolution_w >= 2);
    assert(resolution_l >= 2);
    ofMesh lasagna;
    lasagna.setMode(OF_PRIMITIVE_TRIANGLES);

    float width = 2 * l_w_ratio;
    float d_w = width / float(resolution_w - 1);
    auto d_x = float(2 / float(resolution_l * periods));
    float x_f = -1.0f;
    float lambda = 2.0f / (float) periods;
    float k = 2.0f * (float) std::numbers::pi / lambda;

    for (int frontier = 0; frontier < periods * resolution_l + 1; frontier++)
    {
        float z_0 = -l_w_ratio;
        for (int i = 0; i < resolution_w; i++)
        {
            lasagna.addVertex(ofPoint(x_f, std::sin(k * (x_f - 2.0f)), z_0));
            z_0 += d_w;
        }
        x_f += d_x;
    }


    for (int col = 0; col < periods * resolution_l; col++)
    {
        for (int row = 0; row < resolution_w - 1; row++)
        {
            int stride = resolution_w;
            int p1 = col * stride + row;
            int p2 = p1 + 1;
            int p3 = p1 + stride;
            int p4 = p3 + 1;


            //top quad
            lasagna.addIndex(p1);
            lasagna.addIndex(p3);
            lasagna.addIndex(p2);

            lasagna.addIndex(p2);
            lasagna.addIndex(p3);
            lasagna.addIndex(p4);
        }
    }

    // scale points to size
    for (size_t i = 0; i < lasagna.getNumVertices(); i++)
    {
        lasagna.setVertex(i, lasagna.getVertex(i) * ofPoint{w, h, d});
    }

    // calculate normals
    for (size_t i = 0; i < lasagna.getNumVertices(); i++)
    {
        lasagna.addNormal(normalize(lasagna.getVertex(i)));
    }

    return of3dPrimitive{lasagna};
}

of3dPrimitive PrimitiveCreator::createPyramid(int sides, float width, float height, float depth)
{
    float delta_theta = 2.0f * float(std::numbers::pi) / float(sides);
    ofMesh pyramid;
    pyramid.setMode(OF_PRIMITIVE_TRIANGLES);

    ofPoint apex(0, -1, 0);
    ofPoint baseCenter(0, 1, 0);

    for (int i = 0; i < sides; i++)
    {
        int next = (i + 1) % sides;
        float theta_i = static_cast<float>(i) * delta_theta;
        float theta_next = static_cast<float>(next) * delta_theta;
        ofPoint base_i(std::cos(theta_i), 1, std::sin(theta_i));
        ofPoint base_next(std::cos(theta_next), 1, std::sin(theta_next));
        ofPoint v0 = apex;
        ofPoint v1 = base_next;
        ofPoint v2 = base_i;
        ofVec3f edge1 = v1 - v0;
        ofVec3f edge2 = v2 - v0;
        ofVec3f normal = (edge1.cross(edge2)).normalize() * -1;
        size_t startIndex = pyramid.getNumVertices();
        pyramid.addVertex(v0);
        pyramid.addNormal(normal);
        pyramid.addVertex(v1);
        pyramid.addNormal(normal);
        pyramid.addVertex(v2);
        pyramid.addNormal(normal);
        pyramid.addIndex(startIndex + 0);
        pyramid.addIndex(startIndex + 1);
        pyramid.addIndex(startIndex + 2);
    }

    ofVec3f baseNormal(0, 1, 0);
    for (int i = 0; i < sides; i++)
    {
        int next = (i + 1) % sides;
        float theta_i = static_cast<float>(i) * delta_theta;
        float theta_next = static_cast<float>(next) * delta_theta;
        ofPoint base_i(std::cos(theta_i), 1, std::sin(theta_i));
        ofPoint base_next(std::cos(theta_next), 1, std::sin(theta_next));
        size_t startIndex = pyramid.getNumVertices();
        pyramid.addVertex(baseCenter);
        pyramid.addNormal(baseNormal);
        pyramid.addVertex(base_i);
        pyramid.addNormal(baseNormal);
        pyramid.addVertex(base_next);
        pyramid.addNormal(baseNormal);
        pyramid.addIndex(startIndex + 0);
        pyramid.addIndex(startIndex + 1);
        pyramid.addIndex(startIndex + 2);
    }

    for (size_t i = 0; i < pyramid.getNumVertices(); i++)
    {
        ofPoint p = pyramid.getVertex(i);
        p.x *= width;
        p.y *= height;
        p.z *= depth;
        pyramid.setVertex(i, p);
    }

    return of3dPrimitive{pyramid};
}

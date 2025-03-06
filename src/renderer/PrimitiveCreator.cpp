#include "PrimitiveCreator.h"


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
    for (int i = 0; i < triangle.getNumVertices(); i++)
    {
        triangle.addNormal(normalize(triangle.getVertex(i)));
    }

    return of3dPrimitive{triangle};
}

of3dPrimitive PrimitiveCreator::createCube()
{
    ofMesh cube;
    cube.setMode(OF_PRIMITIVE_TRIANGLES);

    auto p1 = ofPoint(-1, -1, -1);// 0
    auto p2 = ofPoint(1, -1, -1); // 1
    auto p3 = ofPoint(1, 1, -1);  // 2
    auto p4 = ofPoint(-1, 1, -1); // 3
    auto p5 = ofPoint(-1, -1, 1); // 4
    auto p6 = ofPoint(1, -1, 1);  // 5
    auto p7 = ofPoint(1, 1, 1);   // 6
    auto p8 = ofPoint(-1, 1, 1);  // 7

    // Add all vertices
    cube.addVertex(p1);// 0
    cube.addVertex(p2);// 1
    cube.addVertex(p3);// 2
    cube.addVertex(p4);// 3
    cube.addVertex(p5);// 4
    cube.addVertex(p6);// 5
    cube.addVertex(p7);// 6
    cube.addVertex(p8);// 7

    // Front face (p5, p6, p7, p8)
    cube.addIndex(4);
    cube.addIndex(5);
    cube.addIndex(6);
    cube.addIndex(4);
    cube.addIndex(6);
    cube.addIndex(7);

    // Back face (p1, p2, p3, p4)
    cube.addIndex(0);
    cube.addIndex(1);
    cube.addIndex(2);
    cube.addIndex(0);
    cube.addIndex(2);
    cube.addIndex(3);

    // Left face (p1, p5, p8, p4)
    cube.addIndex(0);
    cube.addIndex(4);
    cube.addIndex(7);
    cube.addIndex(0);
    cube.addIndex(7);
    cube.addIndex(3);

    // Right face (p2, p6, p7, p3)
    cube.addIndex(1);
    cube.addIndex(5);
    cube.addIndex(6);
    cube.addIndex(1);
    cube.addIndex(6);
    cube.addIndex(2);

    // Top face (p4, p3, p7, p8)
    cube.addIndex(3);
    cube.addIndex(2);
    cube.addIndex(6);
    cube.addIndex(3);
    cube.addIndex(6);
    cube.addIndex(7);

    // Bottom face (p1, p2, p6, p5)
    cube.addIndex(0);
    cube.addIndex(1);
    cube.addIndex(5);
    cube.addIndex(0);
    cube.addIndex(5);
    cube.addIndex(4);

    // calculate normals
    for (int i = 0; i < cube.getNumVertices(); i++)
    {
        cube.addNormal(normalize(cube.getVertex(i)));
    }

    return of3dPrimitive{cube};
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

    // calculate normals
    for (int i = 0; i < sphere.getNumVertices(); i++)
    {
        sphere.addNormal(normalize(sphere.getVertex(i)));
    }

    // scale points to radius
    for (int i = 0; i < sphere.getNumVertices(); i++)
    {
        sphere.setVertex(i, sphere.getVertex(i) * radius);
    }

    return of3dPrimitive{sphere};
}

of3dPrimitive PrimitiveCreator::createLasagna(float l_w_ratio, int periods, int resolution_l, int resolution_w)
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
            lasagna.addVertex(ofPoint(x_f, sin(k * (x_f - 2.0f)), z_0));
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

    // calculate normals
    for (int i = 0; i < lasagna.getNumVertices(); i++)
    {
        lasagna.addNormal(normalize(lasagna.getVertex(i)));
    }

    return of3dPrimitive{lasagna};
}

of3dPrimitive PrimitiveCreator::createPyramid(int sides)
{
    float delta_theta = 2.0f * float(std::numbers::pi) / float(sides);
    ofMesh pyramid;
    pyramid.setMode(OF_PRIMITIVE_TRIANGLES);

    for (int i = 0; i < sides; i++)
    {
        pyramid.addVertex(ofPoint(cos(float(i) * delta_theta), -1, sin(float(i) * delta_theta)));
    }
    pyramid.addVertex(ofPoint(0, 1, 0));
    pyramid.addVertex(ofPoint(0, -1, 0));

    for (int i = 0; i < sides; i++)
    {
        pyramid.addIndex(sides);
        pyramid.addIndex(i);
        pyramid.addIndex((i + 1) % sides);

        pyramid.addIndex(sides + 1);
        pyramid.addIndex((i + 1) % sides);
        pyramid.addIndex(i);
    }

    // calculate normals
    for (int i = 0; i < pyramid.getNumVertices(); i++)
    {
        pyramid.addNormal(normalize(pyramid.getVertex(i)));
    }

    return of3dPrimitive{pyramid};
}

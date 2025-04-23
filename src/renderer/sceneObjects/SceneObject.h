#pragma once

#include "ofMain.h"
#include "renderer/rayTracer/ray.h"
#include "renderer/Interval.h"
//#include "renderer/Material.h"

class Material;

class hit_record
{
public:
    ofVec3f p;
    ofVec3f normal;
    shared_ptr<Material> mat;
    float t;
    bool front_face;

    void set_face_normal(const Ray& r, const ofVec3f& outward_normal)
    {
        front_face = r.getDirection().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class SceneObject : public ofNode
{
public:
    virtual std::pair<ofVec3f, ofVec3f> getBoundingVertices() = 0;

    virtual std::vector<ofVec3f> getMeshVertices(ofMesh mesh)
    {
        ofMatrix4x4 transform = this->getGlobalTransformMatrix();

        vector<ofVec3f> transformedVertices;
        for (auto& vertex: mesh.getVertices())
        {
            ofVec4f transformedVertex = transform.preMult(ofVec4f(vertex.x, vertex.y, vertex.z, 1.0f));
            transformedVertices.push_back(ofVec3f(transformedVertex.x, transformedVertex.y, transformedVertex.z));
        }
        return transformedVertices;
    }

    
    virtual bool hit(const Ray& r, Interval ray_t, hit_record& rec)
    {
        return false;
    }

    std::pair<ofVec3f, ofVec3f> getBoundingVerticesFromVector(std::vector<ofVec3f> vertices)
    {
        ofVec3f minVertex = vertices[0];
        ofVec3f maxVertex = vertices[0];

        for (int i = 0; i < vertices.size(); i++)
        {
            minVertex.x = std::min(vertices[i].x, minVertex.x);
            maxVertex.x = std::max(vertices[i].x, maxVertex.x);
            minVertex.y = std::min(vertices[i].y, minVertex.y);
            maxVertex.y = std::max(vertices[i].y, maxVertex.y);
            minVertex.z = std::min(vertices[i].z, minVertex.z);
            maxVertex.z = std::max(vertices[i].z, maxVertex.z);
        }

        return std::pair(minVertex, maxVertex);
    };


    ofMesh getSelectionBox()
    {
        pair<ofVec3f, ofVec3f> vertices = getBoundingVertices();
        ofVec3f minVertex = vertices.first;
        ofVec3f maxVertex = vertices.second;

        ofMesh new_mesh;
        new_mesh.setMode(OF_PRIMITIVE_LINE_LOOP);

        new_mesh.addVertex(minVertex);
        new_mesh.addVertex(ofVec3f(minVertex.x, maxVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(maxVertex.x, maxVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(maxVertex.x, minVertex.y, minVertex.z));


        new_mesh.addVertex(ofVec3f(maxVertex.x, minVertex.y, maxVertex.z));
        new_mesh.addVertex(ofVec3f(maxVertex.x, maxVertex.y, maxVertex.z));
        new_mesh.addVertex(ofVec3f(maxVertex.x, maxVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(maxVertex.x, minVertex.y, minVertex.z));


        new_mesh.addVertex(ofVec3f(maxVertex.x, minVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, minVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, minVertex.y, maxVertex.z));
        new_mesh.addVertex(ofVec3f(maxVertex.x, minVertex.y, maxVertex.z));


        new_mesh.addVertex(ofVec3f(maxVertex.x, maxVertex.y, maxVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, maxVertex.y, maxVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, minVertex.y, maxVertex.z));

        new_mesh.addVertex(ofVec3f(minVertex.x, minVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, maxVertex.y, minVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, maxVertex.y, maxVertex.z));
        new_mesh.addVertex(ofVec3f(minVertex.x, minVertex.y, maxVertex.z));

        return new_mesh;
    }

protected:
    shared_ptr<Material> mat;

};

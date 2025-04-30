#pragma once

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "ofMain.h"
#include "renderer/rayTracer/Ray.h"
#include "renderer/rayTracer/Interval.h"
#include "HitRecord.h"
#include "RayMaterial.h"
#include "AABB.h"
//#include "Material.h"

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

    
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec)
    {
        //ofLog() << "Default no hit" << endl;
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

    shared_ptr<RayMaterial> getMaterial()
    {
        return mat;
    }

    void setMaterial(shared_ptr<RayMaterial> material)
    {
        mat->setColor(material->getColor());
        mat->setFuzz(material->getFuzz());
        mat->setRefractionIndex(material->getRefractionIndex());
    }



protected:
    shared_ptr<RayMaterial> mat;

};
#endif
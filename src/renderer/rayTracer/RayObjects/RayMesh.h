#ifndef RAYMESH_H
#define RAYMESH_H


#include "Primitive3D.h"
#include "ComposedShape.h"
#include "Triangle.h"
#include "Material.h"
#include "RayMaterial.h"
#include <numbers>


class RayMesh : public ComposedShape
{
public:

    RayMesh(shared_ptr<MaterialContainer> mat, of3dPrimitive primitive) : ComposedShape(size, mat, primitive)
    {
        setShapes();
    }
    void setShapes() override{
        ofMesh mesh = model.getMesh();
        vector<glm::vec3> vertices = mesh.getVertices();
        vector<ofIndexType> indices = mesh.getIndices();// usually u16/u32

        for (int i = 0; i < indices.size(); i += 3)
        {
            ofVec3f v0 = vertices[indices[i]];
            ofVec3f v1 = vertices[indices[i + 1]];
            ofVec3f v2 = vertices[indices[i + 2]];
            Triangle triangle = Triangle::triangleFromPoints(center, scale, orientation, v0, v1, v2, mat);
            //ofLog() << "V1 " << v0 << " V2 " << v1 << " V3 " << v2;
            addShape(make_shared<Triangle>(triangle));
        }
    }

};


#endif
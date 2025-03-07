#include "ImportModel.h"
#include "SceneObject.h"


std::vector<ofVec3f> ImportModel::getMeshVertices(ofMesh mesh)
{
    ofMatrix4x4 transform = getGlobalTransformMatrix()  * model.getModelMatrix();


    vector<ofVec3f> transformedVertices;
    for (auto& vertex: mesh.getVertices())
    {
        ofVec4f transformedVertex = transform.preMult(ofVec4f(vertex.x, vertex.y, vertex.z, 1.0f));
        transformedVertices.push_back(ofVec3f(transformedVertex.x, transformedVertex.y, transformedVertex.z));
    }
    return transformedVertices;
};

std::pair<ofVec3f, ofVec3f> ImportModel::getBoundingVertices() {
    vector<ofVec3f> vertices = getMeshVertices(getCombinedMesh());

    return getBoundingVerticesFromVector(vertices);
}

ofMesh ImportModel::getCombinedMesh() {
    int meshNum = model.getNumMeshes();
    ofMesh combinedMesh;
    for (int i = 0; i < meshNum; i++)
    {
        combinedMesh.append(model.getMesh(i));
    }
    return combinedMesh;
}

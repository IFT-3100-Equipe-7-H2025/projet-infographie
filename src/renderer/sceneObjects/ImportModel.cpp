#include "ImportModel.h"
#include "SceneObject.h"


std::vector<ofVec3f> ImportModel::getMeshVertices(ofMesh mesh)
{
    model.calculateDimensions();
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
    //vector<ofVec3f> vertices = getMeshVertices(getCombinedMesh());
    ofVec3f min3 = model.getSceneMinModelSpace();
    ofVec3f max3 = model.getSceneMaxModelSpace();
    ofVec4f min = ofVec4f(min3.x, min3.y, min3.z, 1);
    ofVec4f max = ofVec4f(max3.x, max3.y, max3.z, 1);
    //ofLog() << "Before " << min;
    //ofLog() << "Before " << getGlobalTransformMatrix();
    min =  getGlobalTransformMatrix() * min;
    //ofLog() << "After " << min;
    max = getGlobalTransformMatrix() * max;

    vector<ofVec3f> corners = {
            ofVec3f(min.x, min.y, min.z),
            ofVec3f(min.x, max.y, min.z),
            ofVec3f(max.x, max.y, min.z),
            ofVec3f(max.x, min.y, min.z),

            ofVec3f(min.x, min.y, max.z),
            ofVec3f(min.x, max.y, max.z),
            ofVec3f(max.x, max.y, max.z),
            ofVec3f(max.x, min.y, max.z)};


   vector<ofVec3f> vertices = corners;
    

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

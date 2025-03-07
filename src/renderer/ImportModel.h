#pragma once

#include "ofMain.h"
#include "SceneObject.h"
#include "ofxAssimpModelLoader.h"

class ImportModel : public SceneObject
{
public:
    void loadModel(string filePath) { model.loadModel(filePath);}
    void customDraw() override{
        ofPushMatrix();
        model.drawFaces();
        ofPopMatrix();
    }

    bool loaded() {
        return model.getNumMeshes() > 0;
    }


    std::vector<ofVec3f> getMeshVertices(ofMesh mesh) override;

    std::pair<ofVec3f, ofVec3f> getBoundingVertices() override;

private:
    ofxAssimpModelLoader model;
    ofMesh getCombinedMesh();
};

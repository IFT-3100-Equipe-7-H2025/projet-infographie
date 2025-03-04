
#include "GeometryScene.h"
#include "ofGraphics.h"
#include "ofGraphicsConstants.h"
#include "ofShader.h"
#include <cmath>
#include <numbers>
#include <ofMesh.h>

ofShader shader;
void GeometryScene::setup()
{
    shader.load("shader.vert", "shader.frag");
}

void GeometryScene::draw()
{
    ofColor bgColor(backgroundColor[0] * 255, backgroundColor[1] * 255, backgroundColor[2] * 255, backgroundColor[3] * 255);
    ofClear(bgColor);
    auto triangle = createTriangle();

    triangle.setScale(1000, 1000, 1000);

    ofMatrix4x4 projMatrix;
    projMatrix.makePerspectiveMatrix(90, (double) ofGetWidth() / (double) ofGetHeight(), 10, 1000);

    ofCamera camera;
    //camera.lookAt(ofVec3f(0,0,0));
    //camera.setPosition(ofVec3f(0, 0, -1000));
    ofMatrix4x4 viewMatrix = camera.getModelViewMatrix();

    ofPushMatrix();

    ofPushStyle();

    //ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    ofSetColor(255, 0, 0);
    shader.begin();
    shader.setUniformMatrix4f("projectionMatrix", projMatrix);
    shader.setUniformMatrix4f("modelViewMatrix", viewMatrix);

    ofFloatColor myColor = ofFloatColor(0.0, 1.0, 0.0, 1.0);// Green
    shader.setUniform4f("inputColor", myColor);

    triangle.draw();
    shader.end();
    ofPopStyle();
    ofPopMatrix();
}

of3dPrimitive GeometryScene::createTriangle()
{
    ofMesh triangle;
    triangle.setMode(OF_PRIMITIVE_TRIANGLES);
    auto p1 = ofPoint(0.5, 0, -0.5);
    auto p2 = ofPoint(0, 0.5, -0.5);
    auto p3 = ofPoint(-0.5, 0, -0.5);
    triangle.addVertex(p1);
    triangle.addVertex(p2);
    triangle.addVertex(p3);
    return of3dPrimitive{triangle};
}

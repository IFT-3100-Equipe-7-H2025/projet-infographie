
#include "GeometryScene.h"
#include "matrix_clip_space.hpp"
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

    ofMatrix4x4 projMatrix;
    projMatrix.makePerspectiveMatrix(90, (double) ofGetWidth() / (double) ofGetHeight(), 10, 1000);

    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255, 0, 0);
    shader.begin();
    shader.setUniformMatrix4f("projectionMatrix", projMatrix);
    shader.setUniformMatrix4f("modelViewMatrix", ofGetCurrentMatrix(OF_MATRIX_MODELVIEW));

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
    auto p1 = ofPoint(500, 0, -500);
    auto p2 = ofPoint(0, 500, -500);
    auto p3 = ofPoint(-500, 0, -200);
    triangle.addVertex(p1);
    triangle.addVertex(p2);
    triangle.addVertex(p3);
    return of3dPrimitive{triangle};
}


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
    camera_speed = 1000;
    camera.lookAt(ofVec3f(0, 0, 0));
    shader.load("shader.vert", "shader.frag");
}

void GeometryScene::update() {
    time_current = ofGetElapsedTimef();
    time_elapsed = time_current - time_last;
    time_last = time_current;


    speed_translation = camera_speed * time_elapsed;

    // déplacement sur le plan XZ en fonction de l'état des flèches du clavier
    if (is_key_press_w) {
        //camera.boom(speed_translation);
        ofLog() << "boom boom";
        offset_y += speed_translation;
    }
    if (is_key_press_s) {
        //camera.boom(-speed_translation);
        ofLog() << "boom down";
        offset_y -= speed_translation;
    }
    if (is_key_press_a) {
        ofLog() << "left";
        //camera.truck(-speed_translation);
        offset_x -= speed_translation;
    }
    if (is_key_press_d) {
        ofLog() << "right";
        //camera.truck(speed_translation);
        offset_x += speed_translation;
    }
}

void GeometryScene::draw()
{
    ofColor bgColor(backgroundColor[0] * 255, backgroundColor[1] * 255, backgroundColor[2] * 255, backgroundColor[3] * 255);
    ofClear(bgColor);
    auto triangle = createTriangle();

    triangle.setScale(1000, 1000, 1000);

    ofMatrix4x4 projMatrix;
    projMatrix.makePerspectiveMatrix(90, (double) ofGetWidth() / (double) ofGetHeight(), 10, 1000);

    camera.begin();
    
    camera.setPosition(ofVec3f(offset_x, offset_y, 1000));
    ofMatrix4x4 viewMatrix = camera.getModelViewMatrix();

    ofPushMatrix();

    ofPushStyle();

    //ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    ofSetColor(255, 0, 0);
    shader.begin();
    /*shader.setUniformMatrix4f("projectionMatrix", projMatrix);
    shader.setUniformMatrix4f("modelViewMatrix", viewMatrix);*/

    ofFloatColor myColor = ofFloatColor(0.0, 1.0, 0.0, 1.0);// Green
    shader.setUniform4f("inputColor", myColor);

    triangle.draw();
    shader.end();
    ofPopStyle();
    ofPopMatrix();
    camera.end();
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

void GeometryScene::keyPressed(int key)
{
    ofLog() << "<GeometryScene::keyPressed: " << key << ">";
    switch (key)
    {
        case 119://w
            is_key_press_w = true;
            break;

        case 100://d
            is_key_press_d = true;
            break;

        case 115://s
            is_key_press_s = true;
            break;

        case 97://a
            is_key_press_a = true;
            break;
    }
}

void GeometryScene::keyReleased(int key)
{
    ofLog() << "<GeometryScene::keyReleased: " << key << ">";
    switch (key)
    {
        case 119://w
            is_key_press_w = false;
            break;

        case 100://d
            is_key_press_d = false;
            break;

        case 115://s
            is_key_press_s = false;
            break;

        case 97://a
            is_key_press_a = false;
            break;
    }
}

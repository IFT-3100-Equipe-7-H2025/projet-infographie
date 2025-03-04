
#include "GeometryScene.h"
#include "ofGraphics.h"
#include "ofGraphicsConstants.h"
#include "ofShader.h"
#include <cmath>
#include <numbers>
#include <ofMesh.h>
#include <ofxAssimpModelLoader.h>

ofShader shader;
void GeometryScene::setup()
{
    for (int i = 0; i < 6; i++) {
        ofCamera new_cam;
        //new_cam.setPosition(0, 500, 500);
        new_cam.lookAt(ofVec3f(0,0,0));
        cameras.push_back(new_cam);
    }

    camera = &cameras[0];
    current_cam = 0;


    translate_speed = 1000;
    rotate_speed = 100;
    camera->lookAt(ofVec3f(0, 0, 0));
    shader.load("shader.vert", "shader.frag");
}

void GeometryScene::update() {
    time_current = ofGetElapsedTimef();
    time_elapsed = time_current - time_last;
    time_last = time_current;


    speed_translation = translate_speed * time_elapsed;
    speed_rotation = rotate_speed * time_elapsed;

    // déplacement sur le plan XZ en fonction de l'état des flèches du clavier
    if (is_key_press_w) {
        camera->boom(speed_translation);
        ofLog() << "boom boom";
        //offset_y += speed_translation;
    }
    if (is_key_press_s) {
        camera->boom(-speed_translation);
        ofLog() << "boom down";
        //offset_y -= speed_translation;
    }
    if (is_key_press_a) {
        ofLog() << "left";
        camera->truck(-speed_translation);
        //offset_x -= speed_translation;
    }
    if (is_key_press_d) {
        ofLog() << "right";
        camera->truck(speed_translation);
        //offset_x += speed_translation;
    }
    if (is_key_press_up)
    {
        camera->tilt(speed_rotation);
        //angle_y += speed_rotation;
    }
    if (is_key_press_down)
    {
        //angle_y -= speed_rotation;
        camera->tilt(-speed_rotation);
    }
    if (is_key_press_left)
    {
        angle_x += speed_rotation;
        camera->pan(speed_rotation);
    }
    if (is_key_press_right)
    {
        angle_x -= speed_rotation;
        camera->pan(-speed_rotation);
    }
    if (is_key_press_q)
    {
        camera->dolly(speed_translation);
    }
    if (is_key_press_e)
    {
        camera->dolly(-speed_translation);

    }
    if (is_key_press_f)
    {
        camera->setFov(camera->getFov() + 0.2);
    }
    if (is_key_press_g)
    {
        camera->setFov(camera->getFov() - 0.2);
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

    camera->begin();
    
    //camera->setPosition(ofVec3f(offset_x, offset_y, 1000));
    //camera->setOrientation(ofVec3f(angle_x, angle_y, 0));
    ofMatrix4x4 viewMatrix = camera->getModelViewMatrix();

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

    for (int i = 0; i < objects.size();i++)
    {
        ofPushStyle();
        objects[i]->drawFaces();
        ofPopStyle();
    }

    shader.end();
    ofPopStyle();
    ofPopMatrix();
    camera->end();
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


void GeometryScene::reset()
{
    camera->resetTransform();
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
        case 57357://up
            is_key_press_up = true;
            break;
        case 57358://right
            is_key_press_right = true;
            break;
        case 57359://down
            is_key_press_down = true;
            break;
        case 57356://left
            is_key_press_left = true;
            break;
        case 113://q
            is_key_press_q = true;
            break;
        case 101://e
            is_key_press_e = true;
            break;
        case 102://f
            is_key_press_f = true;
            break;
        case 103://g
            is_key_press_g = true;
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
        case 57357://up
            is_key_press_up = false;
            break;
        case 57358://right
            is_key_press_right = false;
            break;
        case 57359://down
            is_key_press_down = false;
            break;
        case 57356://left
            is_key_press_left = false;
            break;
        //q
        case 113:
            is_key_press_q = false;
            break;
        //e
        case 101:
            is_key_press_e = false;
            break;

        //f
        case 102:
            is_key_press_f = false;
            break;
        case 103:
            is_key_press_g = false;
            break;
        //r
        case 114:
            reset();
            break;
        //-
        case 45:
            previousCam();
            break;
        case 61:
            nextCam();
            break;
    }


    

}

void GeometryScene::nextCam() {
    current_cam = ++current_cam < cameras.size() ? current_cam : 0;
    camera = &cameras[current_cam];
}

void GeometryScene::previousCam() {
    current_cam = --current_cam >= 0 ? current_cam : cameras.size() - 1;
    camera = &cameras[current_cam];
}

void GeometryScene::dragEvent(ofDragInfo dragInfo)
{

    if (!dragInfo.files.empty())
    {
        auto model = std::make_shared<ofxAssimpModelLoader>();
        std::string filePath = dragInfo.files[0];// Get the first dropped file
        if (ofFilePath::getFileExt(filePath) == "obj")
        {
            model->loadModel(filePath);// Load the OBJ model
            model->setPosition(0, 0, 0);
            objects.push_back(model);
        }
        else
        {
            ofLog() << "Not an OBJ file!";
        }
    }
    ofLog() << "<GeometryScene::dragEvent: >";
    ofLog() << "<GeometryScene::ofDragInfo file[0]: " << dragInfo.files.at(0)
            << " at : " << dragInfo.position << ">";
}

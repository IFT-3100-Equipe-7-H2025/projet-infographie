
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

    primitives.push_back(createTriangle());
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
    /*auto triangle = createTriangle();

    triangle.setScale(1000, 1000, 1000);*/

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
    if (is_selected) {
        selectionMesh.draw();
    }
    for (auto& primitive : primitives) {
        //primitive.setScale(1000, 1000, 1000);
        primitive.draw();
    }

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
    auto p1 = ofPoint(500, 0, -500);
    auto p2 = ofPoint(0, 500, -500);
    auto p3 = ofPoint(-500, 0, -500);
    triangle.addVertex(p1);
    triangle.addVertex(p2);
    triangle.addVertex(p3);
    return of3dPrimitive{triangle};
}


void GeometryScene::reset()
{
    camera->resetTransform();
}

void GeometryScene::mousePressed(int x, int y, int button) {
    ofVec3f rayOrigin = camera->getPosition();
    ofVec3f rayDirection = camera->screenToWorld(ofVec3f(x, y, 0)) - rayOrigin;
    rayDirection.normalize();

    for (int i = 0; i < primitives.size(); i++) {
        ofVec3f minVertex, maxVertex;
        ofMesh mesh = primitives[i].getMesh();


        float minX = 0, maxX = 0;
        float minY = 0, maxY = 0;

        getBoundingBox(mesh, minVertex, maxVertex);


        vector<ofVec3f> corners = {
                camera->worldToScreen(minVertex),
                camera->worldToScreen(ofVec3f(minVertex.x, maxVertex.y, minVertex.z)),
                camera->worldToScreen(ofVec3f(maxVertex.x, maxVertex.y, minVertex.z)),
                camera->worldToScreen(ofVec3f(maxVertex.x, minVertex.y, minVertex.z))};
        minX = corners[0].x, maxX = corners[0].x;
        minY = corners[0].y, maxY = corners[0].y;
            
        for (auto& corner: corners)
        {
            minX = min(minX, corner.x);
            maxX = max(minX, corner.x);
            minY = min(minY, corner.y);
            maxY = max(minY, corner.y);
        }

        ofLog() << "Clicked At x:" << x << " y: " << y;
        ofLog() << "Box at minX" << minX << " maxX : " << maxX << "minY" << minY << " maxY : " << maxY;

        



        selectionMesh = createBox(minVertex, maxVertex);
        is_selected = true;
        ofLog() << "Selected" << is_selected;
        if (minX < x && x < maxX && minY < y && y < maxY)
        {
            ofLog() << "Object : " << i << " selected. ";
            break;
        }
        

        //ofMesh mesh = createTriangle().getMesh();
        




    }


    
}


ofMesh GeometryScene::createBox(ofVec3f minVertex, ofVec3f maxVertex)
{
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


//void GeometryScene::mousePressed(int x, int y, int button)
//{
//    ofVec3f rayOrigin = camera->getPosition();
//    ofVec3f rayDirection = camera->screenToWorld(ofVec3f(x, y, 0)) - rayOrigin;
//    rayDirection.normalize();
//
//    for (int i = 0; i < objects.size(); i++)
//    {
//        ofVec3f minVertex, maxVertex;
//        int meshNumber = objects[i]->getNumMeshes();
//
//
//        float minX = 0, maxX = 0;
//        float minY = 0, maxY = 0;
//        for (int j = 0; j < meshNumber; j++)
//        {
//            ofMesh mesh = objects[i]->getMesh(j);
//
//            getBoundingBox(mesh, minVertex, maxVertex);
//
//
//            vector<ofVec3f> corners = {
//                    camera->worldToScreen(minVertex),
//                    camera->worldToScreen(ofVec3f(minVertex.x, maxVertex.y, minVertex.z)),
//                    camera->worldToScreen(ofVec3f(maxVertex.x, maxVertex.y, minVertex.z)),
//                    camera->worldToScreen(ofVec3f(maxVertex.x, minVertex.y, minVertex.z))};
//            if (j == 0)
//            {
//                minX = corners[0].x, maxX = corners[0].x;
//                minY = corners[0].y, maxY = corners[0].y;
//            }
//
//            for (auto& corner: corners)
//            {
//                minX = min(minX, corner.x);
//                maxX = max(minX, corner.x);
//                minY = min(minY, corner.y);
//                maxY = max(minY, corner.y);
//            }
//        }
//
//        ofLog() << "Clicked At x:" << x << " y: " << y;
//        ofLog() << "Box at minX" << minX << " maxX : " << maxX << "minY" << minY << " maxY : " << maxY;
//        ofLog() << "Num meshes" << meshNumber;
//        ofMesh new_mesh;
//        new_mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
//
//        new_mesh.addVertex(ofPoint(minX, minY, 0));
//        new_mesh.addVertex(ofPoint(minX, maxY, 0));
//        new_mesh.addVertex(ofPoint(maxX, maxY, 0));
//        new_mesh.addVertex(ofPoint(maxX, minY, 0));
//
//        selectionMesh = new_mesh;
//        is_selected = true;
//        ofLog() << "Selected" << is_selected;
//        if (minX < x && x < maxX && minY < y && y < maxY)
//        {
//            ofLog() << "Object : " << i << " selected. ";
//            break;
//        }
//
//
//        //ofMesh mesh = createTriangle().getMesh();
//    }
//}



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



void GeometryScene::getBoundingBox(ofMesh& mesh, ofVec3f& minVertex, ofVec3f& maxVertex)
{
    minVertex = mesh.getVertex(0);
    maxVertex = mesh.getVertex(0);

    vector<glm::vec3> vertices = mesh.getVertices();

    for (int i = 0; i < vertices.size(); i++) {
        ofLog() << "Vertex X: " << vertices[i].x << "Vertex Y: " << vertices[i].y << "Vertex Z: " << vertices[i].z;
        minVertex.x = min(vertices[i].x, minVertex.x);
        maxVertex.x = max(vertices[i].x, maxVertex.x);
        minVertex.y = min(vertices[i].y, minVertex.y);
        maxVertex.y = max(vertices[i].y, maxVertex.y);
        minVertex.z = min(vertices[i].z, minVertex.z);
        maxVertex.z = max(vertices[i].z, maxVertex.z);

    }
}

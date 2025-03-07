
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

    //primitives.push_back(createTriangle());
    of3dPrimitive sphere = PrimitiveCreator::createSphere(30, 30, 100);
    sphere.setScale(10);
    of3dPrimitive pyramid = PrimitiveCreator::createPyramid(3);
    pyramid.setScale(10);
    primitives.push_back(sphere);
    primitives.push_back(pyramid);
    for (int i = 0; i < 6; i++)
    {
        ofCamera new_cam;
        new_cam.lookAt(ofVec3f(0, 0, 0));
        cameras.push_back(new_cam);
    }

    camera = &cameras[0];
    current_cam = 0;


    translate_speed = 100;
    rotate_speed = 50;
    camera->lookAt(ofVec3f(0, 0, 0));
    shader.load("shader.vert", "shader.frag");
}

void GeometryScene::update()
{
    time_current = ofGetElapsedTimef();
    time_elapsed = time_current - time_last;
    time_last = time_current;


    speed_translation = translate_speed * time_elapsed;
    speed_rotation = rotate_speed * time_elapsed;

    // d?placement sur le plan XZ en fonction de l'?tat des fl?ches du clavier
    if (is_key_press_w)
    {
        camera->boom(speed_translation);
        ofLog() << "boom boom";
        //offset_y += speed_translation;
    }
    if (is_key_press_s)
    {
        camera->boom(-speed_translation);
        ofLog() << "boom down";
        //offset_y -= speed_translation;
    }
    if (is_key_press_a)
    {
        ofLog() << "left";
        camera->truck(-speed_translation);
        //offset_x -= speed_translation;
    }
    if (is_key_press_d)
    {
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
    auto triangle = PrimitiveCreator::createTriangle();
    auto lasasgn = PrimitiveCreator::createLasagna(0.5, 4, 40, 2);
    auto sphere = PrimitiveCreator::createSphere(30, 30, 100);
    auto pyramide = PrimitiveCreator::createPyramid(100);
    /*auto triangle = createTriangle();

    pyramide.setPosition(0, 0, -500);
    pyramide.setScale(1000, 1000, 1000);
    triangle.setPosition(0, 0, -500);
    triangle.setScale(1000, 1000, 1000);
    sphere.setPosition(0, 0, -500);
    sphere.setScale(1000, 1000, 1000);
    lasasgn.setPosition(0, 0, -500);
    lasasgn.setScale(6000, 1000, 1000);
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
    if (is_selected)
    {
        selectionMesh.draw();
    }
    for (auto& primitive: primitives)
    {
        if (debugger)
        {
            primitive.setScale(1000, 1000, 1000);
        }
        primitive.draw();
    }
    //triangle.draw();
    //sphere.drawWireframe();
    //lasasgn.draw();
    //pyramide.drawWireframe();

    for (int i = 0; i < objects.size(); i++)
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


void GeometryScene::reset()
{
    camera->resetTransform();
}

void GeometryScene::mouseDragged(int x, int y, int button)
{
    switch (button)
    {
        case 0://left
            if (is_selected)
            {
                ofVec3f cameraForward = camera->getLookAtDir();
                ofVec3f cameraRight = camera->getSideDir();
                ofVec3f cameraUp = camera->getUpDir();

                ofVec3f current_pos = primitives[selected_primitive].getPosition();
                ofVec3f current_screen_pos = camera->worldToScreen(current_pos);

                ofVec3f current = ofVec3f(x, y, current_screen_pos.z);
                ofVec3f past = ofVec3f(previous_x, previous_y, current_screen_pos.z);

                ofVec3f new_pos = camera->screenToWorld(current_screen_pos + (current - past));


                ofVec3f translate = new_pos - current_pos;

                primitives[selected_primitive].setPosition(new_pos);
                //translateMesh(translate, selected_primitive);
                //primitives[selected_primitive].setPosition(new_pos);
            }

            break;
        case 2://right
            if (is_selected)
            {
                ofVec3f cameraForward = camera->getLookAtDir();
                ofVec3f cameraRight = camera->getSideDir();
                ofVec3f cameraUp = camera->getUpDir();

                ofVec3f current_pos = primitives[selected_primitive].getPosition();
                ofVec3f current_screen_pos = camera->worldToScreen(current_pos);

                ofVec3f current = ofVec3f(x, y, current_screen_pos.z);
                ofVec3f past = ofVec3f(previous_x, previous_y, current_screen_pos.z);

                ofVec3f new_pos = camera->screenToWorld(current_screen_pos + (current - past));


                float scale = 1 + ((x - previous_x) + (previous_y - y)) * 0.1;
                ofLog() << scale;
                ofVec3f scaleVec = primitives[selected_primitive].getScale() * scale;
                primitives[selected_primitive].setScale(scaleVec);
            }
    }

    previous_x = x;
    previous_y = y;
}

void GeometryScene::mousePressed(int x, int y, int button)
{
    previous_x = x;
    previous_y = y;

    ofVec3f rayOrigin = camera->getPosition();
    ofVec3f rayDirection = camera->screenToWorld(ofVec3f(x, y, 0)) - rayOrigin;
    rayDirection.normalize();
    is_selected = false;
    float closeness = -1;
    for (int i = 0; i < primitives.size(); i++)
    {
        ofVec3f minVertex, maxVertex;
        ofMesh mesh = primitives[i].getMesh();


        float minX = 0, maxX = 0;
        float minY = 0, maxY = 0;

        getBoundingBox(primitives[i], minVertex, maxVertex);

        vector<ofVec3f> corners = {
                camera->worldToScreen(maxVertex),
                camera->worldToScreen(ofVec3f(minVertex.x, maxVertex.y, maxVertex.z)),
                camera->worldToScreen(ofVec3f(minVertex.x, minVertex.y, maxVertex.z)),
                camera->worldToScreen(ofVec3f(maxVertex.x, minVertex.y, maxVertex.z)),

                camera->worldToScreen(minVertex),
                camera->worldToScreen(ofVec3f(minVertex.x, maxVertex.y, minVertex.z)),
                camera->worldToScreen(ofVec3f(minVertex.x, minVertex.y, minVertex.z)),
                camera->worldToScreen(ofVec3f(maxVertex.x, minVertex.y, minVertex.z)),
        };
        ofVec3f camPos = camera->getPosition();
        ofVec3f objPos = primitives[i].getPosition();
        float new_closeness = camPos.distance(objPos);

        minX = corners[0].x, maxX = corners[0].x;
        minY = corners[0].y, maxY = corners[0].y;

        for (auto& corner: corners)
        {
            minX = std::min(minX, corner.x);
            maxX = std::max(maxX, corner.x);
            minY = std::min(minY, corner.y);
            maxY = std::max(maxY, corner.y);
        }

        ofLog() << "Clicked At x:" << x << " y: " << y;
        ofLog() << "Box at minX" << minX << " maxX : " << maxX << "minY" << minY << " maxY : " << maxY;

        if (minX < x && x < maxX && minY < y && y < maxY)
        {
            if (new_closeness < closeness || closeness == -1)
            {
                closeness = new_closeness;
                selectionMesh = createBox(primitives[i]);
                selected_primitive = i;
                is_selected = true;
                ofLog() << "Object : " << i << " selected. ";
            }
        }
    }
}


ofMesh GeometryScene::createBox(of3dPrimitive& primitive)
{
    ofVec3f minVertex, maxVertex;

    getBoundingBox(primitive, minVertex, maxVertex);


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

void GeometryScene::nextCam()
{
    current_cam = ++current_cam < cameras.size() ? current_cam : 0;
    camera = &cameras[current_cam];
}

void GeometryScene::previousCam()
{
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
            ofMesh combinedMesh;
            for (int i = 0; i < model->getNumMeshes(); i++)
            {
                ofMesh mesh = model->getMesh(i);
                combinedMesh.append(mesh);
            }
            of3dPrimitive import = of3dPrimitive(combinedMesh);
            primitives.push_back(import);
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

void GeometryScene::getBoundingBox(of3dPrimitive& primitive, ofVec3f& minVertex, ofVec3f& maxVertex)
{
    vector<ofVec3f> vertices = getPrimitiveVertices(primitive);

    minVertex = vertices[0];
    maxVertex = vertices[0];

    for (int i = 0; i < vertices.size(); i++)
    {
        ofLog() << "Vertex X: " << vertices[i].x << "Vertex Y: " << vertices[i].y << "Vertex Z: " << vertices[i].z;
        minVertex.x = std::min(vertices[i].x, minVertex.x);
        maxVertex.x = std::max(vertices[i].x, maxVertex.x);
        minVertex.y = std::min(vertices[i].y, minVertex.y);
        maxVertex.y = std::max(vertices[i].y, maxVertex.y);
        minVertex.z = std::min(vertices[i].z, minVertex.z);
        maxVertex.z = std::max(vertices[i].z, maxVertex.z);
    }
}

vector<ofVec3f> GeometryScene::getPrimitiveVertices(of3dPrimitive& primitive)
{
    ofMesh mesh = primitive.getMesh();
    ofMatrix4x4 transform = primitive.getLocalTransformMatrix();

    vector<ofVec3f> transformedVertices;
    for (auto& vertex: mesh.getVertices())
    {
        ofVec4f transformedVertex = transform.preMult(ofVec4f(vertex.x, vertex.y, vertex.z, 1.0f));
        transformedVertices.push_back(ofVec3f(transformedVertex.x, transformedVertex.y, transformedVertex.z));
    }
    return transformedVertices;
}


void GeometryScene::focus()
{
    if (is_selected)
    {
        camera->lookAt(primitives[selected_primitive].getPosition());
    }
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
        case 113://q
            is_key_press_q = false;
            break;
        case 101://e
            is_key_press_e = false;
            break;
        case 102://f
            is_key_press_f = false;
            break;
        case 103:
            is_key_press_g = false;
            break;
        case 114://r
            reset();
            break;
        case 45://-
            previousCam();
            break;
        case 61:
            nextCam();
            break;
        case 107://k
            debugger = !debugger;
            break;
        case 'y':
            focus();
            break;
    }
}

#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "component.h"
#include "mesh.h"
#include "vertex.h"
#include "vec2.h"
#include "vec3.h"
#include "gameobject.h"
#include "transform.h"
#include "triangle.h"
#include "meshrenderer.h"

struct Triangles{
    Vec3 normal;
    Vec3 pointInTriangle;

    Triangles(Vec3 _normal, Vec3 _pointInTriangle)
    {
        normal = _normal;
        pointInTriangle = _pointInTriangle;
    }
};

// class for handling physics on a object, only sphere to plane at the moment
// sphere sometimes falls through plane, which i believe is because it not always detects collition correctly
// not sure how to fix that yet //
class PhysicsObject : public Component
{
private:
    float friction = 0.025f;
    void initComponent();
    void startComponent(){;}
    void stopComponent(){;}
    bool renderBoundingBox;
    void simulateGravity();

    Vec3 gravity; // gravity for calculating valocity
    Vec3 velocity; // velocity for movement

    Mesh *mMesh; // mesh of colliding gameobject
    std::vector<GLushort> indices;
    Transform *mCollidingGameObject; // object to collide with (plane)
    std::vector<GameObject*> trianglesHit; // holds triangles (mesh) which is hit with ball on plane, to visualize them
    std::vector<Vertex> vertices;

public slots:
    //void on_comboBox_currentIndexChanged(int index){;}

public:
    PhysicsObject(Mesh *mesh, Transform* collidingGameObject);
    ~PhysicsObject();

    Vec3 getVelocity(){return velocity;}
    void setVelocity(Vec3 inn){ velocity = inn;}

    void runComponent();
    //void visualizeComponentWidget(QWidget* componentWidget){;}

    // creates triangles which sphere is close to and about to hit
    void CreateTrianglesToVizualise(float distanceFromFinalPoint, Vec3 p1, Vec3 p2, Vec3 p3, int i);
    //draw all triangles in trianglesHit vector
    void DrawTriangles();
    // deletes all triangles and cleans up
    void cleanUp();
};

#endif // PHYSICSOBJECT_H

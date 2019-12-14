#include "physicsobject.h"

void PhysicsObject::initComponent()
{
    type = Orf::PHYSICSOBJECT;
    gravity = Vec3(0.0f, -0.20f, 0.0f);
    vertices = mMesh->getVertices();
    indices = mMesh->getIndices();
}

PhysicsObject::PhysicsObject(Mesh *mesh, Transform *collidingGameObject)
{
    mMesh = mesh;
    mCollidingGameObject = collidingGameObject;
}

PhysicsObject::~PhysicsObject()
{

}
#include "plane.h"
void PhysicsObject::runComponent()
{
    std::vector<Vec3> point; // vertex positions
    std::vector<Triangles> collidedTriangles;
    Transform *transform = attachedGameobject->mTransform;
    float distanceFromPoint = 99.0f;

    cleanUp();

    foreach (Vertex V, vertices){
        point.push_back(V.get_xyz()); // gets position values from Vertices
    }

    for (unsigned int i = 0; i < indices.size()-4; i++){ // checks each triangle in plane for collition

        // get vertices based on indices from plane
        Vec3 p1 = point[indices[i]] + mCollidingGameObject->getPosition();
        Vec3 p2 = point[indices[i+1]] + mCollidingGameObject->getPosition();
        Vec3 p3 = point[indices[i+2]] + mCollidingGameObject->getPosition();

        if((p1.x == p2.x && p1.y == p2.y && p1.z == p2.z) || (p2.x == p3.x && p2.y == p3.y && p2.z == p3.z))
            continue; // since we make plane with tringlestrip, we get overlapping vertices when
                      // connection strips, we ignore them or else we get weird triangles where collition always is true

        Vec3 pointInTriangle; // point in triangle when calculating barysentricCoords
        // calculating barysentric coords, return true if point in triangle is inside triangle
        bool collition = transform->getPosition().calcBarysentricCoords(p1, p2, p3, pointInTriangle);

        if(collition){ // if point in triangle is inside the triangle
            // normal on plane triangle, every other triangle drawn with trianglestrip is inverse order,
            // so we need to revert them to get correct normal
            qDebug() << "collition!";
            Vec3 triangleNormal;
            if(i % 2 == 0){
                triangleNormal = (p2-p1)^(p3-p1);
            } else{
                triangleNormal = (p1-p2)^(p3-p2);
            }
            triangleNormal.normalize();

            // sphere distance from point in triangle
            distanceFromPoint = (pointInTriangle - transform->getPosition()).length();
            qDebug() << distanceFromPoint;
            if(distanceFromPoint < 1){ // saves triangle if sphere is close enough
                collidedTriangles.push_back(Triangles(triangleNormal, pointInTriangle));

                //gravity = Vec3(0,-0.05,0); // testing with different gravity on collition
            }    
            CreateTrianglesToVizualise(distanceFromPoint, p1, p2, p3, i);
        }
        DrawTriangles();
    }


    if(collidedTriangles.size() > 0){

        //  reflection vector for ball, based on all colliding triangles
        Vec3 reflectionNormal;
        Vec3 collitonPoint;

        // adds all normals and pointInTriangles, then normalizes them
        for(unsigned int i = 0; i < collidedTriangles.size(); i++){
            reflectionNormal = reflectionNormal + collidedTriangles[i].normal;
            collitonPoint = collitonPoint + collidedTriangles[i].pointInTriangle;
        }
        reflectionNormal.normalize();
        collitonPoint = collitonPoint * (1.0/collidedTriangles.size());

        // calculation velocity based on normal
        float vn = velocity * reflectionNormal;
        Vec3 velocityReflection = velocity - (reflectionNormal * (2*vn));
        float gravityLenght = gravity.length();
        velocity = ((velocityReflection.normalized() * (velocity.length() * (1.0f-friction))) + (gravity + (reflectionNormal * gravityLenght)) * 0.05f - (gravity * 0.05f));
        //velocity = velocityReflection.normalized() ;
    }

    simulateGravity();

}

void PhysicsObject::CreateTrianglesToVizualise(float distanceFromFinalPoint, Vec3 p1, Vec3 p2, Vec3 p3, int i)
{

    Vec3 upOffset = Vec3(0.0f, 0.05f, 0.0f);

    Vec3 rgb = Vec3(std::min(1.0f, 1.0f/std::pow(distanceFromFinalPoint,2.0f)), std::min(1.0f, 1.0f/std::pow(distanceFromFinalPoint,2.0f)));
        // draws triangles, two methods depending on order of indices since it is trianglestrip
    if(i % 2 == 0)
    {
        GameObject *tempGameObject = new GameObject(0.0f, 0.0f, 0.0f, "test");
        Mesh *tempMesh = new Triangle(Vertex(p1 + upOffset, rgb, Vec2(0,0)),
                                      Vertex(p2 + upOffset, rgb, Vec2(0,0)),
                                      Vertex(p3 + upOffset, rgb, Vec2(0,0)));
        MeshRenderer *tempMeshRenderer = new MeshRenderer(tempMesh);
        Material *material = new Material(0);
        tempMeshRenderer->setMaterial(material);
        tempGameObject->addComponent(tempMeshRenderer);
        trianglesHit.push_back(tempGameObject);
    } else
    {
        GameObject *tempGameObject = new GameObject(0.0f, 0.0f, 0.0f, "test");
        Mesh *tempMesh = new Triangle(Vertex(p2 + upOffset, rgb, Vec2(0,0)),
                                      Vertex(p1 + upOffset, rgb, Vec2(0,0)),
                                      Vertex(p3 + upOffset, rgb, Vec2(0,0)));
        MeshRenderer *tempMeshRenderer = new MeshRenderer(tempMesh);
        Material *material = new Material(0);
        tempMeshRenderer->setMaterial(material);
        tempGameObject->addComponent(tempMeshRenderer);
        trianglesHit.push_back(tempGameObject);
    }

}

void PhysicsObject::DrawTriangles()
{
    //qDebug() << trianglesHit.size();
    if(trianglesHit.size() > 0){
        for(unsigned int i = 0; i < trianglesHit.size(); i++){
            //trianglesHit[i]->setViewMatrix(attachedGameobject->getViewMatrix());
            //trianglesHit[i]->setprojectionMatrix(attachedGameobject->getProjectionMatrix());
            trianglesHit[i]->runComponents();
        }
    }
}

void PhysicsObject::cleanUp()
{
    if(trianglesHit.size() > 0)
        {
            for(unsigned int i = 0; i < trianglesHit.size(); i++)
            {
                delete trianglesHit.at(i);
            }
            trianglesHit.clear();
        }
}


void PhysicsObject::simulateGravity()
{
    // resetting position and velocity if object is below colliding object
    if(attachedGameobject->mTransform->getPosition().y < -10){
        attachedGameobject->mTransform->setPosition(0.5, 7.5, 0.5);
        velocity = Vec3(0,0,0);
    }

    velocity = velocity + gravity * 0.05f;
    attachedGameobject->mTransform->translate(velocity);
}

#include "beziercurve.h"
#include <cmath>
#include "vertex.h"
#include "vec3.h"
#include "vec2.h"

BezierCurve::BezierCurve()
{
    type = Orf::BEZIER;
    initGeometry();
}

void BezierCurve::drawGeometry(GLint positionattribute, GLint normalAttribute, GLint textureAttribute, int renderMode)
{
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

    SetAttributePointers(positionattribute, normalAttribute, textureAttribute, renderMode);

    //glPointSize((GLfloat)10.0f);
    glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
    //GL_POINT_SIZE

    //Unbind buffers:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Unbind shader
    glUseProgram(0);
}
#include <cstdlib>
#include <ctime>
void BezierCurve::initGeometry()
{

    int curveCount = 1000; // amount of connected beziercurves

    std::srand((int)std::time(0));

    for(int m = 0; m < 4 * curveCount; m++){
        controlPoints.push_back(Vec3((std::rand() % 11) -5, (std::rand() % 11) -5, (std::rand() % 11) -5));
    }

    int lineSteps = 100; // amount of lines
    Vec3 nextPointsOffset;

    for(int x = 0; x < curveCount; x++)
    {

        for(int i = 0; i < lineSteps; i++){
            float t = 1.0f/(float)lineSteps * i;
            Vec3 b = EvaluateBezier(3, t, x);
            mVertices.push_back(Vertex(Vec3(b.x, b.y, b.z), Vec3(0, 1, 1), Vec2(0, 0)));
        }

        // preping points on next curve
        int nextCurveIndex  = (x+1) * 4;
        // offset from first point in curve to last point
        nextPointsOffset = nextPointsOffset + (controlPoints[nextCurveIndex-1] - controlPoints[nextCurveIndex-4]);
        // controlpoint 3 to 4 vector for alligning second point of curve for continous curve
        Vec3 P2_P3 = controlPoints[nextCurveIndex-1] - controlPoints[nextCurveIndex-2];

        //P2_P3 = P2_P3.normalized() * (controlPoints[point-1] - controlPoints[point]);

        if(x != curveCount-1){ // not done on the last curve
            controlPoints[nextCurveIndex] = (controlPoints[nextCurveIndex-1]); // set first point in curve to the last of previus curve
            controlPoints[nextCurveIndex+1] = (controlPoints[nextCurveIndex] + P2_P3); //force allignement
            controlPoints[nextCurveIndex+2] = (controlPoints[nextCurveIndex+2] + nextPointsOffset); // add offset
            controlPoints[nextCurveIndex+3] = (controlPoints[nextCurveIndex+3] + nextPointsOffset);
        }
    }
    // we must add the last point
    mVertices.push_back(Vertex(controlPoints[controlPoints.size()-1], Vec3(0, 1, 1), Vec2(0, 0)));

    mBoundingBoxmesh = new BoundingBoxMesh();
    mBoundingBoxmesh->getBoundingBox()->calulateBoundingBox(mVertices);

    initializeOpenGLFunctions();

    // Transfer vertex data to VBO 0
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);
}

Vec3 BezierCurve::EvaluateBezier(int degree, float t, int curveNum)
{
    Vec3 c[4];
    for (unsigned int i = 0; i < 4; i++){
        c[i] = controlPoints[i + (curveNum*4)];
    }

    for(int k = 1; k <= degree; k++){
        for(int i = 0; i < degree - k + 1; i++){
            c[i] = c[i] * (1-t) + c[i+1] * t;
        }
    }
    return c[0];
}


//for(int x = 0; x < curveCount; x++)
//{
//    for (float i = 0; i <= 1.0f; i += 1.0f/(float)lineSteps)
//    {
//        Vec3 temp = (controlPoints[0] * ((float)pow(1 - i, 3))) +
//                    (controlPoints[1] * (3 * (float)pow(1 - i, 2) * i)) +
//                    (controlPoints[2] * (3 * (1 - i) * (float)pow(i, 2))) +
//                    (controlPoints[3] * (float)pow(i, 3));
//
//        //qDebug() << "x: " << temp.x << " y: " << temp.y << " z: " << temp.z << "    " << i;
//        mVertices.push_back(Vertex(temp, Vec3(1.0f, 0.0f + i, 1.0f - i), Vec2(0, 0)));
//    }
//    // because of inaccuracies (rounding error) in float, we have to add the last point manually
//    mVertices.push_back(Vertex(controlPoints[3], Vec3(1, 0, 1), Vec2(0, 0)));
//
//    // vector from startPoint to endPoint in curve
//    Vec3 nextPointsOffset = (controlPoints[3] - controlPoints[0]);
//
//    // points are offset to be repeated
//    controlPoints[0] = (controlPoints[3]);
//    controlPoints[1] = (controlPoints[1] + nextPointsOffset);
//    controlPoints[2] = (controlPoints[2] + nextPointsOffset);
//    controlPoints[3] = (controlPoints[3] + nextPointsOffset);
//}

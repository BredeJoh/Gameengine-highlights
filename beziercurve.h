#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H
#include "mesh.h"
#include "material.h"


class BezierCurve : public Mesh
{
public:
    BezierCurve();
    void drawGeometry(GLint positionattribute, GLint normalAttribute, GLint textureAttribute, int renderMode = 0) override;
    void initGeometry() override;
private:
    GLuint mVertexBuffer;
    GLuint mIndexBuffer;

    std::vector<Vec3> controlPoints;
    Vec3 EvaluateBezier(int degree, float t, int curveNum);
};

#endif // BEZIERCURVE_H

#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <iostream>
#include "vec3.h"
#include "vec2.h"
#include "vertex.h"

// reads obj files and returns its data for meshes
class OBJLoader
{
public:
    OBJLoader();
    ~OBJLoader();
    bool loadOBJ (const char *path,
                 std::vector<Vec3> &out_vertices,
                 std::vector<Vec3> &out_normals,
                 std::vector<Vec2> &out_uvs);
    bool loadSOSI(QString filePath, std::vector<Vertex> &vertices, std::vector<GLushort> &indices);
    bool loadOBJ(QString filePath,
                 std::vector<Vertex> &out_vertices,
                 std::vector<GLushort> &indices);
};

#endif // OBJLOADER_H

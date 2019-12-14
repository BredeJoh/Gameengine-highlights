#include "objloader.h"
#include <fstream>
#include <ostream>
#include <iostream>
#include <sstream>
#include "meshrenderer.h"
#include "scene.h"
#include "QString"
#include <QFile>
#include <QTextStream>
#include <QDebug>

OBJLoader::OBJLoader()
{

}

OBJLoader::~OBJLoader()
{

}

bool OBJLoader::loadOBJ(const char *path, std::vector<Vec3> &out_vertices, std::vector<Vec3> &out_normals, std::vector<Vec2> &out_uvs)
{

    std::vector< Vec3 > temp_vertices;
    std::vector< Vec2 > temp_uvs;
    std::vector< Vec3 > temp_normals;



    FILE * file = fopen(path, "r");
    if(file == NULL){
        printf("Impossible to open this file !\n");
        return false;
    }


    while(1){
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if(res == EOF)
            break;

        if(strcmp(lineHeader, "v") == 0){ // reads position data
            Vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if(strcmp(lineHeader, "vt") == 0){ // reads uv data
            Vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if( strcmp(lineHeader, "vn") == 0){ // reads normal data
            Vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if(strcmp(lineHeader, "f") == 0){ // reads indicies
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                    &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if(matches != 9){
                printf("File can't be read by parser");
                return false;
            }

            std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);

            for (unsigned int i=0; i < vertexIndices.size(); i++){
                unsigned int vertexIndex = vertexIndices[i];
                Vec3 vertex = temp_vertices[vertexIndex-1];
                out_vertices.push_back(vertex);

            }
            for (unsigned int i=0; i < uvIndices.size(); i++){
                unsigned int uvIndex = uvIndices[i];
                Vec2 uv = temp_uvs[uvIndex-1];
                out_uvs.push_back(uv);
            }
            for (unsigned int i=0; i < normalIndices.size(); i++){
                unsigned int normalIndex = normalIndices[i];
                Vec3 normal = temp_normals[normalIndex-1];
                out_normals.push_back(normal);
            }
        }
    }

    return true;

}

#include <ctime>

bool OBJLoader::loadSOSI(QString filePath, std::vector<Vertex> &vertices, std::vector<GLushort> &indices)
{

    QFile mFile(filePath);
    if(!mFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open SOSI file";
        return false;
    }

    //loading

    QTextStream in(&mFile);
    std::string text;

    Vec2 *subtract{nullptr};
    Vec3 divide(10000, 100, 10000);

    clock_t startTime = clock();

    Vec3 color = Vec3(1.0f, 1.0f, 0.0f); //gul

    while(!in.atEnd()){
        std::stringstream stream;
        text = in.readLine().toStdString();
        stream << text;
        std::string lineHeader;
        stream >> lineHeader;

        if(lineHeader == ".PUNKT"){
            float y;

            while (true){
                std::stringstream pointStream;
                text = in.readLine().toStdString();
                pointStream << text;
                lineHeader = "";
                pointStream >> lineHeader;

                if(lineHeader == "..HØYDE"){
                    pointStream >> y;
                    y = y/divide.y;
                }
                else if(lineHeader == "..NØ"){
                    text = in.readLine().toStdString();
                    std::stringstream points;
                    points << text;

                    float x,z;
                    points >> x >> z;

                    if(subtract == nullptr){
                        subtract = new Vec2(x,z);
                    }

                    x = (x - subtract->x)/divide.x;
                    z = (z - subtract->y)/divide.z;

                    vertices.push_back(Vertex(Vec3(x,y,z), Vec3(color), Vec2(0,0)));
                    indices.push_back(indices.size());

                    break;
                }
            }
        }
        else if (lineHeader == ".KURVE"){
            float y = 0;

            while(lineHeader != ".SLUTT"){
                std::stringstream pointStream;
                text = in.readLine().toStdString();
                pointStream << text;
                lineHeader = "";
                pointStream >> lineHeader;

                if(lineHeader == "..HØYDE"){
                    pointStream >> y;
                    int hoyde = y;
                    if(hoyde % 100 == 0)
                        color = Vec3(1.0f, 0.0f, 1.0f);
                    else
                        color = Vec3(1.0f, 1.0f, 0.0f);
                    y = y/divide.y;

                }
                else if(lineHeader == "..NØ"){
                    while(true){
                        text = in.readLine().toStdString();
                        std::stringstream points;
                        points << text;
                        lineHeader = "";
                        points >> lineHeader;

                        if(lineHeader == "..NØ"){
                            continue;
                        }
                        else if(lineHeader == ".KURVE" || lineHeader == ".SLUTT"){
                            break;
                        }

                        float x,z;
                        x =  std::stof(lineHeader);
                        points >> z;
                        if(subtract == nullptr){
                            subtract = new Vec2(x,z);
                        }

                        x = (x - subtract->x)/divide.x;
                        z = (z - subtract->y)/divide.z;

                        vertices.push_back(Vertex(Vec3(x, y, z), Vec3(color), Vec2(0,0)));
                        indices.push_back(indices.size());
                    }
                }
            }
        }
        else {
            continue;
        }

    }

    qDebug() << vertices.size() << " points was made";
    double duration = (std::clock() - startTime) / (double) CLOCKS_PER_SEC;
    qDebug() << duration;
    return true;
}

bool OBJLoader::loadOBJ(QString filePath, std::vector<Vertex> &out_vertices, std::vector<GLushort> &indices)
{
    std::vector< Vec3 > temp_vertices;
    std::vector< Vec2 > temp_uvs;
    std::vector< Vec3 > temp_normals;

    QFile mFile(filePath);
    if(!mFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file: " << filePath;
        //return;
    }

    QTextStream in(&mFile);
    std::string text;

    int temp_index{0};

    while(!in.atEnd())
    {
        std::stringstream sStream;
        text = in.readLine().toStdString();
        sStream << text;
        std::string lineHeader;
        sStream >> lineHeader;

        if(lineHeader == std::string("v")){
            Vec3 vertex;
            sStream >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if(lineHeader == std::string("vt")){
            Vec2 uv;
            sStream >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if(lineHeader == std::string("vn")){
            Vec3 normal;
            sStream >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if(lineHeader == std::string("f")){
            char dummy1, dummy2;
            int index, normal, uv;
            for(int i = 0; i < 3; i++){
                sStream >> index >> dummy1 >> uv >> dummy2 >> normal;

                index -= 1;
                normal -= 1;
                uv -= 1;

                Vertex tempVert (temp_vertices[index], temp_normals[normal], temp_uvs[uv]);
                out_vertices.push_back(tempVert);

                indices.push_back(temp_index++);
            }
        }
        else
            continue;
    }

    mFile.close();
    return true;
}





























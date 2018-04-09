#ifndef MESH_H
#define MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include <GLES3/gl3.h>

using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
//    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
//    glm::vec3 Tangent;
//    // bitangent
//    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int vao;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
         vector<Texture> textures);
    void Draw(GLuint shaderID);

private:
    GLuint vbo, ibo;

    void setupMesh();
};

#endif // MESH_H

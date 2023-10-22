#ifndef MESH_H
#define MESH_H

#include <string>
#include "Shader.hpp"
#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess);
        void draw(Shader &shader);
    
    private:
        unsigned int _VAO, _VBO, _EBO;
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::vector<Texture> _textures;
        glm::vec3 _diffuse, _specular;
        float _shininess;
        void setupMesh();


};

#endif // MESH_H
#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : _vertices(vertices), _indices(indices), _textures(textures)
{
    setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,
            glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess)
    : _vertices(vertices), _indices(indices), _textures(textures), _diffuse(diffuseColor), _specular(specularColor), _shininess(shininess)
{
    setupMesh();
}

/**
 * @brief Create vertex array object, bind vertex data, 
 * normals, texture coords to VBO
 * 
 */
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
  
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), 
                 _indices.data(), GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // vertex tangents + bitangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
}

void Mesh::draw(Shader& shader)
{

    auto is_diffuse = [](Texture t) { return t.type == "texture_diffuse"; };
    auto is_specular = [](Texture t) { return t.type == "texture_specular"; };
    auto is_normal = [](Texture t) { return t.type == "texture_normal"; };

    if (std::find_if(_textures.begin(), _textures.end(), is_diffuse) == _textures.end()) {
        shader.setBool("useDiffuseColor", true);
        shader.setVec3("material.diffuse", _diffuse);
    } else {
        shader.setBool("useDiffuseColor", false);
    }

    if (std::find_if(_textures.begin(), _textures.end(), is_specular) == _textures.end()) {
        shader.setBool("useSpecularColor", true);
        shader.setVec3("material.specular", _specular);
    } else {
        shader.setBool("useSpecularColor", true);
    }

    if (std::find_if(_textures.begin(), _textures.end(), is_normal) == _textures.end()) {
        shader.setBool("useNormalMap", false);
    } else {
        shader.setBool("useNormalMap", true);
    }

    shader.setFloat("material.shininess", _shininess);

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;

    for(unsigned int i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = _textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, _textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
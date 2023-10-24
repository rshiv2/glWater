#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Mesh.hpp"

class Model 
{
    public:
        Model(const char *path)
        {
            loadModel(path);
        }

        void setModelMat(const glm::mat4& m) {
            _model = m;
        }

        glm::vec3 centroid() const {
            return _centroid;
        }

        void draw(Shader &shader);	
    private:
        // model data
        std::vector<Mesh> _meshes;
        std::string _directory;

        // helper functions for loading model via assimp
        void loadModel(const std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             std::string typeName);
        unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = false);

        // transforms
        glm::mat4 _model;
        glm::vec3 _centroid;    // compute on construction
        int _numVertices = 0;
};

#endif // MODEL_H
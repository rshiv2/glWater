#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "Shader.hpp"
#include "Model.hpp"

// too small to make into a class
struct DirLight
{
    glm::vec3 _direction;
    glm::vec3 _ambient;
    glm::vec3 _diffuse;
    glm::vec3 _specular;
};

static const float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

class PointLight
{
    public:

        PointLight(const std::string& path); 

        glm::vec3 position() const {
            return _position + _translation;
        }
        glm::vec3 ambient() const {
            return _ambient;
        }

        glm::vec3 diffuse() const {
            return _diffuse;
        }

        glm::vec3 specular() const {
            return _specular;
        }

        float kConstant() const {
            return _kConstant;
        }

        float kLinear() const {
            return _kLinear;
        }

        float kQuadratic() const {
            return _kQuadratic;
        }

        void translate(const glm::vec3& t) {
            _translation += t;
        }

        void setAmbient(const glm::vec3& color) {
            _ambient = color;
        }

        void setDiffuse(const glm::vec3& color) {
            _diffuse = color;
        }

        void setSpecular(const glm::vec3& color) {
            _specular = color;
        }

        void setConstant(const float k) {
            _kConstant = k;
        }

        void setLinear(const float k) {
            _kLinear = k;
        }

        void setQuadratic(const float k) {
            _kQuadratic = k;
        }

        void scale(const glm::vec3 s) {
            _scale *= s;
        }

        void draw(Shader* shader);

        void setShaderUniforms(Shader* shader);
        

    private:

        GLuint _VAO, _VBO;
        glm::mat4 _modelMat;
        Model* _model;

        glm::vec3 _ambient, _diffuse, _specular;
        glm::vec3 _position;
        float _kConstant = 1.0f, _kLinear = 0.7f, _kQuadratic = 1.8;    // arbitrary values

        glm::vec3 _translation = glm::vec3(0.0f);
        glm::vec3 _scale = glm::vec3(1.0f);
};

PointLight::PointLight(const std::string& path) { 
    _position = glm::vec3(0.0f);
    _model = new Model(path.c_str());
    _modelMat = glm::mat4(1.0f);
}

void PointLight::draw(Shader* shader)
{
    setShaderUniforms(shader);
    _model->draw(*shader);
}

void PointLight::setShaderUniforms(Shader* shader)
{

    shader->setVec3("color", _diffuse);
    
    glm::mat4 translation = glm::translate(_modelMat, _translation);
    glm::mat4 scale = glm::scale(_modelMat, _scale);
    glm::mat4 modelMat = translation * scale;

    shader->setMat4("model", modelMat);
    glm::mat4 invTransposeModelMat = glm::inverseTranspose(modelMat);
    shader->setMat4("invTransposeModel", invTransposeModelMat);
}


#endif // LIGHT_H
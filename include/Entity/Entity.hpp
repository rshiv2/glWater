#ifndef ENTITY_H
#define ENTITY_H

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "Model.hpp"
#include "Shader.hpp"

class Entity
{
    public:

        Entity(const std::string& path) {
            _model = new Model(path.c_str());
        }

        void translate(glm::vec3 t) {
            _translation += t;
        }

        void rotateX(float degrees) {
            _rotation.x += degrees;
        }

        void rotateY(float degrees) {
            _rotation.y += degrees;
        }

        void rotateZ(float degrees) {
            _rotation.z += degrees;
        }

        void scale(glm::vec3 s) {
            _scale *= s;
        }

        void draw(Shader* shader) {
            setShaderUniforms(shader);
            _model->draw(*shader);
        }

        void setShaderUniforms(Shader* shader);
        void setTexCoordScale(float f) {
            _texCoordScale = f;
        }

    private:
        Model* _model;

        glm::vec3 _translation = glm::vec3(0.0f);
        glm::vec3 _rotation = glm::vec3(0.0f);
        glm::vec3 _scale = glm::vec3(1.0f);

        //glm::mat4 _modelMat;
        float _texCoordScale = 1.0f;

};

void Entity::setShaderUniforms(Shader* shader)
{

    glm::mat4 translation = glm::translate(_translation);

    const glm::vec3 xAxis(1.0f, 0.0f, 0.0f);
    const glm::vec3 yAxis(0.0f, 1.0f, 0.0f);
    const glm::vec3 zAxis(0.0f, 0.0f, 1.0f);

    glm::mat4 rotX = glm::rotate(glm::radians(_rotation.x), xAxis);
    glm::mat4 rotY = glm::rotate(glm::radians(_rotation.y), yAxis);
    glm::mat4 rotZ = glm::rotate(glm::radians(_rotation.z), zAxis);

    glm::mat4 rotation = rotZ * rotY * rotX;
    glm::mat4 scale = glm::scale(_scale);
    glm::mat4 modelMat = translation * rotation * scale;
    glm::mat4 invTransposeModelMat = glm::inverseTranspose(modelMat);

    shader->setMat4("model", modelMat);
    shader->setMat4("invTransposeModel", invTransposeModelMat);
    shader->setFloat("texCoordScale", _texCoordScale);
}

#endif 
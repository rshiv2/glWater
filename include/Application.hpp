#ifndef APP_H
#define APP_H 

#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Scene.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

/******** GLFW callbacks ******/
// need to give glfw free functions as callbacks
// can access application data using glfwGetWindowUserPointer
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xPos, double yPos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/******** Application class **********/

class Application
{
    public:

        Application(unsigned int viewportWidth, unsigned int viewportHeight);
        ~Application();

        bool ready() const;
        void run();

        GLFWwindow* window() {
            return _window;
        }

        void attachScene(Scene& scene);
        void attachCamera(Camera& camera);
        
        inline void framebufferSizeCallback(int width, int height);
        inline void mouseCallback(double xPos, double yPos);
        inline void scrollCallback(double xoffset, double yoffset);

    private:
        GLFWwindow* glfwSetup();
        void processInput(GLFWwindow* window);
        void renderScene(const Camera* cam);
        void renderReflection();

    private:

        unsigned int _viewportWidth;
        unsigned int _viewportHeight;
        GLFWwindow* _window;
        Scene* _scene;
        Camera* _camera;

        float _deltaTime;   // time since previous frame
        float _prevFrame;   // time of previous frame
        float _prevMouseX, _prevMouseY; // position of mouse at previous frame
        bool _firstMouse;   // is first time we are reading mouse position

        Shader* _entityShader;
        Shader* _lightSourceShader;
        Shader* _skyBoxShader;
        Shader* _waterShader;
};

Application::Application(unsigned int viewportWidth, unsigned int viewportHeight)
    : _viewportWidth(viewportWidth), _viewportHeight(viewportHeight)
{
    _window = glfwSetup();
    _scene = nullptr;
    _camera = nullptr;

    _deltaTime = 0.0f;
    _prevFrame = 0.0f;
    _firstMouse = true;

    _entityShader      = new Shader("../include/Entity/shader.vert", "../include/Entity/shader.frag");
    _lightSourceShader = new Shader("../include/LightSource/shader.vert", "../include/LightSource/shader.frag");
    _skyBoxShader      = new Shader("../include/Skybox/shader.vert", "../include/Skybox/shader.frag");
    _waterShader       = new Shader("../include/Water/shader.vert", "../include/Water/shader.frag");
}

Application::~Application()
{
    delete _entityShader;
    delete _lightSourceShader;
    delete _skyBoxShader;
    delete _waterShader;
    glfwTerminate();
}

bool Application::ready() const 
{
    if (!_scene || !_camera) {
        std::cout << "APPLICATION::ERROR: Could not find attached scene or camera" << std::endl;
        return false;
    }

    if (!_entityShader || !_lightSourceShader || !_skyBoxShader || !_waterShader) {
        std::cout << "APPLICATION::ERROR: Failed to compile one or more shaders" << std::endl;
        return false;
    }

    return true;
}

void Application::run()
{
    if (!ready())
        return;

    while(!glfwWindowShouldClose(_window)) {
        processInput(_window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene(_camera);

        if (!_scene->waters.empty()) {
            renderReflection();
        }

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

GLFWwindow* Application::glfwSetup()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(_viewportWidth, _viewportHeight, "glWater", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }

    // for accessing Application data in callback functions
    glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

    // set framebuffer size and window callbacks
    int fWidth, fHeight;
    glfwGetFramebufferSize(window, &fWidth, &fHeight);
    glViewport(0,0,fWidth,fHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glViewport(0, 0, fWidth, fHeight);
    glEnable(GL_MULTISAMPLE);

    return window;
}

void Application::attachScene(Scene& scene) 
{
    _scene = &scene;

    glUniform1f(glGetUniformLocation(_entityShader->ID, "material.shininess"), 32.0f);
    
    /** Set uniforms that do not change throughout the scene **/
    _entityShader->use();

    for (int i = 0; i < _scene->dirLights.size(); i++) {
        std::string name = std::string("dirLights[") + std::to_string(i) + std::string("]");
        DirLight dirLight = _scene->dirLights[i];
        _entityShader->setVec3(name + std::string(".direction"), dirLight._direction);
        _entityShader->setVec3(name + std::string(".ambient"), dirLight._ambient);
        _entityShader->setVec3(name + std::string(".diffuse"), dirLight._diffuse);
        _entityShader->setVec3(name + std::string(".specular"), dirLight._specular);
    }

    for (int i = 0; i < _scene->pointLights.size(); i++) {
        
        std::string name = std::string("pointLights[") + std::to_string(i) + std::string("]");       
        PointLight& pl = _scene->pointLights[i];
        _entityShader->setVec3(name + ".ambient", pl.ambient());
        _entityShader->setVec3(name + ".diffuse", pl.diffuse());
        _entityShader->setVec3(name + ".specular", pl.specular());
        _entityShader->setVec3(name + ".position", pl.position());
        _entityShader->setFloat(name + ".kConstant", pl.kConstant()); 
        _entityShader->setFloat(name + ".kLinear", pl.kLinear());
        _entityShader->setFloat(name + ".kQuadratic", pl.kQuadratic());
    }
}

void Application::attachCamera(Camera& camera)
{
    _camera = &camera;
}

inline void Application::framebufferSizeCallback(int width, int height) 
{ 
    _viewportWidth = width;
    _viewportHeight = height;
}

inline void Application::mouseCallback(double xPos, double yPos) 
{

    if (_firstMouse) {
        _prevMouseX = xPos;
        _prevMouseY = yPos;
        _firstMouse = false;
    }

    float xOffset = xPos - _prevMouseX;
    float yOffset = _prevMouseY - yPos;
    _prevMouseX = xPos;
    _prevMouseY = yPos;

    _camera->mouse_callback(xOffset, yOffset);
}

inline void Application::scrollCallback(double xoffset, double yoffset) 
{
    _camera->scroll_callback(xoffset, yoffset);
}

void Application::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float currentFrame = glfwGetTime();
    _deltaTime = currentFrame - _prevFrame;
    _prevFrame = currentFrame;
    _camera->scaleSpeed(_deltaTime * 60.0f);
    _camera->process_input(window);
}

void Application::renderScene(const Camera* cam)
{
    glEnable(GL_DEPTH_TEST);

    glm::mat4 view = cam->lookAt(); 
    glm::mat4 projection = glm::perspective(glm::radians(cam->getFov()), (float)(_viewportWidth) / _viewportHeight, 0.1f, 100.0f); 

    // render light source
    _lightSourceShader->use();
    _lightSourceShader->setMat4("view", view);
    _lightSourceShader->setMat4("projection", projection); 

    for (auto& pls : _scene->pointLights) {
        pls.draw(_lightSourceShader);
    }

    // render entities
    _entityShader->use();
    _entityShader->setMat4("view", view);
    _entityShader->setMat4("projection", projection);
    _entityShader->setInt("numDirLights", _scene->dirLights.size());
    _entityShader->setInt("numPointLights", _scene->pointLights.size());
    
    for (auto& entity : _scene->entities) {
        entity.draw(_entityShader);
    }

    // render skybox if it exists
    if (_scene->skyBox != nullptr) {
        _skyBoxShader->use();
        glm::mat4 viewWithoutTranslation = glm::mat4(glm::mat3(view));
        _skyBoxShader->setMat4("view", viewWithoutTranslation);
        _skyBoxShader->setMat4("projection", projection);
        _scene->skyBox->draw(_skyBoxShader); 
    }
}

void Application::renderReflection()
{

    for (auto& water : _scene->waters) {
        
        glm::vec4 plane = water.getPlaneEquation();
        glm::vec4 planeFlipped = -plane;
        planeFlipped.w = plane.w;

        _lightSourceShader->use();
        _lightSourceShader->setVec4("reflectionClippingPlane", plane); 
        _entityShader->use();
        _entityShader->setVec4("reflectionClippingPlane", plane); 

        // render reflection
        auto waterFBO = water.getWaterFrameBuffer();
        waterFBO->bindReflectionFrameBuffer();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Camera camReflected = _camera->reflect(plane);

        glEnable(GL_CLIP_DISTANCE0);
        renderScene(&camReflected);
        waterFBO->unbindReflectionFrameBuffer(_window);
        glDisable(GL_CLIP_DISTANCE0);
        
        // render refraction
        _lightSourceShader->use();
        _lightSourceShader->setVec4("reflectionClippingPlane", planeFlipped); 

        _entityShader->use();
        _entityShader->setVec4("reflectionClippingPlane", planeFlipped); 
        waterFBO->bindRefractionFrameBuffer();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CLIP_DISTANCE0);
        renderScene(_camera);
        waterFBO->unbindRefractionFrameBuffer(_window);
        glDisable(GL_CLIP_DISTANCE0);
        
        // draw water
        water.draw(_waterShader, _camera, _viewportWidth, _viewportHeight); 
    }
}


/** GLFW Callback definitions **/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferSizeCallback(width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->mouseCallback(xPos, yPos); 
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->scrollCallback(xoffset, yoffset);
}
#endif // APP_H 
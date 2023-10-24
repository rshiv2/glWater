#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Application.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Water/Water.hpp"
#include "Water/WaterFrameBuffer.hpp"
#include "Skybox/Skybox.hpp"
#include "Entity/Entity.hpp"
#include "LightSource/LightSource.hpp"
#include "Scene.hpp"

Scene loadBoatScene(GLFWwindow* window)
{
    Scene scene;

    // create light sources
    std::string pointLightPath = "../res/lantern/light.obj";
    PointLight pl(pointLightPath); 
    glm::vec3 warmLight = glm::vec3(250.0f, 152.0f, 32.0f) / 255.0f;
    pl.setAmbient(warmLight * .05f);
    pl.setDiffuse(warmLight * 0.8f);
    pl.setSpecular(warmLight);
    pl.translate(glm::vec3(0.0, 0.09f, -0.05f));
    pl.scale(glm::vec3(0.001f));
    scene.pointLights.push_back(pl);

    // create directional light
    DirLight dirLight;
    dirLight._direction = glm::vec3(0.0f, -1.0f, -0.2f);
    dirLight._ambient = glm::vec3(0.05f);
    dirLight._diffuse = glm::vec3(0.4f);
    dirLight._specular = glm::vec3(0.5f);
    scene.dirLights.push_back(dirLight);

    // lantern
    char lanternPath[] = "../res/lantern/OBJ.obj";
    Entity lantern(lanternPath);
    lantern.translate(glm::vec3(0.0, 0.09f, -0.05f));
    lantern.scale(glm::vec3(0.001f));
    scene.entities.push_back(lantern);

    // boat
    char boatPath[] = "../res/oldboat/OldBoat.blend";
    Entity boat(boatPath);
    boat.translate(glm::vec3(0.0f, 0.05f, 0.05f));
    boat.scale(glm::vec3(0.02f));
    boat.rotateX(-90.0f);
    scene.entities.push_back(boat);

    // sand
    char sandPath[] = "../res/island/sand/quad.obj";
    Entity sand(sandPath);
    sand.scale(glm::vec3(3.0f));
    sand.rotateX(92.5f);
    sand.translate(glm::vec3(0.0f, 0.05f, -2.0f));
    sand.setTexCoordScale(20.0f);
    scene.entities.push_back(sand);

    // create skybox
    const std::string prefix = "../res/night_skybox/";
    std::vector<std::string> faces = {
        "px.png",
        "nx.png",
        "py.png",
        "ny.png",
        "pz.png",
        "nz.png"
    };

    for (auto& f : faces)
        f = prefix + f;
    scene.skyBox = new Skybox(faces);

    // create water
    glm::vec3 center(0.0f, 0.0f, 0.0f), dx(100.0f, 0.0f, 0.0f), dy(0.0f, 0.0f, -100.0f);
    scene.waters.emplace_back(window, center, dx, dy);
    return scene;
}

/*
Scene loadLuxoScene()
{
    Scene scene;

    // load table quad
    char tablePath[] = "../res/quad/quad.obj";
    Entity table(tablePath);
    table.scale(5.0f);
    table.rotateX(90.0f);
    scene.entities.push_back(table);

    // load luxo sr.
    char luxoPath[] = "../res/pixar-lamp/source/pixar.obj";
    Entity luxoSr(luxoPath);
    luxoSr.translate(glm::vec3(0.0f, 2.2f, 0.0f));
    scene.entities.push_back(luxoSr);

    // load luxo jr.
    Entity luxoJr(luxoPath);
    luxoJr.translate(glm::vec3(0.0f, 1.7f, -2.0f));
    luxoJr.scale(glm::vec3(0.7f));
    scene.entities.push_back(luxoJr);

    // create directional light source
    DirLight dirLight;
    dirLight._direction = glm::vec3(0.0f, -1.0f, 0.0f);
    dirLight._ambient = glm::vec3(1.0f);
    dirLight._diffuse = glm::vec3(1.0f);
    dirLight._specular = glm::vec3(1.0f);
    scene.dirLights.push_back(dirLight);

    return scene;
}*/

int main() 
{
    
    Application app(800, 600);
    Camera camera(glm::vec3(0.0f, 0.3f,-2.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    camera.setMoveSensitivity(0.01f);
    Scene scene = loadBoatScene(app.window());

    app.attachScene(scene);
    app.attachCamera(camera);
    
    app.run();

    return 0;
}
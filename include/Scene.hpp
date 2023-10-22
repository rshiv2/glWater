#ifndef SCENE_H
#define SCENE_H

#include "Skybox/Skybox.hpp"
#include "Entity/Entity.hpp"
#include "LightSource/LightSource.hpp"
#include "Water/Water.hpp"

class Scene {
    public:
        ~Scene() {
            delete skyBox;
        }
        Skybox* skyBox;
        std::vector<Entity> entities;
        std::vector<PointLight> pointLights;
        std::vector<DirLight> dirLights;
        std::vector<Water> waters;
};

#endif // SCENE_H
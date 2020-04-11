#include "PhotonMapping.hpp"
#include "RayTracer.hpp"

// Photons per light.
const uint NUM_PHOTONS = 400000;

void EmitPhoton(const SceneNode *root, const Ray &ray, Photon &photon,
                PhotonMap *global, PhotonMap *caustic)
{
}

void MapPhotons(const std::list<Light *> &lights, const SceneNode *root,
                PhotonMap *global, PhotonMap *caustic)
{
    // TODO ADD PROGRESS OUTPUT + MULTITHREAD
    for (auto *light : lights)
    {
        Photon photon;
        photon.power = light->colour / (float)NUM_PHOTONS;

        double phi = 0.0;
        double theta = 0.0;
        for (uint i = 0; i < NUM_PHOTONS; ++i)
        {
            int x = rand();
            int y = rand();
            int z = rand();
            if (x == 0 && y == 0 && z == 0)
            {
                continue; // Lazy!
            }
            glm::vec3 direction = {x, y, z};
            direction = glm::normalize(direction);
            Ray emit = Ray(light->position, direction);
            EmitPhoton(root, emit, photon, global, caustic);
        }
    }
}

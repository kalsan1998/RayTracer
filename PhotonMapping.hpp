#pragma once

#include "SceneNode.hpp"
#include "Light.hpp"

#include <mutex>
#include <thread>
#include <vector>

class Ray;

struct Photon
{
    glm::vec3 power; //rgb
    bool first_surface_hit = false;
    bool caustic = true;
};

template <class T>
class KdTree;

using PhotonMap = KdTree<Photon>;

// https://graphics.stanford.edu/courses/cs348b-00/course8.pdf
// Need to balance KdTree as well.
// Power of emitted photons is light_power/# of emitted photons
class PhotonMapper
{
public:
    PhotonMapper();
    void MapPhotons(const std::list<Light *> &lights, const SceneNode *root);
    const PhotonMap *GlobalMap() const { return global; }
    const PhotonMap *CausticMap() const { return caustic; }

private:
    void EmitPhotonBatch(const Light *light, const SceneNode *root);
    void EmitPhoton(const SceneNode *root, const Ray *ray, Photon &photon);

    void insert_global(Photon &photon, glm::vec3 &point);
    void insert_caustic(Photon &photon, glm::vec3 &point);
    void update_progress(int n);

    PhotonMap *global;
    PhotonMap *caustic;

    std::vector<std::thread> threads;

    std::mutex global_lock;
    std::mutex caustic_lock;

    float progress;
    std::mutex progress_lock;
};
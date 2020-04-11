#include "KdTree.hpp"
#include "SceneNode.hpp"
#include "Light.hpp"

struct Photon
{
    glm::vec3 power; //rgb
    bool first_surface_hit = false;
    bool caustic = true;
};

using PhotonMap = KdTree<Photon>;

// https://graphics.stanford.edu/courses/cs348b-00/course8.pdf
// Need to balance KdTree as well.
// Power of emitted photons is light_power/# of emitted photons
void MapPhotons(const std::list<Light *> &lights, const SceneNode *root,
                PhotonMap *global, PhotonMap *caustic);

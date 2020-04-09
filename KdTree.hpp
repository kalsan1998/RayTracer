#pragma once

#include <glm/glm.hpp>

struct KdNode
{
    glm::vec3 pos;
    KdNode *left;
    KdNode *right;
};

// Essentially a kd-tree
class KdTree
{
    KdTree();
    ~KdTree();
    void insert(KdNode *node)
    {
        InsertNode(node, root, 0);
    }
    // Counts how many nodes have a pos with each dimension within r from p.
    // r is positive
    int count_within_range(int r, const glm::vec3 &p) const
    {
        return CountRange(root, p, r, 0);
    }

private:
    KdNode *InsertNode(KdNode *node, KdNode *root, int d);
    int CountRange(KdNode *root, const glm::vec3 &p, int r, int d) const;
    const int dim = 3;
    KdNode *root;
};
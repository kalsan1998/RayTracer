#include "KdTree.hpp"

#include <iostream>

KdTree::KdTree()
{
}

KdTree::~KdTree()
{
    // Should delete all the nodes but it doesn't matter for a ray tracer since is used once.
}

KdNode *KdTree::InsertNode(KdNode *node, KdNode *t, int d)
{
    if (!node)
    {
        std::cerr << "Attempted to insert null node into tree." << std::endl;
    }
    else if (!t)
    {
        t = new KdNode();
        t->pos = node->pos;
    }
    else if (node->pos[d] < t->pos[d])
    {
        t->left = InsertNode(node, t->left, (d + 1) % dim);
    }
    else
    {
        t->right = InsertNode(node, t->right, (d + 1) % dim);
    }
    return t;
}

int KdTree::CountRange(KdNode *t, const glm::vec3 &p, int r, int d) const
{
    if (!t)
    {
        return 0;
    }
    else if (t->pos[d] < p[d] - r)
    {
        return CountRange(t->right, p, r, (d + 1) % dim);
    }
    else if (t->pos[d] > p[d] + r)
    {
        return CountRange(t->left, p, r, (d + 1) % dim);
    }
    else if (t->pos[0] < p[0] + r && t->pos[0] > p[0] - r &&
             t->pos[1] < p[1] + r && t->pos[0] > p[1] - r &&
             t->pos[2] < p[2] + r && t->pos[2] > p[2] - r)
    {
        return 1 + CountRange(t->right, p, r, (d + 1) % dim) + CountRange(t->left, p, r, (d + 1) % dim);
    }
    return CountRange(t->right, p, r, (d + 1) % dim) + CountRange(t->left, p, r, (d + 1) % dim);
}
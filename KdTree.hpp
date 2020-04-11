#pragma once

#include <glm/glm.hpp>
#include <iostream>

template <class T>
struct KdNode
{
    glm::vec3 pos;
    KdNode<T> *left;
    KdNode<T> *right;
    T *data;
};

template <class T>
class KdTree
{
public:
    KdTree();
    ~KdTree();
    void insert(KdNode<T> *node)
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
    KdNode<T> *InsertNode(KdNode<T> *node, KdNode<T> *root, int d);
    int CountRange(KdNode<T> *root, const glm::vec3 &p, int r, int d) const;

    const int dim = 3;
    KdNode<T> *root;
};

template <class T>
KdTree<T>::KdTree()
{
}

template <class T>
KdTree<T>::~KdTree()
{
    // Should delete all the nodes but it doesn't matter for a ray tracer since is used once.
}

template <class T>
KdNode<T> *KdTree<T>::InsertNode(KdNode<T> *node, KdNode<T> *t, int d)
{
    if (!node)
    {
        std::cerr << "Attempted to insert null node into tree." << std::endl;
    }
    else if (!t)
    {
        t = new KdNode<T>();
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

template <class T>
int KdTree<T>::CountRange(KdNode<T> *t, const glm::vec3 &p, int r, int d) const
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
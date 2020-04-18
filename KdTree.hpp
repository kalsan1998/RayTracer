#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

template <class T>
struct KdNode
{
    glm::vec3 pos;
    T data;
    KdNode<T> *left;
    KdNode<T> *right;
};

template <class T>
class KdTree
{
public:
    KdTree();
    ~KdTree();
    void insert(const T &data, glm::vec3 pos)
    {
        KdNode<T> *node = new KdNode<T>();
        node->pos = pos;
        node->data = data;
        InsertNode(node, root, 0);
    }
    // Gets the nodes that have a pos with each dimension within r from p.
    // r is positive
    std::vector<T *> get_within_range(float r, const glm::vec3 &p) const
    {
        std::vector<T *> v = std::vector<T *>();
        GetRange(root, p, r, 0, v);
        return v;
    }
    uint size() const
    {
        return s;
    }

private:
    KdNode<T> *InsertNode(KdNode<T> *node, KdNode<T> *&root, int d);
    void GetRange(KdNode<T> *root, const glm::vec3 &p, float r,
                  int d, std::vector<T *> &v) const;

    const int dim = 3;
    uint s = 0;
    KdNode<T> *root = nullptr;
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
KdNode<T> *KdTree<T>::InsertNode(KdNode<T> *node, KdNode<T> *&t, int d)
{
    if (!node)
    {
        std::cerr << "Attempted to insert null node into tree." << std::endl;
    }
    else if (!t)
    {
        t = node;
        ++s;
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
void KdTree<T>::GetRange(KdNode<T> *t, const glm::vec3 &p, float r,
                         int d, std::vector<T *> &v) const
{
    if (!t)
    {
        return;
    }
    else if (t->pos[d] < p[d] - r)
    {
        GetRange(t->right, p, r, (d + 1) % dim, v);
    }
    else if (t->pos[d] > p[d] + r)
    {
        GetRange(t->left, p, r, (d + 1) % dim, v);
    }
    else if (t->pos[0] < p[0] + r && t->pos[0] > p[0] - r &&
             t->pos[1] < p[1] + r && t->pos[1] > p[1] - r &&
             t->pos[2] < p[2] + r && t->pos[2] > p[2] - r)
    {
        v.push_back(&t->data);
        GetRange(t->right, p, r, (d + 1) % dim, v);
        GetRange(t->left, p, r, (d + 1) % dim, v);
    }
    else
    {
        GetRange(t->right, p, r, (d + 1) % dim, v);
        GetRange(t->left, p, r, (d + 1) % dim, v);
    }
}
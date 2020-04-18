#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <queue>

template <class T>
struct KdNode
{
    glm::vec3 pos;
    T data;
    KdNode<T> *left;
    KdNode<T> *right;
};

template <class T>
struct Compare
{
    bool operator()(std::pair<T *, float> a, std::pair<T *, float> b)
    {
        return a.second < b.second;
    }
};

template <class T>
struct Heap
{
    Heap(uint k) : q(), max_size(k) {}

    std::priority_queue<std::pair<T *, float>, std::vector<std::pair<T *, float>>, Compare<T>> q;
    const uint max_size;

    float Front() const
    {
        if (q.size() < max_size)
        {
            return std::numeric_limits<float>::infinity();
        }
        else
        {
            return q.top().second;
        }
    }

    bool TryInsert(KdNode<T> *t, float dist)
    {
        if (q.size() + 1 > max_size)
        {
            if (Front() > dist)
            {
                q.pop();
                q.emplace(&t->data, dist);
                return true;
            }
        }
        else
        {
            q.emplace(&t->data, dist);
            return true;
        }
        return false;
    }

    std::vector<T *> GetValues()
    {
        std::vector<T *> v;
        while (q.size())
        {
            v.push_back(q.top().first);
            q.pop();
        }
        return v;
    }
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

    std::vector<T *> k_nearest(int k, const glm::vec3 &ref, float &max_distance) const
    {
        Heap<T> heap(k);
        Nearest(root, heap, ref, 0);
        max_distance = heap.Front();
        return heap.GetValues();
    }

    uint size() const
    {
        return s;
    }

private:
    KdNode<T> *InsertNode(KdNode<T> *node, KdNode<T> *&root, int d);
    void GetRange(KdNode<T> *root, const glm::vec3 &p, float r,
                  int d, std::vector<T *> &v) const;
    void Nearest(KdNode<T> *t, Heap<T> &heap, const glm::vec3 &ref, int d) const;

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

// http://andrewd.ces.clemson.edu/courses/cpsc805/references/nearest_search.pdf
template <class T>
void KdTree<T>::Nearest(KdNode<T> *t, Heap<T> &heap,
                        const glm::vec3 &ref, int d) const
{
    if (!t)
    {
        return;
    }
    float dist = heap.Front();
    float new_dist = glm::distance(ref, t->pos);
    if (heap.TryInsert(t, new_dist))
    {
        dist = heap.Front();
    }
    if (ref[d] < t->pos[d])
    {
        if (ref[d] - dist <= t->pos[d])
        {
            Nearest(t->left, heap, ref, (d + 1) % dim);
        }
        if (ref[d] + dist > t->pos[d])
        {
            Nearest(t->right, heap, ref, (d + 1) % dim);
        }
    }
    else
    {
        if (ref[d] + dist > t->pos[d])
        {
            Nearest(t->right, heap, ref, (d + 1) % dim);
        }
        if (ref[d] - dist <= t->pos[d])
        {
            Nearest(t->left, heap, ref, (d + 1) % dim);
        }
    }
}
// Winter 2020

#include "Primitive.hpp"
#include "polyroots.hpp"

double kEpsilon = 0.0005;
Primitive::~Primitive()
{
}

Sphere::Sphere()
    : internal({0.0, 0.0, 0.0}, 1.0) {}

Sphere::~Sphere()
{
}

int Sphere::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    return internal.Intersection(ray, t_vals, normal);
}

Cube::Cube()
    : internal({0.0, 0.0, 0.0}, 1.0) {}

Cube::~Cube()
{
}

int Cube::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    return internal.Intersection(ray, t_vals, normal);
}

Cone::Cone()
    : internal({0.0, 0.0, 0.0}, 1.0) {}

Cone::~Cone()
{
}

int Cone::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    return internal.Intersection(ray, t_vals, normal);
}

NonhierSphere::~NonhierSphere()
{
}

int NonhierSphere::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    glm::vec3 diff = ray.A - m_pos;
    glm::vec3 diff_2 = diff * diff;
    double A = ray.B_A_2.x / a_2 + ray.B_A_2.y / b_2 + ray.B_A_2.z / c_2;
    double B = 2.0 * ((ray.B_A.x * diff.x / a_2) + (ray.B_A.y * diff.y / b_2) + (ray.B_A.z * diff.z / c_2));
    double C = diff_2.x / a_2 + diff_2.y / b_2 + diff_2.z / c_2 - 1.0;
    quadraticRoots(A, B, C, t_vals);
    int roots = 0;
    if (t_vals[0] > 0.0)
    {
        ++roots;
    }
    if (t_vals[1] > 0.0)
    {
        ++roots;
    }
    glm::vec3 p = ray.GetPoint(std::min(std::abs(t_vals[0]), std::abs(t_vals[1])));
    normal = 2.0f * (p - m_pos) / glm::vec3(a_2, b_2, c_2);
    return roots;
};

NonhierBox::~NonhierBox()
{
}

int NonhierBox::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    glm::vec3 t0 = (m_pos - ray.A) / ray.B_A;
    glm::vec3 t1 = (m_pos + glm::vec3(m_size, m_size, m_size) - ray.A) / ray.B_A;
    double t_min_x = std::min(t0.x, t1.x);
    double t_min_y = std::min(t0.y, t1.y);
    double t_min_z = std::min(t0.z, t1.z);
    double t_max_x = std::max(t0.x, t1.x);
    double t_max_y = std::max(t0.y, t1.y);
    double t_max_z = std::max(t0.z, t1.z);

    if (ray.B_A.x == 0)
    {
        t_min_x = 0.0001;
        t_max_x = std::numeric_limits<double>::infinity();
    }
    if (ray.B_A.y == 0)
    {
        t_min_y = 0.0001;
        t_max_y = std::numeric_limits<double>::infinity();
    }
    if (ray.B_A.z == 0)
    {
        t_min_z = 0.0001;
        t_max_z = std::numeric_limits<double>::infinity();
    }
    if (t_min_x > t_max_y || t_min_y > t_max_x)
    {
        return 0;
    }
    double t_max = std::min(t_max_x, t_max_y);
    double t_min = std::max(t_min_x, t_min_y);
    if (t_min_z > t_max || t_max_z < t_min)
    {
        return 0;
    }

    t_min = std::max(t_min, t_min_z);
    t_max = std::min(t_max, t_max_z);
    t_vals[0] = t_min;
    t_vals[1] = t_max;

    glm::vec3 p = ray.GetPoint(t_min);
    normal = {0, 0, 0};
    if (std::abs(p.y - m_pos.y) < kEpsilon)
    {
        normal.y--;
    }
    else if (std::abs(p.y - m_pos.y - m_size) < kEpsilon)
    {
        normal.y++;
    }
    if (std::abs(p.x - m_pos.x) < kEpsilon)
    {
        normal.x--;
    }
    else if (std::abs(p.x - m_pos.x - m_size) < kEpsilon)
    {
        normal.x++;
    }
    if (std::abs(p.z - m_pos.z) < kEpsilon)
    {
        normal.z--;
    }
    else if (std::abs(p.z - m_pos.z - m_size) < kEpsilon)
    {
        normal.z++;
    }

    return 2;
}

NonhierCone::~NonhierCone()
{
}

int NonhierCone::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    glm::vec3 diff = ray.A - m_pos;
    glm::vec3 diff_2 = diff * diff;
    double A = ray.B_A_2.x / size_2 - ray.B_A_2.y / size_2 + ray.B_A_2.z / size_2;
    double B = 2.0 * ((ray.B_A.x * diff.x / size_2) - (ray.B_A.y * diff.y / size_2) + (ray.B_A.z * diff.z / size_2));
    double C = diff_2.x / size_2 - diff_2.y / size_2 + diff_2.z / size_2;
    quadraticRoots(A, B, C, t_vals);
    int roots = 0;
    glm::vec3 p;
    glm::vec3 p1;
    glm::vec3 p2;
    if (t_vals[0] > 0.0)
    {
        p1 = ray.GetPoint(t_vals[0]);
        if (p1.y < m_pos.y && std::abs(p1.y - m_pos.y) <= m_size)
        {
            p = p1;
            ++roots;
        }
    }
    if (t_vals[1] > 0.0)
    {
        p2 = ray.GetPoint(t_vals[1]);
        if (p2.y < m_pos.y && std::abs(p2.y - m_pos.y) <= m_size)
        {
            ++roots;
            if (t_vals[1] < t_vals[0] || roots == 0)
                p = p2;
        }
    }
    normal = 2.0f * (p - m_pos) / (float)size_2;
    return roots;
    // glm::vec3 co = ray.A - m_pos;
    // double A = (ray.B_A_2.y * size_2) - cos_angle_2;
    // double B = 2. * ((ray.B_A.y * co.y * size_2) - (cos_angle_2 * glm::dot(ray.B_A, co)));
    // double C = (co.y * co.y * size_2) - (glm::dot(co, co) * cos_angle_2);
    // quadraticRoots(A, B, C, t_vals);
    // glm::vec3 p = ray.GetPoint(std::min(std::abs(t_vals[0]), std::abs(t_vals[1])));
    // if (p.y - m_pos.y > 0)
    // {
    //     return 0;
    // }
    // int roots = 0;
    // if (t_vals[0] > 0.0)
    // {
    //     ++roots;
    // }
    // if (t_vals[1] > 0.0)
    // {
    //     ++roots;
    // }
    // normal = 2.0f * (p - m_pos);
    // return roots;
}

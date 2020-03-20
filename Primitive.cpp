// Winter 2020

#include "Primitive.hpp"
#include "polyroots.hpp"

double kEpsilon = 0.0005;
Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

int Sphere::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
{
    glm::vec3 diff = ray.A - m_pos;
    glm::vec3 diff_2 = diff * diff;
    double A = ray.B_A_2.x / radius_2 + ray.B_A_2.y / radius_2 + ray.B_A_2.z / radius_2;
    double B = 2.0 * ((ray.B_A.x * diff.x / radius_2) + (ray.B_A.y * diff.y / radius_2) + (ray.B_A.z * diff.z / radius_2));
    double C = diff_2.x / radius_2 + diff_2.y / radius_2 + diff_2.z / radius_2 - 1.0;
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
    normal = 2.0f * (p - m_pos) / glm::vec3(radius_2, radius_2, radius_2);
    return roots;
};

Box::~Box()
{
}

int Box::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
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
        if (ray.A.x > max_co.x || ray.A.x < m_pos.x)
        {
            return 0;
        }
        t_min_x = 0;
        t_max_x = std::numeric_limits<double>::infinity();
    }
    if (ray.B_A.y == 0)
    {
        if (ray.A.y > max_co.y || ray.A.y < m_pos.y)
        {
            return 0;
        }
        t_min_y = 0;
        t_max_y = std::numeric_limits<double>::infinity();
    }
    if (ray.B_A.z == 0)
    {
        if (ray.A.z > max_co.z || ray.A.z < m_pos.z)
        {
            return 0;
        }
        t_min_z = 0;
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

Cone::~Cone()
{
}

int Cone::Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const
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
        if (p1.y < m_pos.y && p1.y > min_y)
        {
            p = p1;
            ++roots;
        }
    }
    if (t_vals[1] > 0.0)
    {

        p2 = ray.GetPoint(t_vals[1]);
        if (p2.y < m_pos.y && p2.y && p2.y > min_y && (t_vals[1] < t_vals[0] || roots == 0))
        {
            ++roots;
            p = p2;
        }
    }
    normal = 2.0f * (p - m_pos) / (float)size_2;
    normal.y *= -1.0f;
    return roots;
}

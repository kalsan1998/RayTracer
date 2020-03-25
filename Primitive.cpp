// Winter 2020

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <math.h>

const double kEpsilon = 0.0001;

double MinT(double *t_vals)
{
    if (t_vals[0] < kEpsilon && t_vals[1] < kEpsilon)
    {
        return std::numeric_limits<double>::infinity();
    }
    else if (t_vals[0] < kEpsilon)
    {
        return t_vals[1];
    }
    else if (t_vals[1] < kEpsilon)
    {
        return t_vals[0];
    }
    return std::min(t_vals[0], t_vals[1]);
}

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

bool Sphere::RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const
{
    glm::vec3 diff = ray.A - m_pos;
    glm::vec3 diff_2 = diff * diff;
    double A = ray.B_A_2.x / radius_2 + ray.B_A_2.y / radius_2 + ray.B_A_2.z / radius_2;
    double B = 2.0 * ((ray.B_A.x * diff.x / radius_2) + (ray.B_A.y * diff.y / radius_2) + (ray.B_A.z * diff.z / radius_2));
    double C = diff_2.x / radius_2 + diff_2.y / radius_2 + diff_2.z / radius_2 - 1.0;
    double t_vals[2];
    quadraticRoots(A, B, C, t_vals);
    double t = MinT(t_vals);
    if (t >= t_min)
    {
        return false;
    }
    t_min = t;
    point = ray.GetPoint(t);
    diff = (point - m_pos) / (float)radius;
    float u = 0.5 + atan2(diff.x, diff.z) / (2.0 * M_PI);
    float v = 0.5 + asin(diff.y) / M_PI;
    uv = glm::vec2(u, v);
    normal = 2.0f * (point - m_pos) / glm::vec3(radius_2, radius_2, radius_2);
    return true;
};

Box::~Box()
{
}

bool Box::RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const
{
    glm::vec3 t0 = (m_pos - ray.A) / ray.B_A;
    glm::vec3 t1 = (m_pos + glm::vec3(size, size, size) - ray.A) / ray.B_A;
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
            return false;
        }
        t_min_x = 0;
        t_max_x = std::numeric_limits<double>::infinity();
    }
    if (ray.B_A.y == 0)
    {
        if (ray.A.y > max_co.y || ray.A.y < m_pos.y)
        {
            return false;
        }
        t_min_y = 0;
        t_max_y = std::numeric_limits<double>::infinity();
    }
    if (ray.B_A.z == 0)
    {
        if (ray.A.z > max_co.z || ray.A.z < m_pos.z)
        {
            return false;
        }
        t_min_z = 0;
        t_max_z = std::numeric_limits<double>::infinity();
    }
    if (t_min_x > t_max_y || t_min_y > t_max_x)
    {
        return false;
    }
    double t_max = std::min(t_max_x, t_max_y);
    double t = std::max(t_min_x, t_min_y);
    if (t_min_z > t_max || t_max_z < t)
    {
        return false;
    }

    t = std::max(t, t_min_z);
    t_max = std::min(t_max, t_max_z);

    if (t < kEpsilon)
    {
        if (t_max < kEpsilon)
        {
            return false;
        }
        t = t_max;
    }
    if (t >= t_min)
    {
        return false;
    }
    t_min = t;
    point = ray.GetPoint(t);
    normal = {0, 0, 0};
    if (std::abs(point.y - m_pos.y) < kEpsilon)
    {
        normal.y--;
    }
    else if (std::abs(point.y - m_pos.y - size) < kEpsilon)
    {
        normal.y++;
    }
    if (std::abs(point.x - m_pos.x) < kEpsilon)
    {
        normal.x--;
    }
    else if (std::abs(point.x - m_pos.x - size) < kEpsilon)
    {
        normal.x++;
    }
    if (std::abs(point.z - m_pos.z) < kEpsilon)
    {
        normal.z--;
    }
    else if (std::abs(point.z - m_pos.z - size) < kEpsilon)
    {
        normal.z++;
    }
    return true;
}

Cone::~Cone()
{
}

bool Cone::RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const
{
    glm::vec3 diff = ray.A - m_pos;
    glm::vec3 diff_2 = diff * diff;
    double A = ray.B_A_2.x / size_2 - ray.B_A_2.y / size_2 + ray.B_A_2.z / size_2;
    double B = 2.0 * ((ray.B_A.x * diff.x / size_2) - (ray.B_A.y * diff.y / size_2) + (ray.B_A.z * diff.z / size_2));
    double C = diff_2.x / size_2 - diff_2.y / size_2 + diff_2.z / size_2;

    // Don't always want the minimum t here because the cone is infinite.
    double t_vals[2];
    quadraticRoots(A, B, C, t_vals);

    if (MinT(t_vals) >= t_min)
    {
        return false;
    }

    bool found = false;
    if (t_vals[0] > kEpsilon)
    {
        glm::vec3 p1 = ray.GetPoint(t_vals[0]);
        if (p1.y < m_pos.y && p1.y > min_y && t_vals[0] < t_min)
        {
            t_min = t_vals[0];
            point = p1;
            found = true;
        }
    }
    if (t_vals[1] > kEpsilon)
    {
        glm::vec3 p2 = ray.GetPoint(t_vals[1]);
        if (p2.y < m_pos.y && p2.y > min_y)
        {
            if (t_vals[1] < t_min)
            {
                t_min = t_vals[1];
                point = p2;
                found = true;
            }
        }
    }
    normal = 2.0f * (point - m_pos) / (float)size_2;
    normal.y *= -1.0f;
    return found;
}

Cylinder::~Cylinder()
{
}

bool Cylinder::RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const
{
    glm::vec3 diff = ray.A - m_pos;
    glm::vec3 diff_2 = diff * diff;
    double A = ray.B_A_2.x / radius_2 + ray.B_A_2.z / radius_2;
    double B = 2.0 * ((ray.B_A.x * diff.x / radius_2) + (ray.B_A.z * diff.z / radius_2));
    double C = diff_2.x / radius_2 + diff_2.z / radius_2 - 1.0;

    // Don't always want the minimum t here because the cylinder is infinite.
    double t_vals[2];
    quadraticRoots(A, B, C, t_vals);

    if (MinT(t_vals) >= t_min)
    {
        return false;
    }
    bool found = false;
    if (t_vals[0] > kEpsilon)
    {
        glm::vec3 p1 = ray.GetPoint(t_vals[0]);
        if (p1.y < max_y && p1.y > min_y && t_vals[0] < t_min)
        {
            found = true;
            point = p1;
            t_min = t_vals[0];
        }
    }
    if (t_vals[1] > kEpsilon && t_vals[1] < t_min)
    {
        glm::vec3 p2 = ray.GetPoint(t_vals[1]);
        if (p2.y < max_y && p2.y > min_y)
        {
            found = true;
            point = p2;
            t_min = t_vals[1];
        }
    }
    normal = 2.0f * (point - m_pos) / (float)radius_2;
    normal.y *= 0;
    return found;
}

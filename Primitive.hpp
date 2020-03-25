// Winter 2020

#pragma once

#include <glm/glm.hpp>
#include "RayTracer.hpp"

class Primitive
{
public:
  virtual ~Primitive();
  // Returns true if ray intersects with a t-val less than t_min.
  // Updates normal, t_min, and point accordingly.
  virtual bool RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const { return 0; }
};

class Sphere : public Primitive
{
public:
  Sphere() : Sphere({0, 0, 0}, 1.0) {}
  Sphere(const glm::vec3 &pos, double radius)
      : m_pos(pos), radius(radius), radius_2(radius * radius) {}
  virtual ~Sphere();
  bool RayTest(const Ray &rayy, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const override;

private:
  glm::vec3 m_pos;
  double radius;
  double radius_2;
};

class Box : public Primitive
{
public:
  Box() : Box({0, 0, 0}, 1.0) {}
  Box(const glm::vec3 &pos, double size)
      : m_pos(pos), size(size), max_co(pos + (float)size) {}
  virtual ~Box();
  bool RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const override;

private:
  glm::vec3 m_pos;
  double size;
  glm::vec3 max_co;
};

class Cone : public Primitive
{
public:
  Cone() : Cone({0, 0, 0}, 1.0) {}
  Cone(const glm::vec3 &pos, double size)
      : m_pos(pos), size(size), min_y(pos.y - size), size_2(size * size) {}
  virtual ~Cone();

  bool RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const override;

private:
  glm::vec3 m_pos;
  double size;
  double min_y;
  double size_2;
};

class Cylinder : public Primitive
{
public:
  Cylinder() : Cylinder({0, 0, 0}, 1, 0.5) {}
  Cylinder(const glm::vec3 &pos, double height, double radius)
      : m_pos(pos), radius(radius), radius_2(radius * radius),
        min_y(pos.y - height), max_y(pos.y + height) {}
  virtual ~Cylinder();
  bool RayTest(const Ray &ray, double &t_min, glm::vec3 &normal, glm::vec3 &point, glm::vec2 &uv) const override;

private:
  glm::vec3 m_pos;
  double radius;
  double radius_2;
  double min_y;
  double max_y;
};

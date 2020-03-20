// Winter 2020

#pragma once

#include <glm/glm.hpp>
#include "RayTracer.hpp"

class Primitive
{
public:
  virtual ~Primitive();
  virtual int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const { return 0; }
};

class Sphere : public Primitive
{
public:
  Sphere() : Sphere({0, 0, 0}, 0.5)
  {
  }
  Sphere(const glm::vec3 &pos, double radius)
      : m_pos(pos), radius(radius), radius_2(radius * radius)
  {
  }
  virtual ~Sphere();

  int Intersection(const Ray &rayy, double *t_vals, glm::vec3 &normal) const override;

  double radius;
  double radius_2;

private:
  glm::vec3 m_pos;
};

class Box : public Primitive
{
public:
  Box() : Box({0, 0, 0}, 1.0)
  {
  }

  Box(const glm::vec3 &pos, double size)
      : m_pos(pos), m_size(size), max_co(m_pos + (float)m_size)
  {
  }

  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  virtual ~Box();

private:
  glm::vec3 m_pos;
  double m_size;
  glm::vec3 max_co;
};

class Cone : public Primitive
{
public:
  Cone() : Cone({0, 0, 0}, 1.0)
  {
  }

  Cone(const glm::vec3 &pos, double size)
      : m_pos(pos), m_size(size), min_y(m_pos.y - m_size), size_2(size * size)
  {
  }

  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  virtual ~Cone();

private:
  glm::vec3 m_pos;
  double m_size;
  double min_y;
  double size_2;
};

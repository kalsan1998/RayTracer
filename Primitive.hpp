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
  Sphere() : Sphere({0, 0, 0}, 1.0) {}
  Sphere(const glm::vec3 &pos, double radius)
      : pos(pos), radius(radius), radius_2(radius * radius) {}
  virtual ~Sphere();
  int Intersection(const Ray &rayy, double *t_vals, glm::vec3 &normal) const override;

  double radius;
  double radius_2;

private:
  glm::vec3 pos;
};

class Box : public Primitive
{
public:
  Box() : Box({0, 0, 0}, 1.0) {}
  Box(const glm::vec3 &pos, double size)
      : pos(pos), size(size), max_co(pos + (float)size) {}
  virtual ~Box();
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

private:
  glm::vec3 pos;
  double size;
  glm::vec3 max_co;
};

class Cone : public Primitive
{
public:
  Cone() : Cone({0, 0, 0}, 1.0) {}
  Cone(const glm::vec3 &pos, double size)
      : pos(pos), size(size), min_y(pos.y - size), size_2(size * size) {}
  virtual ~Cone();

  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

private:
  glm::vec3 pos;
  double size;
  double min_y;
  double size_2;
};

class Cylinder : public Primitive
{
public:
  Cylinder() : Cylinder({0, 0, 0}, 1, 0.5) {}
  Cylinder(const glm::vec3 &pos, double height, double radius)
      : pos(pos), radius(radius), radius_2(radius * radius),
        min_y(pos.y - height), max_y(pos.y + height) {}
  virtual ~Cylinder();
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

private:
  glm::vec3 pos;
  double radius;
  double radius_2;
  double min_y;
  double max_y;
};

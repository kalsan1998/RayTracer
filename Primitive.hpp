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

class NonhierSphere : public Primitive
{
public:
  NonhierSphere(const glm::vec3 &pos, double radius)
      : m_pos(pos), a(radius), b(radius), c(radius),
        a_2(radius * radius), b_2(a_2), c_2(a_2)
  {
  }
  virtual ~NonhierSphere();

  int Intersection(const Ray &rayy, double *t_vals, glm::vec3 &normal) const override;

  float a;
  float b;
  float c;

  // Precompute the squares.
  float a_2;
  float b_2;
  float c_2;

private:
  glm::vec3 m_pos;
};

class NonhierBox : public Primitive
{
public:
  NonhierBox(const glm::vec3 &pos, double size)
      : m_pos(pos), m_size(size)
  {
  }
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};

class NonhierCone : public Primitive
{
public:
  NonhierCone(const glm::vec3 &pos, double size)
      : m_pos(pos), m_size(size), m_angle(glm::atan(1.0)),
        cos_angle(cos(m_angle)), cos_angle_2(cos_angle * cos_angle), size_2(size * size)
  {
  }
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  virtual ~NonhierCone();

private:
  glm::vec3 m_pos;
  double m_size;
  double m_angle;
  double cos_angle;
  double cos_angle_2;
  double size_2;
};

class Sphere : public Primitive
{
public:
  Sphere();
  virtual ~Sphere();
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  NonhierSphere internal;
};

class Cube : public Primitive
{
public:
  Cube();
  virtual ~Cube();
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  NonhierBox internal;
};

class Cone : public Primitive
{
public:
  Cone();
  virtual ~Cone();
  int Intersection(const Ray &ray, double *t_vals, glm::vec3 &normal) const override;

  NonhierCone internal;
};

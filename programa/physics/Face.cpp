#include <glm/glm.hpp>

#include "../math/mathhelpers.h"

#include "Face.h"

namespace physics {

Face::Face() : m_a(nullptr), m_b(nullptr), m_c(nullptr), m_d(nullptr) {
}

Face::Face(Particle* a, Particle* b, Particle* c, Particle* d) {
  assert(a != nullptr);
  assert(b != nullptr);
  assert(c != nullptr);
  assert(d != nullptr);
  m_a = a;
  m_b = b;
  m_c = c;
  m_d = d;
}

Face::~Face() {
}

void Face::add_force(const glm::vec3& force) {
  m_a->add_force(force);
  m_b->add_force(force);
  m_c->add_force(force);
  m_d->add_force(force);
}

void Face::make_connection(Particle* a, Particle* b, Particle* c, Particle* d) {
  assert(a != nullptr);
  assert(b != nullptr);
  assert(c != nullptr);
  assert(d != nullptr);
  m_a = a;
  m_b = b;
  m_c = c;
  m_d = d;
}

float Face::get_area() const {
  //For the triangle ACB
  glm::vec3 cross_product = glm::cross(m_c->get_position() - m_a->get_position(), m_b->get_position() - m_a->get_position());
  float area = 0.5f * glm::length(cross_product);
  //For the triangle BCD
  cross_product = glm::cross(m_c->get_position() - m_d->get_position(), m_b->get_position() - m_d->get_position());
  area += 0.5f * glm::length(cross_product);

  return area;
}

glm::vec3 Face::get_normal() const {

  const glm::vec3 atA = glm::normalize(glm::cross(m_c->get_position() - m_a->get_position(), m_b->get_position() - m_a->get_position()));
  const glm::vec3 atB = glm::normalize(glm::cross(m_a->get_position() - m_b->get_position(), m_d->get_position() - m_b->get_position()));
  const glm::vec3 atC = glm::normalize(glm::cross(m_d->get_position() - m_c->get_position(), m_a->get_position() - m_c->get_position()));
  const glm::vec3 atD = glm::normalize(glm::cross(m_b->get_position() - m_d->get_position(), m_c->get_position() - m_d->get_position()));

  return glm::normalize(0.25f * (atA + atB + atC + atD));
}

float Face::get_contributed_volume() const {
  using namespace math;
  float volume{0.0f};

  volume += tethVolume(m_a->get_position(), m_d->get_position(), m_b->get_position());
  volume += tethVolume(m_a->get_position(), m_c->get_position(), m_d->get_position());

  return volume;
}

} // namespace physics

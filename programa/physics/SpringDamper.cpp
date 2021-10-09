#include "SpringDamper.h"
#include <iostream>
namespace physics {
SpringDamper::SpringDamper() : m_L(0.0f), m_a(nullptr), m_b(nullptr) {

}

SpringDamper::~SpringDamper() {
}

SpringDamper::SpringDamper(Particle* a, Particle* b) {
  assert(a != nullptr);
  assert(b != nullptr);
  m_a = a;
  m_b = b;
  m_L = glm::distance(a->get_position(), b->get_position());
}

void SpringDamper::add_force(const float& Ks, const float& Kd) {
  glm::vec3 f, r, v;
  float f_s;

  r = (m_a->get_position() - m_b->get_position());
  v = (m_a->get_velocity() - m_b->get_velocity());

  f_s = Ks * (glm::length(r) - m_L) + Kd * (glm::dot(v, r) / glm::length(r));
  r = glm::normalize(r);
  f = f_s * r;

  m_b->add_force(f);
  m_a->add_force(-1.0f * f);
}

float SpringDamper::get_initial_lenght() const {
  return m_L;
}

void SpringDamper::make_connection(Particle* a, Particle* b) {
  assert(a != nullptr);
  assert(b != nullptr);
  m_a = a;
  m_b = b;
  m_L = glm::distance(a->get_position(), b->get_position());
}

void SpringDamper::set_initial_lenght(float L) {
  assert(L > 0.0f);
  m_L = L;
}

std::pair<glm::vec3, glm::vec3> SpringDamper::get_ends_positions() const {
  return std::make_pair(m_a->get_position(), m_b->get_position());
}
} // namespace physics

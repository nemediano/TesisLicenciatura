#ifndef SPRINGDAMPER_H_
#define SPRINGDAMPER_H_

#include "Particle.h"
#include <utility>

namespace physics {

class SpringDamper {
public:
  SpringDamper();
  SpringDamper(Particle* a, Particle* b);
  void add_force(const float& Ks, const float& Kd);
  float get_initial_lenght() const;
  void make_connection(Particle* a, Particle* b);
  void set_initial_lenght(float L);
  std::pair<glm::vec3, glm::vec3> get_ends_positions() const;
  ~SpringDamper();
private:
  float m_L;
  Particle* m_a;
  Particle* m_b;
};
	
} //namespace physics

#endif

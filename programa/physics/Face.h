#ifndef FACE_H_
#define FACE_H_
#include "Particle.h"
namespace physics {
	class Face {
	public:
		Face();
		Face(Particle* a, Particle* b, Particle* c, Particle* d);
		~Face();
		void add_force(const glm::vec3& force);
		void make_connection(Particle* a, Particle* b, Particle* c, Particle* d);
		float get_contributed_volume() const;
		float get_area() const;
		glm::vec3 get_normal() const;
	private:
		Particle* m_a;
		Particle* m_b;
		Particle* m_c;
		Particle* m_d;
	};
}
#endif

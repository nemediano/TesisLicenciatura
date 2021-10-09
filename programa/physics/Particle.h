#ifndef PARTICLE_H_
#define PARTICLE_H_
#include <glm/glm.hpp>
namespace physics {
	class Particle 	{
	public:
		Particle();
		Particle(const glm::vec3& p, const glm::vec3& v, const glm::vec3& f, const float& m);
		Particle(const glm::vec3& p, const glm::vec3& v, const float& m);
		Particle(const glm::vec3& p, const float& m);
		Particle(const float& m);
		Particle(const glm::vec3& p, const glm::vec3& v, const glm::vec3& f);
		Particle(const glm::vec3& p, const glm::vec3& v);
		Particle(const glm::vec3& p);
		~Particle ();

		void add_force(const glm::vec3& f);
		void add_velocity(const glm::vec3& v);
		void add_position(const glm::vec3& p);
		void set_position(const glm::vec3& p);
		void set_velocity(const glm::vec3& v);
		void set_force(const glm::vec3& f);
		void set_mass(const float& m);
		float get_mass() const;
		glm::vec3 get_position() const;
		glm::vec3 get_velocity() const;
		glm::vec3 get_force() const;
		void fix();
		void release();
		bool fixed() const;
	private:
		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_force;
		float m_mass;
		bool m_fixed;
	};
}

#endif

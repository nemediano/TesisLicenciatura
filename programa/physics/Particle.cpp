#include "Particle.h"
namespace physics {

	Particle::Particle () : m_position(glm::vec3()), m_velocity(glm::vec3()), m_force(glm::vec3()), m_mass(1.0f), m_fixed(false) {
	}

	void Particle::fix() {
		m_fixed = true;
	}

	void Particle::release() {
		m_fixed = false;
	}

	bool Particle::fixed() const {
		return m_fixed;
	}

	Particle::Particle (const glm::vec3& p, const glm::vec3& v, const glm::vec3& f, const float& m) : m_position(p), m_velocity(v), m_force(f), m_fixed(false) {
		assert(m > 0.0f);
		m_mass = m;
	}

	Particle::Particle(const glm::vec3& p, const glm::vec3& v, const float& m) : m_position(p), m_velocity(v), m_fixed(false) {
		assert(m > 0.0f);
		m_mass = m;
	}

	Particle::Particle(const glm::vec3& p, const float& m) : m_position(p), m_fixed(false) {
		assert(m > 0.0f);
		m_mass = m;
	}

	Particle::Particle (const glm::vec3& p, const glm::vec3& v, const glm::vec3& f) : m_position(p), m_velocity(v), m_force(f), m_mass(1.0f), m_fixed(false) {
	}

	Particle::Particle (const glm::vec3& p, const glm::vec3& v) : m_position(p), m_velocity(v), m_mass(1.0f), m_fixed(false) {
	}

	Particle::Particle (const glm::vec3& p) : m_position(p), m_mass(1.0f), m_fixed(false) {
	}

	Particle::~Particle () {
	}

	void Particle::add_force(const glm::vec3& f) {
		m_force += f;
	}

	void Particle::add_velocity(const glm::vec3& v) {
		m_velocity += v;
	}

	void Particle::add_position(const glm::vec3& p) {
		m_position += p;
	}

	void Particle::set_position(const glm::vec3& p) {
		m_position = p;
	}

	void Particle::set_velocity(const glm::vec3& v) {
		m_velocity = v;
	}

	void Particle::set_force(const glm::vec3& f) {
		m_force = f;
	}

	void Particle::set_mass(const float& m) {
		assert(m > 0.0f);
		m_mass = m;
	}

	float Particle::get_mass() const {
		return m_mass;
	}

	glm::vec3 Particle::get_position() const {
		return m_position;
	}

	glm::vec3 Particle::get_velocity() const {
		return m_velocity;
	}

	glm::vec3 Particle::get_force() const {
		return m_force;
	}
}

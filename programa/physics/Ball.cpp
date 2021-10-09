#include "Ball.h"

namespace physics {

Ball::Ball() : m_radious(1.0f), m_integrator(1) {
	reset();
}

Ball::Ball(const glm::vec3& pos, float radious) : Ball() {
	m_particle.set_position(pos);
	m_radious = radious;
	m_integrator = 1;
}

void Ball::setIntegrator(const int& integrator) {
	assert(integrator >= 1 && integrator <= 2);
	m_integrator = integrator;
}

void Ball::update(const float& dt, const float& G) {
	if (m_integrator == 1) {
		euler_integrate(dt, G);
	} else {
		rk4_integrate(dt, G);
	}
}

void Ball::euler_integrate(const float& dt, const float& G) {
	// Clear force
	m_particle.set_force(glm::vec3(0.0f));
	// Accumulate forces
	const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
	// Negative sign comes in the G value
	// For stability I need to scale it
	// m_particle.add_force((G / 100.0f) * UP);
	m_particle.add_force((m_particle.get_mass() * G) * UP);
	// Integrate
	if (!m_particle.fixed()) {
	   m_particle.add_velocity((dt / m_particle.get_mass()) * m_particle.get_force());
	   m_particle.add_position(dt * m_particle.get_velocity());
	} else {
	   m_particle.set_force(glm::vec3(0.0f));
	   m_particle.set_velocity(glm::vec3(0.0f));
	}
}

void Ball::rk4_integrate(const float& dt, const float& G) {
	// Clear force
	m_particle.set_force(glm::vec3(0.0f));

	// Accumulate force
	const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
	m_particle.set_force((m_particle.get_mass() * G) * UP);

	// Create ponderators
	glm::vec3 K1{glm::vec3(0.0f)};
	glm::vec3 K2{glm::vec3(0.0f)};
	glm::vec3 K3{glm::vec3(0.0f)};
	glm::vec3 K4{glm::vec3(0.0f)};
	glm::vec3 L1{glm::vec3(0.0f)};
	glm::vec3 L2{glm::vec3(0.0f)};
	glm::vec3 L3{glm::vec3(0.0f)};
	glm::vec3 L4{glm::vec3(0.0f)};
	/*
	 * Note that gravity does not depend on the position or velocity of the
	 * particle. Therefore I do not need to update it to calculate (no need
	 * for an aux array either) different forces for each ponderator (they are all
	 * the same)
	 */
	// Calculate first set of ponderators
	K1 = m_particle.get_force() / m_particle.get_mass();
	L1 = m_particle.get_velocity();
	// Calculate second set of ponderators
	K2 = m_particle.get_force() / m_particle.get_mass();
	L2 = m_particle.get_velocity() + (dt / 2.0f) * K1;
	// Calculate third set of ponderators
	K3 = m_particle.get_force() / m_particle.get_mass();
	L3 = m_particle.get_velocity() + (dt / 2.0f) * K2;
	// Calculate fourth set of ponderators
	K4 = m_particle.get_force() / m_particle.get_mass();
	L4 = m_particle.get_velocity() + (dt / 2.0f) * K3;

	glm::vec3 deltaPos{glm::vec3(0.0f)};
	glm::vec3 deltaVel{glm::vec3(0.0f)};

	deltaPos = (dt / 6.0f) * (L1 + (2.0f * (L2 + L3)) + L4);
	deltaVel = (dt / 6.0f) * (K1 + (2.0f * (K2 + K3)) + K4);

	if (!m_particle.fixed()) {
		m_particle.add_position(deltaPos);
		m_particle.add_velocity(deltaVel);
	} else {
		const glm::vec3 ZERO = glm::vec3(0.0f);
		m_particle.set_force(ZERO);
		m_particle.set_velocity(ZERO);
	}
}

void Ball::solveCollisionFloor(const float& floor_height) {
	const glm::vec3 floor_normal = glm::vec3(0.0f, 1.0f, 0.0f);
	const float restitution_coeficient = 0.35f;
	const float bias = 0.001f;
	// If you are bellow the floor
	if ((m_particle.get_position().y - m_radious) < floor_height) {
		// First adjust your velocity
		glm::vec3 u = m_particle.get_velocity();
		glm::vec3 s = glm::reflect(u, floor_normal);
		// Quick hack
		glm::vec3 adjusted_velocity = m_particle.get_velocity() + s;
		m_particle.set_velocity(restitution_coeficient * adjusted_velocity);
		// Then your position
		glm::vec3 new_pos = m_particle.get_position();
		new_pos.y = floor_height + m_radious + bias;
		m_particle.set_position(new_pos);
	}
}

void Ball::reset() {
	const glm::vec3 ZERO = glm::vec3(0.0f);
	m_particle.set_velocity(ZERO);
	m_particle.set_position(ZERO);
	m_particle.set_force(ZERO);
}

void Ball::move(const glm::vec3& pos) {
	m_particle.set_position(pos);
}

void Ball::addVelocity(const glm::vec3& velocity) {
	m_particle.add_velocity(velocity);
}

void Ball::setVelocity(const glm::vec3& velocity) {
	m_particle.set_velocity(velocity);
}

void Ball::setRadious(float radious) {
	m_radious = radious;
}

void Ball::setMass(float mass) {
	m_particle.set_mass(mass);
}

glm::vec3 Ball::getPosition() const {
    return m_particle.get_position();
}

glm::vec3 Ball::getVelocity() const {
    return m_particle.get_velocity();
}

float Ball::getMass() const {
    return m_particle.get_mass();
}

float Ball::getRadious() const {
    return m_radious;
}

Ball::~Ball() {

}

}

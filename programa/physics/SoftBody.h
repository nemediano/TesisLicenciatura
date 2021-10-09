#ifndef SOFT_BODY_H_
#define SOFT_BODY_H_

#include <vector>
#include <array>

#include "../math/hrclock.h"
#include "../mesh/mesh.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Face.h"
#include "Ball.h"

namespace physics {

struct SoftBodyParameters {
    float Ks;
    float Kd;
    float Kg;
    float M;
    float G;
    float MSphere;
	bool pressure;
	bool gravity;

    void setDefault() {
      Ks = 100.0f;
      Kd = 5.0f;
      Kg = 17.0f;
      G = -10.0f;
      MSphere = 0.8f;
      pressure = true;
      gravity = true;
    }
};

struct SBTimers {
	math::HRClock gravityTime; // 0
	math::HRClock springTime;  // 1
	math::HRClock volumeTime;  // 2
	math::HRClock preasureTime;// 3
	math::HRClock forcesTime;  // 4

	std::array<double, 5> plotData;

	void reset() {
		gravityTime.reset();
		springTime.reset();
		volumeTime.reset();
		preasureTime.reset();
		forcesTime.reset();
	}

	void toZeroes() {
		for (auto& d : plotData) {
			d = 0.0;
		}
	}
};

class SoftBody {
public:
    SoftBodyParameters mParams;
    SoftBody();
    SoftBody(int side_size);
    void reset();
    void update(float dt);
    float mSoftBodyVolume;
    mesh::Mesh getBoxSolidMesh() const;
    mesh::Mesh getFabricSolidMesh();
    mesh::Mesh getFabricWireMesh();
    mesh::Mesh getBoxWireMesh() const;
    void updateFabricSolidVertices(std::vector<mesh::Vertex>& vertices) const;
    void updateFabricWireVertices(std::vector<mesh::Vertex>& vertices) const;
    void solveCollisionFloor(float floor_height);
    void solveCollisionSphere(physics::Ball& ball);
    void setIntegrator(const int& integrator);
    ~SoftBody();
    SBTimers mTimers;
private:
    std::vector<unsigned int> m_spring_indices;
    std::vector<Particle> m_particles;
    std::vector<Face> m_faces;
    std::vector<SpringDamper> m_springs;
    // Auxiliary arrays for the Runge - Kutta method
    std::vector<Particle> m_aux_part;
	std::vector<Face> m_aux_face;
	std::vector<SpringDamper> m_aux_spdm;
    float m_box_height;
    float m_box_width;
    size_t m_side;
    int m_integrator;
    void recreate_particles(const mesh::Mesh& fabric);
    void acumulateForces(std::vector<Particle>& particles, std::vector<SpringDamper>& springs, std::vector<Face>& faces);
    void clearForces(std::vector<Particle>& particles);
    void euler_integrate(float dt);
    void rk4_integrate(float dt);
    void addGravity(std::vector<Particle>& particles);
    void addSpringDamper(std::vector<SpringDamper>& springs);
    void addPreassure(std::vector<Face>& faces);
    float calculateVolume(std::vector<Face>& faces);
    void connectSprings(std::vector<Particle>& particles, std::vector<SpringDamper>& springs);
    void connectFaces(std::vector<Particle>& particles, std::vector<Face>& faces);
    glm::vec3 normalPart(const glm::vec3& v, const glm::vec3& n) const;
    glm::vec3 adjust_positions(physics::Ball& ball, physics::Particle& p);
    void adjust_velocities(physics::Ball& ball, physics::Particle& p, const glm::vec3& normal);
    inline size_t realIndex(const size_t& i, const size_t& j) const {
    	assert(i >= 0);
    	assert(j >= 0);
    	assert(i < m_side);
    	assert(j < m_side);

    	return j * m_side + i;
    }
    inline glm::ivec2 simIndexes(const size_t& index) const {
    	assert(index >= 0);
    	assert(index < (m_side * m_side));

    	glm::vec2 indexes;
    	indexes.x = index % m_side;
    	indexes.y = index / m_side;

    	return indexes;
    }
};

}

#endif

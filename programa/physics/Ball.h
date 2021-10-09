#ifndef BALL_H_
#define BALL_H_

#include <vector>

#include <glm/glm.hpp>

#include "../mesh/mesh.h"
#include "Particle.h"

namespace physics {

class Ball {
public:
    Ball();
    Ball(const glm::vec3& pos, float radious);
    void update(const float& dt, const float& G);
    void reset();
    void move(const glm::vec3& pos);
    void addVelocity(const glm::vec3& velocity);
    void setVelocity(const glm::vec3& velocity);
    void setRadious(float radious);
    void setMass(float mass);
    void solveCollisionFloor(const float& floor_height);
    glm::vec3 getPosition() const;
    glm::vec3 getVelocity() const;
    float getMass() const;
    float getRadious() const;
    void setIntegrator(const int& integrator);
    ~Ball();
private:
    float m_radious;
    Particle m_particle;
    int m_integrator;
    void euler_integrate(const float& dt, const float& G);
    void rk4_integrate(const float& dt, const float& G);
};

}

#endif

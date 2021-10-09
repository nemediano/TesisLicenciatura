#include <iostream>
#include "../math/mathhelpers.h"
#include "../ogl/oglhelpers.h"
#include "SoftBody.h"

namespace physics {

using namespace ogl;
using namespace std;

SoftBody::SoftBody(){
  m_box_height = 0.75f;
  m_box_width = 1.5f;
  m_side = 22;
  m_integrator = 2;
  mParams.setDefault();
  mSoftBodyVolume = 0.0f;
  mTimers.reset();
}

SoftBody::SoftBody(int side_size) {
  assert(side_size > 0);
  m_side = side_size;
  m_box_height = 0.75f;
  m_box_width = 1.5f;
  m_integrator = 2;
  mParams.setDefault();
  mSoftBodyVolume = 0.0f;
  mTimers.reset();
}

mesh::Mesh SoftBody::getBoxSolidMesh() const {
  using namespace mesh;
  using glm::vec3;
  using glm::vec2;
  using std::vector;

  Mesh box;
  vector<vec3> points(8);
  vector<vec3> normals(5);
  vector<vec2> textCoords(4);

  points[0] = vec3(-m_box_width / 2.0f,         0.0f, -m_box_width / 2.0f);
  points[1] = vec3( m_box_width / 2.0f,         0.0f, -m_box_width / 2.0f);
  points[2] = vec3( m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  points[3] = vec3(-m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  points[4] = vec3(-m_box_width / 2.0f,         0.0f,  m_box_width / 2.0f);
  points[5] = vec3( m_box_width / 2.0f,         0.0f,  m_box_width / 2.0f);
  points[6] = vec3( m_box_width / 2.0f, m_box_height,  m_box_width / 2.0f);
  points[7] = vec3(-m_box_width / 2.0f, m_box_height,  m_box_width / 2.0f);

  normals[0] = vec3( 0.0f,  0.0f, -1.0f);
  normals[1] = vec3( 0.0f, -1.0f,  0.0f);
  normals[2] = vec3(-1.0f,  0.0f,  0.0f);
  normals[3] = vec3( 0.0f,  0.0f,  1.0f);
  normals[4] = vec3( 1.0f,  0.0f,  0.0f);

  textCoords[0] = vec2(0.0f, 0.0f);
  textCoords[1] = vec2(0.0f, 1.0f);
  textCoords[2] = vec2(1.0f, 1.0f);
  textCoords[3] = vec2(1.0f, 0.0f);

  vector<unsigned int> indices;
  vector<Vertex> vertices;
  Vertex v;
  //Back face of the cube
  v.position = points[0]; v.normal = normals[0]; v.textCoords = textCoords[2]; vertices.push_back(v); //0
  v.position = points[1]; v.normal = normals[0]; v.textCoords = textCoords[1]; vertices.push_back(v); //1
  v.position = points[2]; v.normal = normals[0]; v.textCoords = textCoords[0]; vertices.push_back(v); //2
  v.position = points[3]; v.normal = normals[0]; v.textCoords = textCoords[3]; vertices.push_back(v); //3
  indices.push_back(2);
  indices.push_back(1);
  indices.push_back(0);
  indices.push_back(3);
  indices.push_back(2);
  indices.push_back(0);
  //Bottom face of the cube
  v.position = points[0]; v.normal = normals[1]; v.textCoords = textCoords[3]; vertices.push_back(v); //4
  v.position = points[1]; v.normal = normals[1]; v.textCoords = textCoords[0]; vertices.push_back(v); //5
  v.position = points[5]; v.normal = normals[1]; v.textCoords = textCoords[1]; vertices.push_back(v); //6
  v.position = points[4]; v.normal = normals[1]; v.textCoords = textCoords[2]; vertices.push_back(v); //7
  indices.push_back(4);
  indices.push_back(5);
  indices.push_back(6);
  indices.push_back(4);
  indices.push_back(6);
  indices.push_back(7);
  //Left face of the cube
  v.position = points[0]; v.normal = normals[2]; v.textCoords = textCoords[1]; vertices.push_back(v); //8
  v.position = points[3]; v.normal = normals[2]; v.textCoords = textCoords[0]; vertices.push_back(v); //9
  v.position = points[4]; v.normal = normals[2]; v.textCoords = textCoords[2]; vertices.push_back(v); //10
  v.position = points[7]; v.normal = normals[2]; v.textCoords = textCoords[3]; vertices.push_back(v); //11
  indices.push_back(8);
  indices.push_back(10);
  indices.push_back(11);
  indices.push_back(8);
  indices.push_back(11);
  indices.push_back(9);
  //Right face of the cube
  v.position = points[1]; v.normal = normals[4]; v.textCoords = textCoords[2]; vertices.push_back(v); //12
  v.position = points[2]; v.normal = normals[4]; v.textCoords = textCoords[3]; vertices.push_back(v); //13
  v.position = points[5]; v.normal = normals[4]; v.textCoords = textCoords[1]; vertices.push_back(v); //14
  v.position = points[6]; v.normal = normals[4]; v.textCoords = textCoords[0]; vertices.push_back(v); //15
  indices.push_back(12);
  indices.push_back(13);
  indices.push_back(14);
  indices.push_back(13);
  indices.push_back(15);
  indices.push_back(14);
  //Front face of the cube
  v.position = points[4]; v.normal = normals[3]; v.textCoords = textCoords[1]; vertices.push_back(v); //16
  v.position = points[5]; v.normal = normals[3]; v.textCoords = textCoords[2]; vertices.push_back(v); //17
  v.position = points[6]; v.normal = normals[3]; v.textCoords = textCoords[3]; vertices.push_back(v); //18
  v.position = points[7]; v.normal = normals[3]; v.textCoords = textCoords[0]; vertices.push_back(v); //19
  indices.push_back(16);
  indices.push_back(17);
  indices.push_back(18);
  indices.push_back(16);
  indices.push_back(18);
  indices.push_back(19);

  box.loadVerticesAndIndices(vertices, indices, true, true);

  return box;
}

mesh::Mesh SoftBody::getBoxWireMesh() const {
  using namespace mesh;
  using glm::vec3;
  using glm::vec2;
  using std::vector;

  Mesh box;
  vector<unsigned int> indices;
  vector<Vertex> vertices;

  vector<vec3> points(8);

  points[0] = vec3(-m_box_width / 2.0f,         0.0f, -m_box_width / 2.0f);
  points[1] = vec3( m_box_width / 2.0f,         0.0f, -m_box_width / 2.0f);
  points[2] = vec3( m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  points[3] = vec3(-m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  points[4] = vec3(-m_box_width / 2.0f,         0.0f,  m_box_width / 2.0f);
  points[5] = vec3( m_box_width / 2.0f,         0.0f,  m_box_width / 2.0f);
  points[6] = vec3( m_box_width / 2.0f, m_box_height,  m_box_width / 2.0f);
  points[7] = vec3(-m_box_width / 2.0f, m_box_height,  m_box_width / 2.0f);

  Vertex v;
  v.normal = vec3(0.0f); // We use the normal field to store the force
  for (size_t i = 0; i < points.size(); ++i) {
	v.position = points[i];
	vertices.push_back(v);
  }

  //Now create 12 lines to create the box
  // Bottom square
  indices.push_back(0);
  indices.push_back(1);

  indices.push_back(1);
  indices.push_back(5);

  indices.push_back(5);
  indices.push_back(4);

  indices.push_back(4);
  indices.push_back(0);
  // Top square
  indices.push_back(3);
  indices.push_back(2);

  indices.push_back(2);
  indices.push_back(6);

  indices.push_back(6);
  indices.push_back(7);

  indices.push_back(7);
  indices.push_back(3);
  // Height lines
  indices.push_back(0);
  indices.push_back(3);

  indices.push_back(1);
  indices.push_back(2);

  indices.push_back(5);
  indices.push_back(6);

  indices.push_back(4);
  indices.push_back(7);

  box.loadVerticesAndIndices(vertices, indices, true, false);

  return box;
}

mesh::Mesh SoftBody::getFabricWireMesh() {
  using namespace mesh;
  std::vector<unsigned int> indices;
  std::vector<Vertex> vertices;

  const float DELTA = m_box_width / (m_side - 1);
  const glm::vec3 ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 position = glm::vec3(-m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);

  for (size_t j = 0; j < m_side; ++j) {
    position.x = -m_box_width / 2.0f;
    for (size_t i = 0; i < m_side; ++i) {
      Vertex v;
      v.position = position;
      v.normal = ZERO;
      v.textCoords = glm::vec2(0.0f);
      vertices.push_back(v);
      position.x += DELTA;
    }
    position.z += DELTA;
  }
  assert(vertices.size() == (m_side * m_side));

  //Create two contiguous series of indexes
  for (size_t j = 0; j < m_side; ++j) {
    for (size_t i = 0; i < m_side; ++i) {
      indices.push_back(realIndex(i, j)); // These are the points that represents the particles
    }
  }
  assert(indices.size() == vertices.size());
  // Insert the indices for the lines that will represent the springs
  indices.insert(indices.end(), m_spring_indices.begin(), m_spring_indices.end());
  assert(indices.size() == (m_particles.size() + 2 * m_springs.size()));

  Mesh fabric;
  fabric.loadVerticesAndIndices(vertices, indices, true, false);

  return fabric;
}

mesh::Mesh SoftBody::getFabricSolidMesh() {
  using namespace mesh;

  std::vector<unsigned int> indices;
  std::vector<Vertex> vertices;
  const float DELTA = m_box_width / (m_side - 1);
  const float DELTA_TEXTURE = 1.0f / (m_side - 1);
  const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 position = glm::vec3(-m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  glm::vec2 textCoords = glm::vec2(0.0f, 0.0f);

  for (size_t j = 0; j < m_side; ++j) {
    position.x = -m_box_width / 2.0f;
    textCoords.s = 0.0f;
    for (size_t i = 0; i < m_side; ++i) {
      Vertex v;
      v.position = position;
	  v.normal = UP;
      v.textCoords = textCoords;
      vertices.push_back(v);
      position.x += DELTA;
	  textCoords.s += DELTA_TEXTURE;
    }
    position.z += DELTA;
    textCoords.t += DELTA_TEXTURE;
  }
  assert(vertices.size() == (m_side * m_side));

  //Create the triangles
  for (size_t j = 0; j < (m_side - 1); ++j) {
    for (size_t i = 0; i < (m_side - 1); ++i) {
      size_t index_corner = realIndex(i, j);
      size_t index_right = realIndex(i + 1, j);
      size_t index_down = realIndex(i, j + 1);
      size_t index_oposite = realIndex(i + 1, j + 1);
      if ((i + j) % 2 == 0) { // For visual stability reasons
        // First triangle
        indices.push_back(index_right);
        indices.push_back(index_corner);
        indices.push_back(index_down);
        // Second triangle
        indices.push_back(index_down);
        indices.push_back(index_oposite);
        indices.push_back(index_right);
      } else {
        // First triangle
        indices.push_back(index_right);
        indices.push_back(index_corner);
        indices.push_back(index_oposite);
        // Second triangle
        indices.push_back(index_oposite);
        indices.push_back(index_corner);
        indices.push_back(index_down);
      }
    }
  }

  Mesh fabric;
  fabric.loadVerticesAndIndices(vertices, indices, true, true);

  recreate_particles(fabric);

  return fabric;
}

void SoftBody::recreate_particles(const mesh::Mesh& fabric) {
  // Destroy previous fabric
  m_particles.clear();
  m_aux_part.clear();

  // Get mesh vertices, since we will need the initial positions
  std::vector<mesh::Vertex> vertices = fabric.getVertices();

  assert(vertices.size() == (m_side * m_side));
  // Each particle's mass is total fabric mass divided by particles number
  const float P_MASS = 10.0f / (m_side * m_side);

  // Create particles
  for (size_t k = 0; k < vertices.size(); ++k) {
    const glm::vec3 velocity = glm::vec3(0.0);
    const glm::vec3 force = glm::vec3(0.0);
    physics::Particle p {vertices[k].position, velocity, force, P_MASS};
    // If you are on the edges get fixed
    glm::ivec2 indexes = simIndexes(k);
    size_t i = indexes.x;
    size_t j = indexes.y;
    if (i == (m_side - 1) || i == 0 || j == (m_side - 1) || j == 0 ) {
      p.fix();
    }
    m_particles.push_back(p);
    //Also make a copy to the aux
    m_aux_part.push_back(p);
  }
  // Create SpringDampers
  connectSprings(m_particles, m_springs);
  // copy for the aux
  connectSprings(m_aux_part, m_aux_spdm);
  //Create faces
  connectFaces(m_particles, m_faces);
  // copy for the aux
  connectFaces(m_aux_part, m_aux_face);

  mSoftBodyVolume = calculateVolume(m_faces);
}

void SoftBody::connectSprings(std::vector<Particle>& particles, std::vector<SpringDamper>& springs) {
  // Empty the given array of spring - dampers
  springs.clear();
  m_spring_indices.clear();
  // Reconnect it using the particles array
  for (size_t k = 0; k < particles.size(); ++k) {
    // Get your relative indices
    glm::ivec2 indexes = simIndexes(k);
    size_t i = indexes.x;
    size_t j = indexes.y;
    // If you have a right neighbor and you are not in the upper or lower edge
    if (i < (m_side - 1) && j > 0 && j < (m_side - 1)) {
      SpringDamper sd{&particles[realIndex(i, j)], &particles[realIndex(i + 1, j)]};
      springs.push_back(sd);
      // Keep a reference of the connected indices (for wireframe rendering)
      m_spring_indices.push_back(realIndex(i, j));
      m_spring_indices.push_back(realIndex(i + 1, j));
    }
    // If you have down neighbor and you are not in the right or left edge
    if (j < (m_side - 1) && i > 0 && i < (m_side - 1)) {
      SpringDamper sd{&particles[realIndex(i, j)], &particles[realIndex(i, j + 1)]};
      springs.push_back(sd);
      // Keep a reference of the connected indices (for wireframe rendering)
      m_spring_indices.push_back(realIndex(i, j));
      m_spring_indices.push_back(realIndex(i, j + 1));
    }
  }
  assert(springs.size() == ((m_side - 1) * (m_side - 2) * 2));
}

void SoftBody::connectFaces(std::vector<Particle>& particles, std::vector<Face>& faces) {
  // Empty the given array of quad faces
  faces.clear();
  // Reconnect it using the given particles array
  for (size_t k = 0; k < particles.size(); ++k) {
    // Get your relative indices
    glm::ivec2 indexes = simIndexes(k);
    size_t i = indexes.x;
    size_t j = indexes.y;
    // If you have a both a right and a down neighbors
    if (i < (m_side - 1) && j < (m_side - 1)) {
      Face face{
          &particles[realIndex(i,     j)], &particles[realIndex(i + 1,     j)],
          &particles[realIndex(i, j + 1)], &particles[realIndex(i + 1, j + 1)]
      };
	  faces.push_back(face);
    }
  }
  assert(faces.size() == (m_side - 1) * (m_side - 1));
}

void SoftBody::updateFabricSolidVertices(std::vector<mesh::Vertex>& vertices) const {
  using std::cout;
  using std::endl;

  assert(vertices.size() == m_particles.size());
  // Copy vertex positions
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].position = m_particles[i].get_position();
  }
  // Recalculate normals based on current positions
  for (size_t k = 0; k < vertices.size(); ++k) {
    // Get your relative indices
    glm::vec2 indexes = simIndexes(k);
    int i = indexes.x;
    int j = indexes.y;
    // Current vertex is consider the center and can have up too 8 neighbors (for the normal purposes)
    glm::vec3 n = glm::vec3(0.0f);
    glm::vec3 center = vertices[realIndex(i, j)].position;
    // We iterate trough all our neighbors accumulating a normal
    // If we have right and down neighbors
    if ((i + 1) < int(m_side) && (j + 1) < int(m_side)) {
      // These vector are from the center to a given direction
      glm::vec3 down = glm::vec3(vertices[realIndex(i, j + 1)].position - center);
      glm::vec3 right = glm::vec3(vertices[realIndex(i + 1, j)].position - center);
      glm::vec3 down_right = glm::vec3(vertices[realIndex(i + 1, j + 1)].position - center);
      // Add the local normals
      n += glm::normalize(glm::cross(down, down_right));
      n += glm::normalize(glm::cross(down_right, right));
    }
    // If we have right and up neighbors
    if ((i + 1) < int(m_side) && (j - 1) >= 0) {
      // These vector are from the center to a given direction
      glm::vec3 up = glm::vec3(vertices[realIndex(i, j - 1)].position - center);
      glm::vec3 right = glm::vec3(vertices[realIndex(i + 1, j)].position - center);
      glm::vec3 up_right = glm::vec3(vertices[realIndex(i + 1, j - 1)].position - center);
      // Add the local normals
      n += glm::normalize(glm::cross(right, up_right));
      n += glm::normalize(glm::cross(up_right, up));
    }
    // If we have left and up neighbors
    if ((i - 1) >= 0 && (j - 1) >= 0) {
      // These vector are from the center to a given direction
      glm::vec3 up = glm::vec3(vertices[realIndex(i, j - 1)].position - center);
      glm::vec3 left = glm::vec3(vertices[realIndex(i - 1, j)].position - center);
      glm::vec3 up_left = glm::vec3(vertices[realIndex(i - 1, j - 1)].position - center);
      // Add the local normals
      n += glm::normalize(glm::cross(up, up_left));
      n += glm::normalize(glm::cross(up_left, left));
    }
    // If we have left and down neighbors
    if ((i - 1) >= 0 && (j + 1) < int(m_side)) {
      // These vector are from the center to a given direction
      glm::vec3 down = glm::vec3(vertices[realIndex(i, j + 1)].position - center);
      glm::vec3 left = glm::vec3(vertices[realIndex(i - 1, j)].position - center);
      glm::vec3 down_left = glm::vec3(vertices[realIndex(i - 1, j + 1)].position - center);
      // Add the local normals
      n += glm::normalize(glm::cross(left, down_left));
	  n += glm::normalize(glm::cross(down_left, down));
    }
    vertices[k].normal = glm::normalize(n);
  }
}

void SoftBody::updateFabricWireVertices(std::vector<mesh::Vertex>& vertices) const {
  assert(vertices.size() == m_particles.size());
  // Copy vertex positions and use normals to store force
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].position = m_particles[i].get_position();
    vertices[i].normal = m_particles[i].get_force();
  }
}

void SoftBody::euler_integrate(float dt) {
  const glm::vec3 ZERO = glm::vec3(0.0f);
  for (auto& particle : m_particles) {
    if (!particle.fixed()) {
      particle.add_velocity((dt / particle.get_mass()) * particle.get_force());
      particle.add_position(dt * particle.get_velocity());
    } else {
      particle.set_force(ZERO);
      particle.set_velocity(ZERO);
    }
  }
}

void SoftBody::rk4_integrate(float dt) {
  // quick check for sync
  assert(m_particles.size() == m_aux_part.size());
  assert(m_springs.size() == m_aux_spdm.size());
  assert(m_faces.size() == m_aux_face.size());
  // create the 8 sets of ponderators
  std::vector<glm::vec3> K1{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> K2{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> K3{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> K4{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> L1{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> L2{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> L3{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> L4{m_particles.size(), glm::vec3(0.0f)};

  // Calculate first set of ponderators
  for(size_t i = 0; i < m_particles.size(); ++i) {
    K1[i] = m_particles[i].get_force() / m_particles[i].get_mass();
    L1[i] = m_particles[i].get_velocity();
  }
  // Copy over the auxiliary
  for(size_t i = 0; i < m_particles.size(); ++i) {
    m_aux_part[i].set_position(m_particles[i].get_position());
    m_aux_part[i].set_velocity(m_particles[i].get_velocity());
  }
  // Accumulate forces for the aux array
  clearForces(m_aux_part);
  acumulateForces(m_aux_part, m_aux_spdm, m_aux_face);
  // Calculate second set of ponderators
  for(size_t i = 0; i < m_aux_part.size(); ++i) {
    K2[i] = m_aux_part[i].get_force() / m_aux_part[i].get_mass();
    L2[i] = m_aux_part[i].get_velocity() + (dt / 2.0f) * K1[i];
  }
  // Copy over the auxiliary
  for(size_t i = 0; i < m_aux_part.size(); ++i) {
    m_aux_part[i].set_position(m_particles[i].get_position() + (dt / 2.0f) * L2[i]);
    m_aux_part[i].set_velocity(m_particles[i].get_velocity() + (dt / 2.0f) * K2[i]);
  }
  // Accumulate forces for the aux array
  clearForces(m_aux_part);
  acumulateForces(m_aux_part, m_aux_spdm, m_aux_face);
  // Calculate third set of ponderators
  for(size_t i = 0; i < m_aux_part.size(); ++i) {
    K3[i] = m_aux_part[i].get_force() / m_aux_part[i].get_mass();
    L3[i] = m_aux_part[i].get_velocity() + (dt / 2.0f) * K2[i];
  }
  // Copy over the auxiliary
  for(size_t i = 0; i < m_aux_part.size(); ++i) {
    m_aux_part[i].set_position(m_particles[i].get_position() + (dt / 2.0f) * L3[i]);
    m_aux_part[i].set_velocity(m_particles[i].get_velocity() + (dt / 2.0f) * K3[i]);
  }
  // Accumulate forces for the aux array
  clearForces(m_aux_part);
  acumulateForces(m_aux_part, m_aux_spdm, m_aux_face);
  // Calculate fourth set of ponderators
  for(size_t i = 0; i < m_aux_part.size(); ++i) {
    K4[i] = m_aux_part[i].get_force() / m_aux_part[i].get_mass();
    L4[i] = m_aux_part[i].get_velocity() + (dt / 2.0f) * K3[i];
  }
  // Now, that we have the ponderators, lets get deltas
  std::vector<glm::vec3> deltaPos{m_particles.size(), glm::vec3(0.0f)};
  std::vector<glm::vec3> deltaVel{m_particles.size(), glm::vec3(0.0f)};
  for (size_t i = 0; i < m_particles.size(); ++i) {
    deltaPos[i] = (dt / 6.0f) * (L1[i] + (2.0f * (L2[i] + L3[i])) + L4[i]);
    deltaVel[i] = (dt / 6.0f) * (K1[i] + (2.0f * (K2[i] + K3[i])) + K4[i]);
  }
  // Finally, for each particle update position and velocity
  for (size_t i = 0; i < m_particles.size(); ++i) {
    if (!m_particles[i].fixed()) {
      m_particles[i].add_position(deltaPos[i]);
      m_particles[i].add_velocity(deltaVel[i]);
    } else {
      const glm::vec3 ZERO = glm::vec3(0.0f);
      m_particles[i].set_force(ZERO);
      m_particles[i].set_velocity(ZERO);
    }
  }
}

void SoftBody::update(float dt) {
  mTimers.toZeroes();
  clearForces(m_particles);
  acumulateForces(m_particles, m_springs, m_faces);
  if (m_integrator == 1) {
    euler_integrate(dt);
  } else {
    rk4_integrate(dt);
  }
}

void SoftBody::acumulateForces(std::vector<Particle>& particles, std::vector<SpringDamper>& springs, std::vector<Face>& faces) {
  mTimers.forcesTime.tick();
  if (mParams.gravity) {
    addGravity(particles);
  }
  addSpringDamper(springs);
  if (mParams.pressure) {
    addPreassure(faces);
  }
  mTimers.forcesTime.tock();
  mTimers.plotData[4] += mTimers.forcesTime.getDeltaMiliS();
}

void SoftBody::clearForces(std::vector<Particle>& particles) {
  const glm::vec3 ZERO = glm::vec3(0.0f);
  for (auto& particle : particles) {
    particle.set_force(ZERO);
  }
}

void SoftBody::addGravity(std::vector<Particle>& particles) {
  const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
  mTimers.gravityTime.tick();
  for (auto& particle : particles) {
    // Negative sign comes in the G value
    particle.add_force((particle.get_mass() * mParams.G) * UP);
  }
  mTimers.gravityTime.tock();
  mTimers.plotData[0] += mTimers.gravityTime.getDeltaMiliS();
}

void SoftBody::addSpringDamper(std::vector<SpringDamper>& springs) {
  mTimers.springTime.tick();
  for (auto& spring : springs) {
    spring.add_force(mParams.Ks, mParams.Kd);
  }
  mTimers.springTime.tock();
  mTimers.plotData[1] += mTimers.springTime.getDeltaMiliS();
}

void SoftBody::addPreassure(std::vector<Face>& faces) {
  mTimers.preasureTime.tick();
  // Update current volume
  mTimers.volumeTime.tick();
  mSoftBodyVolume = calculateVolume(faces);
  mTimers.volumeTime.tock();
  mTimers.plotData[2] += mTimers.volumeTime.getDeltaMiliS();
  for (auto& face : faces) {
    // Accumulate the pressure forces in the fabric faces
    glm::vec3 force = ((face.get_area() * mParams.Kg) / mSoftBodyVolume) * face.get_normal();
    face.add_force(force);
  }
  mTimers.preasureTime.tock();
  mTimers.plotData[3] += mTimers.preasureTime.getDeltaMiliS();
}

float SoftBody::calculateVolume(std::vector<Face>& faces) {
  using namespace math;

  float volume{0.0f};
  // First, the fixed faces.
  // Start by calculating the corners of the box
  const glm::vec3 v0 = glm::vec3(-m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  const glm::vec3 v1 = glm::vec3( m_box_width / 2.0f, m_box_height, -m_box_width / 2.0f);
  const glm::vec3 v2 = glm::vec3(-m_box_width / 2.0f, m_box_height,  m_box_width / 2.0f);
  const glm::vec3 v3 = glm::vec3( m_box_width / 2.0f, m_box_height,  m_box_width / 2.0f);
  const glm::vec3 v4 = glm::vec3(-m_box_width / 2.0f,         0.0f, -m_box_width / 2.0f);
  const glm::vec3 v5 = glm::vec3( m_box_width / 2.0f,         0.0f, -m_box_width / 2.0f);
  const glm::vec3 v6 = glm::vec3(-m_box_width / 2.0f,         0.0f,  m_box_width / 2.0f);
  const glm::vec3 v7 = glm::vec3( m_box_width / 2.0f,         0.0f,  m_box_width / 2.0f);
  // Front face
  volume += tethVolume(v2, v7, v3);
  volume += tethVolume(v2, v6, v7);
  // Back face
  volume += tethVolume(v0, v1, v5);
  volume += tethVolume(v0, v5, v4);
  // Left face
  volume += tethVolume(v2, v0, v4);
  volume += tethVolume(v2, v4, v6);
  // Right face
  volume += tethVolume(v3, v5, v1);
  volume += tethVolume(v3, v7, v5);
  // Down face
  volume += tethVolume(v4, v5, v7);
  volume += tethVolume(v4, v7, v6);

  for (const auto& face : faces) {
    volume += face.get_contributed_volume();
  }

  return (1.0f / 6.0f) * volume;
}

void SoftBody::reset() {
  auto aux_mesh = getFabricSolidMesh();
  recreate_particles(aux_mesh);
}

SoftBody::~SoftBody() {

}

void SoftBody::solveCollisionFloor(float floor_height) {
  const glm::vec3 floor_normal = glm::vec3(0.0f, 1.0f, 0.0f);
  const float restitution_coeficient = 0.5f;
  const float bias = 0.01f;
  // For all particles
  for (auto& p : m_particles) {
    // If you are bellow the floor
    if (p.get_position().y < floor_height) {
      // First adjust your velocity
	  glm::vec3 u = p.get_velocity();
      glm::vec3 s = glm::reflect(u, floor_normal);
      p.add_velocity(restitution_coeficient * s);
      // Then your position
      glm::vec3 new_pos = p.get_position();
      new_pos.y = floor_height + bias;
      p.set_position(new_pos);
    }
  }
}

void SoftBody::setIntegrator(const int& integrator) {
  assert(integrator >= 1 && integrator <= 2);
  m_integrator = integrator;
}

glm::vec3 SoftBody::normalPart(const glm::vec3& v, const glm::vec3& n) const {
  return glm::dot(v, n) * n;
}

void SoftBody::solveCollisionSphere(physics::Ball& ball) {
  const float radious_squared = ball.getRadious() * ball.getRadious();
  // For all particles
  for (auto& p : m_particles) {
    // If you are inside the sphere
    if (glm::distance2(ball.getPosition(), p.get_position()) < radious_squared) {
      // Solving collision is done in two parts:
      // First, you move the object away of each other
      const glm::vec3 normal = adjust_positions(ball, p);
      // Second, you adjust the corresponding velocities
      adjust_velocities(ball, p, normal);
    }
  }
}

glm::vec3 SoftBody::adjust_positions(physics::Ball& ball, physics::Particle& p) {
  // Calculate colision's normal
  glm::vec3 normal = glm::normalize(p.get_position()- ball.getPosition());
  // How to separate depends on the particle being fix
  if (p.fixed()) {
    // Move ball outside the particle along the normal
    ball.move(p.get_position() - (ball.getRadious() * normal));
  } else {
    // Move particle outside the ball along the normal
    p.set_position(ball.getPosition() + (ball.getRadious() * normal));
  }

  return normal;
}

void SoftBody::adjust_velocities(physics::Ball& ball, physics::Particle& p, const glm::vec3& normal) {
  // Use momentum equations to solve the elastic collision
  const float ratio = ball.getMass() / p.get_mass();
  // Calculate resultant velocity
  glm::vec3 u = ball.getVelocity() - p.get_velocity();
  // Split it in two components: normal and tangential with the respect of the collision plane
  glm::vec3 u_n = normalPart(u, normal);
  glm::vec3 u_t = u - u_n;
  // Calculate normal velocities after the collision
  glm::vec3 s_n = ((ratio - 1.0f) / (ratio + 1.0f)) * u_n;
  glm::vec3 w_n = ((2.0f * ratio) / (ratio + 1.0f)) * u_n;
  // Adjust ball's velocity
  ball.setVelocity(u_t + s_n + p.get_velocity());
  // Accumulate particle's return velocity
  p.add_velocity(w_n);
}

}


#include "Flag.h"
#include <iostream>
namespace scene {
	Flag::Flag () : m_width(3), m_height(5) {
		recreate_flag();
	}

	Flag::Flag (unsigned int width, unsigned int height) {
		assert(width > 0);
		assert(height > 0);
		m_width = width;
		m_height = height;
		recreate_flag();
	}

	Flag::Flag (unsigned int minimun_particles) {
		assert(minimun_particles > 0);
		m_width = minimun_particles;
		m_height = 2 * m_width;
		recreate_flag();
	}

	Flag::~Flag () {
	}

	void Flag::set_width(unsigned int width) {
		assert(width > 0);
		m_width = width;
		recreate_flag();
	}

	void Flag::set_height(unsigned int height) {
		assert(height > 0);
		m_height = height;
		recreate_flag();
	}

	std::vector<glm::vec3> Flag::get_positions() {
		std::vector<glm::vec3> positions;
		for (auto particle : m_particles) {
			positions.push_back(particle.get_position());
		}
		return positions;
	}

	void Flag::recreate_flag() {
		//move to ther place
		m_total_mass = 15.0f;
		m_faces.clear();
		m_edges.clear();
		m_particles.clear();
		float particle_mass = m_total_mass / (m_height * m_width);
		glm::vec3 position = glm::vec3(-1.0f, 2.0f, 0.0f);
		float delta_x = 2.0f / (m_width - 1);
		float delta_y = 4.0f / (m_height - 1);
		for (unsigned int j = 0; j < m_height; ++j) {
			for (unsigned int i = 0; i < m_width; ++i) {
				Particle p(position, particle_mass);
				position.x += delta_x;
				m_particles.push_back(p);
			}
			position.x = -1.0f;
			position.y -= delta_y;
		}
		//Create the edges
		//float k_d = 1.5f;
		//float k_s = 10.0f;
		float k_d = 0.75f;
		float k_s = 20.0f;
		//Horizonal
		for (unsigned int j = 0; j < m_height; ++j) {
			for (unsigned int i = 0; i < (m_width - 1); ++i) {
				SpringDamper edge(&m_particles[i + j * m_width], &m_particles[i + j * m_width + 1]);
				edge.set_damper_constant(k_d);
				edge.set_spring_constant(k_s);
				m_edges.push_back(edge);
			}
		}
		//Vertical
		for (unsigned int i = 0; i < m_width; ++i) {
			for (unsigned int j = 0; j < (m_height - 1); ++j) {
				SpringDamper edge(&m_particles[i + j * m_width], &m_particles[i + (j + 1) * m_width]);
				edge.set_damper_constant(k_d);
				edge.set_spring_constant(k_s);
				m_edges.push_back(edge);
			}
		}
		//Super cool diagonal springs
		for (unsigned int i = 0; i < (m_width - 1); ++i) {
			for (unsigned int j = 0; j < (m_height - 1); ++j) {
				SpringDamper edge_1(&m_particles[i + j * m_width], &m_particles[(i + 1) + (j + 1) * m_width]);
				edge_1.set_damper_constant(k_d);
				edge_1.set_spring_constant(k_s);
				m_edges.push_back(edge_1);
				SpringDamper edge_2(&m_particles[(i + 1) + j * m_width], &m_particles[i + (j + 1) * m_width]);
				edge_2.set_damper_constant(k_d);
				edge_2.set_spring_constant(k_s);
				m_edges.push_back(edge_2);
			}
		}
		//Fix the upper ones
		for (unsigned int i = 0; i < m_width; ++i) {
			m_particles[i].fix();
		}
		//Create Faces
		for (unsigned int i = 0; i < (m_width - 1); ++i) {
			for (unsigned int j = 0; j < (m_height - 1); ++j) {
				Face face(&m_particles[i + j * m_width], &m_particles[(i + 1) + j * m_width],
					&m_particles[i + (j + 1) * m_width], &m_particles[(i + 1) + (j + 1) * m_width]);
				m_faces.push_back(face);
			}
		}
		//Create windsource
		//m_wind = WindSource(glm::vec3(0.0f, 0.0f, -0.5f), 0.0075f, 0.0025f);
		m_wind = WindSource(glm::vec3(0.0f, 0.0f, -1.0f), 0.02f, 0.025f);

	}

	void Flag::integrate() {
		//Call nasty integrator here!! =)
		float dt = 0.025f;
		for (auto& particle : m_particles) {
		   if (!particle.fixed()) {
			   particle.add_velocity((dt / particle.get_mass()) * particle.get_force());
			   particle.add_position(dt * particle.get_velocity());
	       } else {
			   glm::vec3 zero;
               particle.set_force(zero);
               particle.set_velocity(zero);
           }
		}
	}

	void Flag::empty_forces() {
		glm::vec3 zero;
		for (auto& particle : m_particles) {
			particle.set_force(zero);
		}
	}

	void Flag::acumulate_forces() {
		add_gravity();
		add_spring_damper();
		add_wind();
	}

	void Flag::add_gravity() {
		for (auto& particle : m_particles) {
			glm::vec3 down = glm::vec3(0.0f, -1.0f, 0.0f);
			float G = 0.2f;
			particle.add_force(G * down);
		}
	}

	void Flag::add_spring_damper() {
		for (auto& spring : m_edges) {
			spring.add_force();
		}
	}

	void Flag::add_wind() {
		m_wind.update_force();
		for (auto& face : m_faces) {
			face.add_force(m_wind);
		}
	}

	void Flag::update() {
		empty_forces();
		acumulate_forces();
		/*for (auto particle : m_particles) {
			std::cout << "Position: "<< particle.get_position().x << ", " << particle.get_position().y << std::endl;
			std::cout << "Velocity: "<< particle.get_velocity().x << ", " << particle.get_velocity().y << std::endl;
			std::cout << "Force: "<< particle.get_force().x << ", " << particle.get_force().y << std::endl;
		}*/
		integrate();
	}

	void Flag::reset_flag() {
		glm::vec3 position = glm::vec3(-1.0f, 2.0f, 0.0f);
		glm::vec3 zero;
		float delta_x = 2.0f / (m_width - 1);
		float delta_y = 4.0f / (m_height - 1);
		for (unsigned int j = 0; j < m_height; ++j) {
			for (unsigned int i = 0; i < m_width; ++i) {
				m_particles[i + j * m_width].set_position(position);
				m_particles[i + j * m_width].set_velocity(zero);
				m_particles[i + j * m_width].set_force(zero);
			}
			position.x = -1.0f;
			position.y -= delta_y;
		}
	}

	std::vector<std::pair<glm::vec3, glm::vec3>> Flag::get_edges() {
		std::vector<std::pair<glm::vec3, glm::vec3>> edges_pos;
		for (auto edge : m_edges) {
			edges_pos.push_back(edge.get_ends_positions());
		}
		return edges_pos;
	}

	std::ostream& operator<<(std::ostream& osObject, Flag& flag) {
		osObject << "Particles: " << std::endl;
		osObject.unsetf( std::ios::floatfield );                // floatfield not set
		osObject.precision(3);
		for (unsigned int j = 0; j < flag.m_height; ++j) {
			for (unsigned int i = 0; i < flag.m_width; ++i) {
				osObject << "[" <<  i << ", " << j << "]: Position: "<< "(" 
				<< flag.m_particles[i + j * flag.m_width].get_position().x << ", "
				<< flag.m_particles[i + j * flag.m_width].get_position().y << ", "
				<< flag.m_particles[i + j * flag.m_width].get_position().z << ") Velocity: "
				<< flag.m_particles[i + j * flag.m_width].get_velocity().x << ", "
				<< flag.m_particles[i + j * flag.m_width].get_velocity().y << ", "
				<< flag.m_particles[i + j * flag.m_width].get_velocity().z << ") Force: "
				<< flag.m_particles[i + j * flag.m_width].get_force().x << ", "
				<< flag.m_particles[i + j * flag.m_width].get_force().y << ", "
				<< flag.m_particles[i + j * flag.m_width].get_force().z << ")" << std::endl;
			}
		}
		return osObject;
	}

	unsigned int Flag::get_width() {
		return m_width;
	}

	unsigned int Flag::get_height() {
		return m_height;
	}
}
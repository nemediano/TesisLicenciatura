#ifndef FLAG_H_
#define FLAG_H_
#include <vector>
#include <glm/glm.hpp>
#include <ostream>
#include <utility>

#include "../physics/Particle.h"
#include "../physics/SpringDamper.h"
#include "../physics/Face.h"

using namespace physics;

namespace scene {
	class Flag 	{
	public:
		Flag ();
		Flag (unsigned int width, unsigned int height);
		Flag (unsigned int minimun_particles);
		~Flag ();
		void set_width(unsigned int width);
		void set_height(unsigned int height);
		//void set_texture();
		unsigned int get_width();
		unsigned int get_height();
		//void set_material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
		std::vector<glm::vec3> get_positions();
		std::vector<std::pair<glm::vec3, glm::vec3>> get_edges();
		void update();
		void reset_flag();
		friend std::ostream& operator<<(std::ostream& osObject, Flag& flag);
		
	private:
		std::vector<Particle> m_particles;
		std::vector<SpringDamper> m_edges;
		std::vector<Face> m_faces;
		unsigned int m_width;
		unsigned int m_height;
		void recreate_flag();
		void acumulate_forces();
		void integrate();
		void empty_forces();
		void add_gravity();
		void add_spring_damper();
		void add_wind();
		float m_total_mass;
		WindSource m_wind;
	};
}
#endif

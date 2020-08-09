#ifndef CYLINDER
#define CYLINDER

#include <glm/glm.hpp>

#include "SceneObject.h"


class Cylinder : public SceneObject
{

	private:
		glm::vec3 middle;
		float rad = 1;
		float height = 1;

	public:	
		Cylinder() : middle(glm::vec3(0)), rad(1), height(1) {};
		
		Cylinder(glm::vec3 c, float r, float h) : middle(c), rad(r), height(h) {};

		float intersect(glm::vec3 pos, glm::vec3 dir);

		glm::vec3 normal(glm::vec3 p);

};

#endif

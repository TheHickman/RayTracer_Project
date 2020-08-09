#ifndef CONE
#define CONE

#include <glm/glm.hpp>

#include "SceneObject.h"

class Cone : public SceneObject
{
	private:
		glm::vec3 middle;
		float rad = 1;
		float height = 1;
		
	public:
		Cone() : middle(glm::vec3(0)), rad(1), height(1) {};
		
		Cone(glm::vec3 c, float r, float h) : middle(c), rad(r), height(h) {};
		
		float intersect(glm::vec3 pos, glm::vec3 dir);
		
		glm::vec3 normal(glm::vec3 p);
};

#endif

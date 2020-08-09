#include "Cylinder.h"
#include <math.h>

float Cylinder::intersect(glm::vec3 pos, glm::vec3 dir)
{
	float a = (dir.x * dir.x) + (dir.z * dir.z);
	float b = 2 * (dir.x * (pos.x-middle.x) + dir.z * (pos.z-middle.z));
	float c = (pos.x - middle.x) * (pos.x - middle.x) + (pos.z - middle.z) * (pos.z - middle.z) - (rad*rad);
	
	float delta = b*b - 4*(a*c);
	

	
	if (delta < 0.0 || (fabs(delta)) < 0.001) {
		return -1.0;
	}
	
	float t1 = (-b - sqrt(delta))/(2*a);
	float t2 = (-b + sqrt(delta))/(2*a);
	float t;
	
	if (t1 <= t2) {
		t = t1;
	}
	else {
		t = t2;
	}
	
	
	float r = pos.y + t*dir.y;
	
	if (r >= middle.y && r <= middle.y + height) {
		return t;
	}
	
	else {
		return -1;
	}
		
}

glm::vec3 Cylinder::normal(glm::vec3 p) 
{
	glm::vec3 n = glm::vec3(p.x-middle.x, 0, p.z-middle.z);
	n = glm::normalize(n);
	return n;
}

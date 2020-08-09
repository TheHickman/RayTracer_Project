#include "Cone.h"
#include <math.h>

float Cone::intersect(glm::vec3 pos, glm::vec3 dir)
{
	float a = pos.x - middle.x;
	float b = pos.z - middle.z;
	float c = height - pos.y + middle.y;
	
	float tan = (rad/height)*(rad/height);
	
	float a2 = (dir.x * dir.x) + (dir.z * dir.z) - (tan*(dir.y * dir.y));
	float b2 = (2*a*dir.x) + (2*b*dir.z) + (2*tan*c*dir.y);
	float c2 = (a*a) + (b*b) - (tan*(c*c));
	
	float delta = b2*b2 - 4*(a2*c2);
	
	if (delta < 0.0 || fabs(delta) < 0.001) {
		return -1;
	}
	
	float t1 = (-b2 - sqrt(delta))/(2*a2);
	float t2 = (-b2 + sqrt(delta))/(2*a2);
	float t = 0;
	
	if (t1 <= t2) {
		t = t1;
	}
	else {
		t = t2;
	}
	
	float r = pos.y + t*dir.y;
	
	if ((r > middle.y) && (r < middle.y + height)) {
		return t;
	}
	
	else {
		return -1;
	}
}

glm::vec3 Cone::normal(glm::vec3 p) {
	float r = sqrt((p.x - middle.x) * (p.x - middle.x) + (p.z - middle.z)*(p.z - middle.z));
	glm::vec3 n = glm::vec3(p.x - middle.x, r*(rad/height), p.z-middle.z);
	n = glm::normalize(n);
	return n;
}
	

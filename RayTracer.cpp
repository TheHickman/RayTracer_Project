/*==================================================================================
* COSC 363  Computer Graphics (2020)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab07.pdf, Lab08.pdf for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"

using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float ETA = 1.1;

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	const float trans1 = 0.3;
	const float trans2 = 0.4;
	glm::vec3 backgroundCol(1);						//Background colour = (0,0,0)
	glm::vec3 lightPos(10, 40, -3);					//Light's position
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) {
		return backgroundCol;		//no intersection
	}
	
	
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found
	
	if (ray.index == 2) { //Checkered Floor
		int stripeWidth = 5;
		int ix = (ray.hit.x + 50) /stripeWidth; 
		int j =  ix % 2;
		int iz = (ray.hit.z + 200) /stripeWidth;
		int k = iz % 2;
		
       if((j == 0 && k == 0) || (j == 1 && k == 1)){
		   color = glm::vec3(0, 1, 0);}
	   else{
		   color = glm::vec3(1, 0, 1);}
		
		obj->setColor(color);
	}
	


	color = obj->lighting(lightPos, -ray.dir, ray.hit);						//Object's colour
	
	
	glm::vec3 lightVec = lightPos - ray.hit;
	Ray shadowRay(ray.hit, lightVec);
	shadowRay.closestPt(sceneObjects);
	float lightDist = glm::length(lightVec);
	if (shadowRay.index > -1 && shadowRay.dist < lightDist) {
		color = 0.2f * obj->getColor();
	}
	if ((shadowRay.index == 9 || shadowRay.index == 1) && shadowRay.dist < lightDist) {
		color = (1-trans2) * obj->getColor();
	}
	
	if (obj->isReflective() && step < MAX_STEPS) {
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step+1);
		color = color + (rho * reflectedColor);
	}
	
	if (ray.index == 1 && step < MAX_STEPS) {
		glm::vec3 n = obj->normal(ray.hit);
		glm::vec3 g = glm::refract(ray.dir, n, ETA);
		Ray refrRay(ray.hit, g);
		refrRay.closestPt(sceneObjects);
		if (refrRay.index == -1) {
			return color * trans1 + backgroundCol*(1-trans1);
		}
		glm::vec3 m = obj->normal(refrRay.hit);
		glm::vec3 h = glm::refract(ray.dir, -m, 1.0f/ETA);
		Ray refrRay2(refrRay.hit, h);
		refrRay2.closestPt(sceneObjects);
		if (refrRay2.index == -1) {
			return color * trans1 + backgroundCol*(1-trans1);
		}
		glm::vec3 refrRayCol = trace(refrRay2, step+1);
		color = color * trans1 + refrRayCol*(1-trans1);
		return color;
	}
	
	if (ray.index == 9 && step < MAX_STEPS) {
		glm::vec3 g = ray.dir;
		Ray refrRay(ray.hit, g);
		refrRay.closestPt(sceneObjects);
		if (refrRay.index == -1) {
			return color * trans2 + backgroundCol*(1-trans2);
		}
		glm::vec3 h = ray.dir;
		Ray refrRay2(refrRay.hit, h);
		refrRay2.closestPt(sceneObjects);
		if (refrRay2.index == -1) {
			return color * trans2 + backgroundCol*(1-trans2);
		}
		glm::vec3 refrRayCol = trace(refrRay2, step+1);
		color = color * trans2 + refrRayCol*(1-trans2);
		return color;
	}
	
	if (ray.index == 12 && step < MAX_STEPS) {
		if (ray.hit.x * ray.hit.x + ray.hit.y * ray.hit.y < 150 && ray.hit.y > 0) {
			color = glm::vec3(1, 0.2, 0);
		}
		else if (ray.hit.x * ray.hit.x + ray.hit.y * ray.hit.y < 150 && ray.hit.y < 0) {
			color = glm::vec3(0.7, 0.2, 0);
		}
		else if (ray.hit.y >= 0) {
			color = glm::vec3(fabs(cos(ray.hit.x)), fabs(sin(sqrt(ray.hit.y))), fabs(cos(sqrt(ray.hit.y))));
		}
		else {
			color = glm::vec3(0, 0, fabs(cos(ray.hit.y)));
		}
	}
		
	float t = (ray.hit.z - -40)/(-250 - -40);
	color = (1-t) * color + t*backgroundCol;
	return color;
}

glm::vec3 anti_aliasing(glm::vec3 eye, float pixel, float x_pos, float y_pos) {
	float quater = pixel/4;
	float three_quater = pixel * 0.75;
	
	glm::vec3 color(0);
	glm::vec3 average(0.25);
	
	Ray ray = Ray(eye, glm::vec3(x_pos + quater, y_pos + quater, -EDIST));
	ray.dir = glm::normalize(ray.dir);
	color = color + trace(ray, 1);
	
	ray = Ray(eye, glm::vec3(x_pos + quater, y_pos + three_quater, -EDIST));
	ray.dir = glm::normalize(ray.dir);
	color = color + trace(ray, 1);
	
	ray = Ray(eye, glm::vec3(x_pos + three_quater, y_pos + quater, -EDIST));
	ray.dir = glm::normalize(ray.dir);
	color = color + trace(ray, 1);
	
	ray = Ray(eye, glm::vec3(x_pos + three_quater, y_pos + three_quater, -EDIST));
	ray.dir = glm::normalize(ray.dir);
	color = color + trace(ray, 1);
	
	color = color * average;
	
	return color;
}

void cubeTime(float x, float y, float z, float size)
{   
	glm::vec3 corn1 = glm::vec3(x, y, z);
	glm::vec3 corn2 = glm::vec3(x+size, y, z);
	glm::vec3 corn3 = glm::vec3(x+size, y+size, z);
	glm::vec3 corn4 = glm::vec3(x, y+size, z);
	glm::vec3 corn5 = glm::vec3(x+size, y, z-size);
	glm::vec3 corn6 = glm::vec3(x+size, y+size, z-size);
	glm::vec3 corn7 = glm::vec3(x, y+size, z-size);
	glm::vec3 corn8 = glm::vec3(x, y, z-size);
	
	Plane *face1 = new Plane(corn1, corn2, corn3, corn4);
	face1->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(face1); //Index 3
	
	Plane *face2 = new Plane(corn2, corn5, corn6, corn3);
	face2->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(face2); //Index 4
	
	Plane *face3 = new Plane(corn5, corn8, corn7, corn6);
	face3->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(face3); //Index 5
	
	Plane *face4 = new Plane(corn4, corn7, corn8, corn1);
	face4->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(face4); //Index 6
	
	Plane *face5 = new Plane(corn5, corn3, corn6, corn7);
	face5->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(face5); //Index 7
	
	Plane *face6 = new Plane(corn8, corn5, corn2, corn1);
	face6->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(face6); //Index 8
		
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray
			//Ray ray = Ray(eye, dir);	
			glm::vec3 col = anti_aliasing(eye, cellX, xp, yp);
		    //glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -9.0, -120.0), 5.0);
	sphere1->setColor(glm::vec3(0, 0, 1));   //Set colour to blue
	sceneObjects.push_back(sphere1);		 //Index 0
	sphere1 -> setSpecularity(false);
	sphere1 -> setReflectivity(true, 0.8);
	
	Sphere *sphere2 = new Sphere(glm::vec3(0, -10, -70), 4);
	sphere2 ->setColor(glm::vec3(0, 1, 0));
	sceneObjects.push_back(sphere2); //Index 1
	sphere2->setShininess(5);
	
	
	Plane *plane1 = new Plane(glm::vec3(-30., -15, -40),
							glm::vec3(30., -15, -40),
							glm::vec3(30., -15, -200),   
							glm::vec3(-30., -15, -200)); 
							
	sceneObjects.push_back(plane1); //Index 2
	plane1->setSpecularity(false);
	
	cubeTime(10, -15.0, -80, 5); //Index 3-8
	
	Sphere *sphere3 = new Sphere(glm::vec3(-10.0, -3.0, -70), 6); 
	sphere3 ->setColor(glm::vec3(0.5, 0, 1));
	sceneObjects.push_back(sphere3); //Index 9
	
	Cylinder *cylinder1 = new Cylinder(glm::vec3(-10, -15, -70), 3, 7);
	cylinder1 ->setColor(glm::vec3(0, 1, 0));
	sceneObjects.push_back(cylinder1); //Index 10
	
	Cone *cone1 = new Cone(glm::vec3(12.5, -10, -82.5), 3, 12);
	cone1 ->setColor(glm::vec3(1, 1, 0));
	sceneObjects.push_back(cone1); //Index 11
	
	Plane *plane2 = new Plane(glm::vec3(-30., -15, -200),
							glm::vec3(30., -15, -200),
							glm::vec3(30., 15, -200),   
							glm::vec3(-30., 15, -200)); 
							
	sceneObjects.push_back(plane2); //Index 12
	plane1->setSpecularity(false);
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");
   

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}

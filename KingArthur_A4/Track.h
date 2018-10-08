// include the OpenGL library header
#ifdef __APPLE__				// if compiling on Mac OS
#include <OpenGL/gl.h>
#else							// if compiling on Linux or Windows OS
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>			// include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>				// for cos(), sin() functionality
#include <stdio.h>			// for printf functionality
#include <stdlib.h>			// for exit functionality
#include <time.h>			  // for time() functionality

#include <fstream>			// for file I/O
#include <iostream>
#include <sstream>
#include <string>
#include <vector>				// for vector
using namespace std;

// Calculates tangent of a given point
glm::vec3 evaluateBezierTangent(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
	glm::vec3 point(0, 0, 0);

	point = -3.0f * (1 - t) * (1 - t) * p0 + 3.0f * (1 - t) * (1 - t) * p1 - 6.0f * t * (1 - t) * p1 - 3.0f * t * t * p2 + 6.0f * t * (1 - t) * p2 + 3.0f * t * t * p3;

	return point;
}

//Draw surface
void drawTraceSurface(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution) {
	for (int i = 0; i < resolution; i++) {
		glm::vec3 point = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * i / resolution);
		glm::vec3 point2 = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * (i + 1) / resolution);
		glm::vec3 align = point2 - point;
		glm::vec3 tangent = evaluateBezierTangent(p0, p1, p2, p3, 1.0f * i / resolution);
		tangent = glm::normalize(tangent) + point;
		glm::vec3 rotationVector = glm::cross(tangent, align);
		float theta = acos(glm::dot(align, tangent) / (align.length()));

		glm::mat4 transMtx = glm::translate(glm::mat4(), point);
		glMultMatrixf(&transMtx[0][0]);

		glm::mat4 rotMtx = glm::rotate(glm::mat4(), theta, rotationVector);
		glMultMatrixf(&rotMtx[0][0]);

		glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, .1, .1));
		glMultMatrixf(&scaleMtx[0][0]);

		CSCI441::drawSolidCube(1);

		glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}
}
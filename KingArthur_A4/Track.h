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

int rainbowMax = 25;
int rainbowRed = rainbowMax;
int rainbowGreen = 0;
int rainbowBlue = 0;
float rainbowAlphaValue = 0.2;
void drawControlPoints(vector<glm::vec3> controlPoints, int resolution);
void drawBezierCurve(vector<glm::vec3> controlPoints, int resolution);
bool testing = true;

/*NOTE:
 * Before or after each FRAME, set rainbowRed to max and rainbowGreen/rainbowBlue to 0.
 * The functions here can not do this or each curve will restart the color set.
 */

 // evaluateBezierCurve() ////////////////////////////////////////////////////////
 //
 // Computes a location along a Bezier Curve.
 //
 ////////////////////////////////////////////////////////////////////////////////
glm::vec3 evaluateBezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
	glm::vec3 point(0, 0, 0);

	point = p0 * (1 - t) * (1 - t) * (1 - t) + 3.0f * p1 * t * (1 - t) * (1 - t) + 3.0f * p2 * t * t * (1 - t) + p3 * t * t * t;

	return point;
}

// Calculates tangent of a given point
glm::vec3 evaluateBezierTangent(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
	glm::vec3 point(0, 0, 0);

	point = -3.0f * (1 - t) * (1 - t) * p0 + 3.0f * (1 - t) * (1 - t) * p1 - 6.0f * t * (1 - t) * p1 - 3.0f * t * t * p2 + 6.0f * t * (1 - t) * p2 + 3.0f * t * t * p3;

	return point;
}

//Set rainbow color gradient
void rainbowUpdateColor() {
	if (rainbowRed == rainbowMax) {
		if (rainbowBlue > 0) rainbowBlue -= 1;
		else if (rainbowGreen < rainbowMax) rainbowGreen += 1;
		else rainbowRed -= 1;
	}
	else if (rainbowGreen == rainbowMax) {
		if (rainbowRed > 0) rainbowRed -= 1;
		else if (rainbowBlue < rainbowMax) rainbowBlue += 1;
		else rainbowGreen -= 1;
	}
	else if (rainbowBlue == rainbowMax) {
		if (rainbowGreen > 0) rainbowGreen -= 1;
		else if (rainbowRed < rainbowMax) rainbowRed += 1;
		else rainbowBlue -= 1;
	}
	glColor4f(1.0f * rainbowRed / rainbowMax, 1.0f * rainbowGreen / rainbowMax, 1.0f * rainbowBlue / rainbowMax, rainbowAlphaValue);
}

//Reset color
//CALL ONCE PER FRAME if not statically defined
void resetColor() {
	rainbowRed = rainbowMax;
	rainbowBlue = 0;
	rainbowGreen = 0;
}

//Draw surface
void drawTraceSurface(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution) {
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if( testing ) {
		vector<glm::vec3> set;
		(set).push_back(p0);
		(set).push_back(p1);
		(set).push_back(p2);
		(set).push_back(p3);
		drawControlPoints(set, resolution);
		drawBezierCurve(set, resolution);
	}
	for (int i = 0; i < resolution; i++) {
		rainbowUpdateColor();
		glm::vec3 point = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * i / resolution);
		glm::vec3 point2 = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * (i + 1) / resolution);
		glm::vec3 align = point2 - point;
		glm::vec3 tangent = evaluateBezierTangent(p0, p1, p2, p3, 1.0f * i / resolution);
		tangent = glm::normalize(tangent + point);
		glm::vec3 rotationVector = glm::cross(tangent, align);
		float theta = acos(glm::dot(align, tangent) / (align.length()));

		glm::mat4 transMtx = glm::translate(glm::mat4(), point);
		glMultMatrixf(&transMtx[0][0]);

		glm::mat4 rotMtx = glm::rotate(glm::mat4(), theta, rotationVector);
		glMultMatrixf(&rotMtx[0][0]);

		glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(10, 1.5, 1.5));
		glMultMatrixf(&scaleMtx[0][0]);

		CSCI441::drawSolidCube(1);

		glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}
	glDisable( GL_BLEND );
}

float getRand2() { return rand() / (float)RAND_MAX; }

// old possible repeat code
void drawControlPoints(vector<glm::vec3> controlPoints, int resolution){

	GLfloat sphereSize = 2.0;
	// I'm just doubling code because I don't have a ton of time
	for (glm::vec3 elem : controlPoints ){
		glm::mat4 moveMtx = glm::translate(glm::mat4(), elem);
		//glPushMatrix();
		glColor3f( 0.0f, 1.0f, 0.0f );
		glColor3f( getRand2(), getRand2(), getRand2() );
		glMultMatrixf(&moveMtx[0][0]);{
			CSCI441::drawSolidSphere(sphereSize,GLint(10), GLint(100));
		} glMultMatrixf( &(glm::inverse( moveMtx )[0][0] ));
		//glPopMatrix();
	}
	glLineWidth(3.0f);
	glBegin(GL_LINE_STRIP);{
		glColor3f(1.0f,1.0f,0.5f);
		for (glm::vec3 elem : controlPoints) {
			glVertex3f(elem.x, elem.y, elem.z);
		}
	} glEnd();
}
void renderBezierCurve2( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution ) {
    // TODO #05: Draw the Bezier Curve!
	float t = 0.0;
	float timing = 1.0 / resolution;
	glColor3f(0.0f,0.0f,1.0f);

	glBegin(GL_LINE_STRIP);{
		for (t = 0.0; t < 1.0; t += timing){
			glm::vec3 point = evaluateBezierCurve(p0,p1,p2,p3,t);
			glVertex3f( point.x, point.y, point.z );
		}
	} glEnd();
	//glLineWidth(30.0f);
}
void drawBezierCurve(vector<glm::vec3> controlPoints, int resolution){

	//glLineWidth(3.0f);
	for (int i = 0; i < (controlPoints).size() - 1; i+=3) {
		renderBezierCurve2((controlPoints)[i],
						(controlPoints)[i+1],
						(controlPoints)[i+2],
						(controlPoints)[i+3], resolution);
	}
}

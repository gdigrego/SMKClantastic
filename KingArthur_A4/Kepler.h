// include the OpenGL library header
#ifdef __APPLE__					// if compiling on Mac OS
#include <OpenGL/gl.h>
#else										// if compiling on Linux or Windows OS
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>	// include GLFW framework header

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

class DarkSlayer {
private:

	float vehicleHeading;           					// Directional facing angle of vehicle
	glm::vec3 vehicleDir;								// Forward vector for vehicle
	float wheelAngle;									// Animates wheel rotation

	//Recalculates the direction vector of the vehicle
	void calculateFacing() {
		float x = sin(vehicleHeading);
		float y = 0;
		float z = cos(vehicleHeading);


		float d = sqrt(x * x + y * y + z * z);
		vehicleDir = glm::vec3(x / d, y / d, z / d);
	}

	//Draw the tire rims
	void drawRim() {
		glColor3f(.5, .5, .5);
		CSCI441::drawSolidCone(1, .25, 10, 10);

		//Draw spokes
		glColor3f(1, 0, 0);
		for (int i = 0; i < 5; i++) {
			glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), glm::radians(72.0f * i), glm::vec3(0, 1, 0));
			glMultMatrixf(&rotMtx1[0][0]);
			glm::mat4 transMtx1 = glm::translate(glm::mat4(), glm::vec3(0, 0, -.5));
			glMultMatrixf(&transMtx1[0][0]);
			glm::mat4 rotMtx = glm::rotate(glm::mat4(), glm::radians(15.0f), glm::vec3(1, 0, 0));
			glMultMatrixf(&rotMtx[0][0]);
			glm::mat4 transMtx2 = glm::translate(glm::mat4(), glm::vec3(0, .37, 1));
			glMultMatrixf(&transMtx2[0][0]);


			glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(.2, .05, 1));
			glMultMatrixf(&scaleMtx[0][0]);

			CSCI441::drawSolidCube(1);

			glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);

			glMultMatrixf(&(glm::inverse(transMtx2))[0][0]);
			glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
			glMultMatrixf(&(glm::inverse(transMtx1))[0][0]);
			glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		}
	}

	//Draw one wheel
	void drawWheel() {
		glm::mat4 rotMtx = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(0, 1, 0));
		glMultMatrixf(&rotMtx[0][0]);
		//Draw the tire
		glColor3f(0, 0, 0);
		CSCI441::drawSolidTorus(.5, 1, 10, 25);

		//Draw rims
		glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, .4, 0));
		glMultMatrixf(&transMtx[0][0]);
		drawRim();
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);

		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(0, .4, 0));
		glMultMatrixf(&transMtx[0][0]);
		drawRim();
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);

		//Draw axle
		transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, -1));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	}

	//Draw connections to rear tire
	void drawRearConnectors() {
		//Draw central connecting rod
		glColor3f(.3, .1, .1);
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(0, 0, 1));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw endcaps
		glColor3f(.6, .1, .1);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw connecting pieces
		glColor3f(.7, .3, .3);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw rear endcaps
		glColor3f(.9, .1, .1);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, -2));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, -2));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}

	//Draw handlebars/front connectors
	void drawFrontConnectors() {
		//Draw central connecting rod
		glColor3f(.3, .1, .1);
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(0, 0, 1));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw endcaps
		glColor3f(.6, .1, .1);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		glColor3f(.9, .1, .1);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 2));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, 2));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidSphere(.15, 10, 10);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw handlebars
		glColor3f(.7, .6, .6);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-2, 2, 1));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(0, 0, 1));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 4, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw hand grips
		glColor3f(.1, 0, 0);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-2.1, 2, 1));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(0, 0, 1));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.12, .12, .5, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-2.1, 2, 1));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(0, 1, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidDisk(0, .15, 25, 25);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		transMtx = glm::translate(glm::mat4(), glm::vec3(2.1, 2, 1));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(0, 0, 1));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.12, .12, .5, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(2.1, 2, 1));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(0, 1, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidDisk(0, .15, 25, 25);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Add body connectors to handlebars
		glColor3f(.7, .3, .3);
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(26.565f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2.25, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, 0));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(26.565f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2.25, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Add tire connections to handlebars
		transMtx = glm::translate(glm::mat4(), glm::vec3(-1, 0, 2));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-26.565f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2.25, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(1, 0, 2));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-26.565f), glm::vec3(1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		CSCI441::drawSolidCylinder(.1, .1, 2.25, 10, 10);
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}

	//Draw the chest rest/forward piece
	void drawChestPiece() {
		glColor3f(0, 0, 0);
		glBegin(GL_TRIANGLES);

		//Draw sides
		glVertex3f(.5, 0, 0);
		glVertex3f(.5, 0, -5.0 / 3);
		glVertex3f(.5, 1, 0);
		glVertex3f(-.5, 0, 0);
		glVertex3f(-.5, 0, -5.0 / 3);
		glVertex3f(-.5, 1, 0);

		//Draw front
		glVertex3f(.5, 0, 0);
		glVertex3f(-.5, 0, 0);
		glVertex3f(.5, 1, 0);
		glVertex3f(-.5, 1, 0);
		glVertex3f(-.5, 0, 0);
		glVertex3f(.5, 1, 0);

		//Draw top
		glVertex3f(.5, 1, 0);
		glVertex3f(.5, 0, -5.0 / 3);
		glVertex3f(-.5, 1, 0);
		glVertex3f(.5, 0, -5.0 / 3);
		glVertex3f(-.5, 0, -5.0 / 3);
		glVertex3f(-.5, 1, 0);

		glEnd();
	}

	//Draw the block that lies between the legs
	void drawLegPiece() {
		glColor3f(0, 0, 0);
		//Draw back part
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, .5, -.5));
		glMultMatrixf(&transMtx[0][0]);
		CSCI441::drawSolidCube(1);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw front part of block
		glBegin(GL_TRIANGLES);

		//Draw top
		glVertex3f(.5, 1, 0);
		glVertex3f(.25, 1, 0);
		glVertex3f(.25, 1, .5);
		glVertex3f(-.5, 1, 0);
		glVertex3f(-.25, 1, 0);
		glVertex3f(-.25, 1, .5);

		glVertex3f(-.25, 1, 0);
		glVertex3f(.25, 1, 0);
		glVertex3f(.25, 1, .5);
		glVertex3f(-.25, 1, 0);
		glVertex3f(-.25, 1, .5);
		glVertex3f(.25, 1, .5);

		//Draw front
		glVertex3f(.25, 0, .5);
		glVertex3f(-.25, 1, .5);
		glVertex3f(.25, 1, .5);
		glVertex3f(.25, 0, .5);
		glVertex3f(-.25, 1, .5);
		glVertex3f(-.25, 0, .5);

		//Draw angled sides
		glVertex3f(.5, 0, 0);
		glVertex3f(.5, 1, 0);
		glVertex3f(.25, 0, .5);
		glVertex3f(.25, 1, .5);
		glVertex3f(.5, 1, 0);
		glVertex3f(.25, 0, .5);

		glVertex3f(-.5, 0, 0);
		glVertex3f(-.5, 1, 0);
		glVertex3f(-.25, 0, .5);
		glVertex3f(-.25, 1, .5);
		glVertex3f(-.5, 1, 0);
		glVertex3f(-.25, 0, .5);

		//Draw fins
		glColor3f(.3, 0, 0);
		glVertex3f(.5, 0, -1);
		glVertex3f(.5, 1, -1);
		glVertex3f(.75, 1.5, -3);

		glVertex3f(-.5, 0, -1);
		glVertex3f(-.5, 1, -1);
		glVertex3f(-.75, 1.5, -3);

		glEnd();
	}

	//Draw body
	void drawBody() {
		//Draw central frame
		glColor3f(0, 0, 0);
		glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 1, 5));
		glMultMatrixf(&scaleMtx[0][0]);
		CSCI441::drawSolidCube(1);
		glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);

		//Draw chest piece
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, .5, 2.5));
		glMultMatrixf(&transMtx[0][0]);
		drawChestPiece();
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw leg block
		transMtx = glm::translate(glm::mat4(), glm::vec3(0, .5, -1.5));
		glMultMatrixf(&transMtx[0][0]);
		drawLegPiece();
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw connector to rear tire
		transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, -2));
		glMultMatrixf(&transMtx[0][0]);
		drawRearConnectors();
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//Draw handlebars
		transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, 2));
		glMultMatrixf(&transMtx[0][0]);
		drawFrontConnectors();
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

	}

public:

	//Draw the vehicle
	void draw() {

		//Draw the frame body
		drawBody();

		//Draw wheels
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, 4));
		glMultMatrixf(&transMtx[0][0]);
		glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), -wheelAngle, glm::vec3(-1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		drawWheel();
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, -4));
		glMultMatrixf(&transMtx[0][0]);
		rotMtx1 = glm::rotate(glm::mat4(), -wheelAngle, glm::vec3(-1, 0, 0));
		glMultMatrixf(&rotMtx1[0][0]);
		drawWheel();
		glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}

	void draw(bool mvmt) {
		draw();
		if (mvmt) wheelAngle += (2.0 / 3) * .25;
	}

	DarkSlayer() {
		vehicleHeading = 0;
		vehicleDir = glm::vec3(1, 0, 0);
		wheelAngle = 0;
	}
};
/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: SMKClantastic
 *  File: SolaireCar.h
 *
 *	Author: Gino Di Gregorio - Fall 2018
 *
 *  Description:
 *      Contains the base code for Solaire of Astora's car drawing.
 *
 */

// HEADERS /////////////////////////////////////////////////////////////////////

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
#include <vector>				// for vector
#include <iostream>
using namespace std;

glm::vec3 carPosition;
float carTheta, carThetaDeg;
glm::vec3 carDirection;	

float carMovement = 0;
bool carMovementUp = false;
bool carMovementDown = true;

class SolaireCar{
    private:
        
    public:
        // draw stuff
        void drawCar();
        void animateCar();
};

void drawCar() {
	// disable lighting for use with primitives
	glDisable( GL_LIGHTING );

	glm::mat4 transMtx1 = glm::translate(glm::mat4(), glm::vec3(-6.25, 2.5, -2.5));
	glMultMatrixf( &transMtx1[0][0] ); 

	// draw body of car
	glBegin(GL_TRIANGLES);{
		glColor3f(1, 0, 0);

		// right side of car
		glVertex3f(0, 0, 0);
		glVertex3f(12.5, 0, 0);
		glVertex3f(0, 5, 0);

		// left side of car
		glVertex3f(0, 0, 5);
		glVertex3f(12.5, 0, 5);
		glVertex3f(0, 5, 5);

		// bottom of car
		glVertex3f(0, 0, 0);
		glVertex3f(12.5, 0, 0);
		glVertex3f(0, 0, 5);

		glVertex3f(12.5, 0, 0);
		glVertex3f(0, 0, 5);
		glVertex3f(12.5, 0, 5);

		glColor3f(.5, 0, .5);

		// top panel of car
		glVertex3f(12.5, 0, 5);
		glVertex3f(0, 5, 5);
		glVertex3f(0, 5, 0);

		glVertex3f(0, 5, 0);
		glVertex3f(12.5, 0, 5);
		glVertex3f(12.5, 0, 0);

		// back panel of car
		glVertex3f(0, 0, 0);
		glVertex3f(0, 5, 0);
		glVertex3f(0, 0, 5);

		glVertex3f(0, 5, 0);
		glVertex3f(0, 0, 5);
		glVertex3f(0, 5, 5);
	
	};glEnd();

	glBegin(GL_LINES); {
		glColor3f(1.0, 0, 0);

		glVertex3f(0, 5, 0);
		glVertex3f(0, 5, 5);
	};glEnd();

	glMultMatrixf( &( glm::inverse( transMtx1 ) )[0][0] );

	// re-enable lighting
	glEnable( GL_LIGHTING );
}

void animateCar() {
	if( carMovementUp ) {
		carMovement += .005;
		if( carMovement >= .5 ) {
			carMovementUp = false;
			carMovementDown = true;
		}
	} else if ( carMovementDown ) {
		carMovement -= .005;
		if( carMovement <= -.5 ) {
			carMovementUp = true;
			carMovementDown = false;
		}
	}
}
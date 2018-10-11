/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: SMK
 *  File: Tree.h
 *
 *	Author: Robert Imbler - Fall 2018
 *
 *  Description:
 *      Contains the base code for Hero drawing.
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
#include "Drawable.h"
using namespace std;

float trunk[3] = {0.5f,0.5f,0.0f};
float leave[3] = {0.0f,0.8f,0.1f};

float getRandy() { return rand() / (float)RAND_MAX; }

int coin(){if (getRandy() <= 0.5) {return 0;} else {return 1;}} 

class PineTree : public Drawable{
    public:
        // draw stuff
        void draw();
    PineTree(glm::mat4 p, glm::mat4 o, glm::mat4 s){
        position = p;
        orientation = o;
        scale = s;
    }
};

void PineTree::draw() {
    glColor3f( trunk[0], trunk[1], trunk[2] );
    CSCI441::drawSolidCylinder(10,10,10,10,10);
    glm::mat4 branchLocation = glm::translate(glm::mat4(),
        glm::vec3(0.0f, 10.0f, 0.0f));
    glMultMatrixf( &branchLocation[0][0] ); {
        glColor3f( leave[0], leave[1], leave[2] );
        if (coin()){
            CSCI441::drawSolidCone(20,20,20,10);
        }
        else{
            CSCI441::drawSolidSphere(20,20,20);
        }
        
    } glMultMatrixf( &(glm::inverse( branchLocation )[0][0]));
}
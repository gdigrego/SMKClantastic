/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: A04
 *  File: Hero.h
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
using namespace std;
const int HEAD_RADIUS = 10;
const int LEG_WIDTH = 10;
const int BODY_WIDTH = 20;
const int SHOE_LENGTH = 15;
const int LEG_MOVEMENT = 10;
const int HEAD_BOB = 5;
int legPosition = 10;
bool legUp = true;
float headPosition = 0.0;
bool headUp = false;
float headSpeed = 0.2;
double legColor[3] = {0.9,0.1,0.1};
double shoeColor[3] = {0.1,0.9,0.1};
double bodyColor[3] = {0.4,0.4,0.1};
double headColor[3] = {0.7,0.7,0.7};
void drawHead();
void drawBody();
void drawLeg(glm::mat4 position);
class Ollie{
    private:
        int hit_points;
        int stamina;
    public:
        // draw stuff
        void draw();
        void draw(bool movement);
};
// Draw Hero on screen, with or without movement
void Ollie::draw(){
    this->draw(false);
}
void Ollie::draw(bool movement){
    if (movement){
        // manage dat rotation of feet
        if (legPosition >= LEG_MOVEMENT){
            legUp = false;
        }
        else if (legPosition <= 0){
            legUp = true;
        }
        if (legUp){
            legPosition++;
        }
        else {
            legPosition--;
        }
    }
    if (headPosition >= HEAD_BOB){
        headUp = false;
    }
    else if (headPosition <= 0){
        headUp = true;
    }
    if (headUp){
        headPosition += headSpeed;
    }
    else {
        headPosition -= headSpeed;
    }

    glm::mat4 rLegLocation = glm::translate(glm::mat4(), 
        glm::vec3(-10.0f,20.0f - float(legPosition),0.0f));
    glm::mat4 lLegLocation = glm::translate(glm::mat4(), 
        glm::vec3(10.0f,float(legPosition)+10.0f,0.0f));
    
    // draw body and scale
    drawBody();
    // draw head
    drawHead();
    // draw appendages
    drawLeg(rLegLocation);
    drawLeg(lLegLocation);
}
void drawHead(){
    glm::mat4 headLocation = glm::translate(glm::mat4(), 
        glm::vec3(0.0f,50.0f+headPosition,0.0f));
    glMultMatrixf( &headLocation[0][0] );{
        glColor3f(headColor[0],headColor[1],headColor[2]);
        CSCI441::drawSolidSphere(GLdouble(HEAD_RADIUS), GLint(10), GLint(10));
    } glMultMatrixf( &(glm::inverse( headLocation ))[0][0]);
}
void drawBody(){
    glm::mat4 bodyLocation = glm::translate(glm::mat4(), glm::vec3(0.0f,30.0f,0.0f));
    // make body taller
    glm::mat4 bodyScale = glm::scale(glm::mat4(), glm::vec3(1.5f, 1.5f, 1.0f));
    glMultMatrixf( &bodyLocation[0][0] );{
        glMultMatrixf( &bodyScale[0][0]);{
            glColor3f(bodyColor[0],bodyColor[1],bodyColor[2]);
            CSCI441::drawSolidCube(GLdouble(BODY_WIDTH));
        }glMultMatrixf(&(glm::inverse(bodyScale)[0][0]));
    } glMultMatrixf( &(glm::inverse( bodyLocation ))[0][0]);
}   

void drawLeg(glm::mat4 position){
    glm::mat4 shoeOffset = glm::translate(glm::mat4(),
        glm::vec3(0.0f, -35.0f, 2.0f));
    
    // make legs taller and feer longer
    glm::mat4 legScale = glm::scale(glm::mat4(), glm::vec3(0.5f, 2.0f, 1.0f));
    glm::mat4 footScale = glm::scale(glm::mat4(), glm::vec3(0.8f, 0.25f, 1.5f));
    // draw right appendage
    glMultMatrixf( &position[0][0] );{
        //draw leg
        glMultMatrixf( &legScale[0][0]);{
            glColor3f(legColor[0],legColor[1],legColor[2]);
            CSCI441::drawSolidCube(GLdouble(LEG_WIDTH));
        } glMultMatrixf( &(glm::inverse( legScale))[0][0]);
        //draw shoe
        glMultMatrixf( &footScale[0][0]);{
            glMultMatrixf( &shoeOffset[0][0]);{
                glColor3f(shoeColor[0],shoeColor[1],shoeColor[2]);
                CSCI441::drawSolidCube(GLdouble(SHOE_LENGTH));
            } glMultMatrixf( &(glm::inverse( shoeOffset))[0][0]);
        } glMultMatrixf( &(glm::inverse(footScale))[0][0]);
    } glMultMatrixf( &(glm::inverse( position ))[0][0]);
}
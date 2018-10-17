/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: SMK
 *  File: Hero.h
 *
 *	Author: Robert Imbler - Fall 2018
 *
 *  Description:
 *      Contains the base code for Hero drawing.
 *
 */

// HEADERS /////////////////////////////////////////////////////////////////////
#pragma once

// include the OpenGL library header
#ifdef __APPLE__				// if compiling on Mac OS
#include <OpenGL/gl.h>
#else							// if compiling on Linux or Windows OS
#include <GL/gl.h>
#endif

// include GLM libraries and matrix functions
#include <GLFW/glfw3.h>			// include GLFW framework header
#include <glm/glm.hpp>

class Hero {
    public:
        glm::vec3 location;
        virtual void draw();
        virtual void draw(bool movement);
};
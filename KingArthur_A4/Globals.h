#pragma once

// All OpenGL stuff
#include "Batch_Includes.h"



//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
static int windowWidth = 1280, windowHeight = 960;
static float PHI_MAX = M_PI;

static GLuint grassTexHandle, skyTexHandle;
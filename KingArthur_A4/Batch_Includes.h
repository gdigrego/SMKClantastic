#ifdef __APPLE__					// if compiling on Mac OS
	#include <OpenGL/gl.h>
#else								// if compiling on Linux or Windows OS
	#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>	// include GLFW framework header
#include <SOIL/SOIL.h>

#include <CSCI441/objects.hpp> // for our 3D objects
#include <CSCI441/OpenGLUtils.hpp>		// for OpenGL helpers
#include <CSCI441/TextureUtils.hpp>		// for Texture helpers

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <math.h>				// for cos(), sin() functionality
#include <stdio.h>			// for printf functionality
#include <stdlib.h>			// for exit functionality
#include <time.h>			  // for time() functionality
#include <SOIL/SOIL.h>



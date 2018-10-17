#include "Globals.h"

#include "Camera.h"

using namespace std;

//*************************************************************************************
//
// Event Callbacks

//
//	void error_callback( int error, const char* description )
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
static void error_callback( int error, const char* description ) {
	fprintf( stderr, "[ERROR]: %s\n", description );
}

bool keyisdown (int keystate) {
	return (keystate == GLFW_PRESS || keystate == GLFW_REPEAT);
}

static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
	if( action == GLFW_PRESS ) {
		switch( key ) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				exit(EXIT_SUCCESS);
			// Key for first person cam
			case GLFW_KEY_3: {
				fpsCam = !fpsCam;
				break;
			}
		}
		
	}

	switch(key) {
		// Character motion
		case GLFW_KEY_UP:{ upState = action; break; }
		case GLFW_KEY_LEFT: { leftState = action; break; }
		case GLFW_KEY_DOWN: { downState = action; break; }
		case GLFW_KEY_RIGHT: { rightState = action; break; }
		// Camera Motion
		case GLFW_KEY_W: { Wstate = action; break; }
		case GLFW_KEY_A: { Astate = action; break; }
		case GLFW_KEY_S: { Sstate = action; break; }
		case GLFW_KEY_D: { Dstate = action; break; }
		case GLFW_KEY_Z: { Dstate = action;
			if(cameraType == 2) { camPos.y += 1; } break; }
		case GLFW_KEY_X: { Dstate = action;
			if(cameraType == 2) { camPos.y -= 1; } break; }
		case GLFW_KEY_C: { if (action == GLFW_PRESS) hideCage = !hideCage; break; }
		case GLFW_KEY_T:
			TESTING();
			break;
		case GLFW_KEY_B: {
			if (action == GLFW_PRESS) hideBezierCurve = !hideBezierCurve; break; }
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL: { CtrlState = action; break; }
		// Key for arcball cam
		case GLFW_KEY_1: { cameraType = 1; break; }
		// Key for freecam
		case GLFW_KEY_2: { cameraType = 2; break; }
		// Key for hero OLLIE
		case GLFW_KEY_4: { heroChoice = 4; break; }
		// Key for hero DarkSlayer
		case GLFW_KEY_5: { heroChoice = 5; break; }
		// Key for hero KING ARTHUR
		case GLFW_KEY_6: { heroChoice = 6; break; }
	}

}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//  GLFW callback for mouse movement. We update cameraPhi and/or cameraTheta
//      based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback( GLFWwindow *window, double x, double y ) {
	if(cameraType == 2) {
		if (leftMouseButton == GLFW_PRESS && keyisdown(CtrlState)) {
			arcballDistance += (y - mousePos.y)*(0.005);
		}
		else if( leftMouseButton == GLFW_PRESS ) {
			cameraTheta += (x - mousePos.x)*(0.005);
			cameraPhi += (mousePos.y - y)*(0.005);
			if (cameraPhi <= 0)
				cameraPhi = 0 + 0.001;
			if (cameraPhi >= M_PI)
				cameraPhi = M_PI - 0.001;
			recomputeOrientation();     // update camera direction based on (theta,phi)
		}
	} else if(cameraType == 1) {
		if( leftMouseButton == GLFW_PRESS ) {
			ollieCamTheta += (x - mousePos.x)*(0.005);
			ollieCamPhi += (mousePos.y - y)*(0.005);
			if (ollieCamPhi <= 0)
				ollieCamPhi = 0 + 0.001;
			if (ollieCamPhi >= M_PI)
				ollieCamPhi = M_PI - 0.001;
			recomputeOllieCamOrientation();     // update camera direction based on (theta,phi)
		}
	}
	mousePos.x = x;
	mousePos.y = y;
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//  GLFW callback for mouse clicks. We save the state of the mouse button
//      when this is called so that we can check the status of the mouse
//      buttons inside the motion callback (whether they are up or down).
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
	if( button == GLFW_MOUSE_BUTTON_LEFT ) {
		leftMouseButton = action;
	}
}

//Set a callback for scrolling the mouse wheel
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	arcballDistance -= yoffset;
	if (arcballDistance <= 0) arcballDistance = .01;
}
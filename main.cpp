/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: SMK
 *  File: main.cpp
 *
 *	Group: Clantastic!
 *	Authors: Kepler Novotny,
 *
 *  Description:
 *      Draws Bezier surface and allows interactions/movement
 *		consistent with the assignment description.
 *
 */

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

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 640, windowHeight = 480;

int leftMouseButton;    	 						// status of the mouse button
glm::vec2 mousePos;			              		  	// last known X and Y of the mouse

float camDistance;										// radius of arcball camera
glm::vec3 targetPos;           						// camera position in cartesian coordinates
float cameraTheta, cameraPhi;               		// camera DIRECTION in spherical coordinates
glm::vec3 camDir; 			                    	// camera DIRECTION in cartesian coordinates

float vehicleHeading;           					// Directional facing angle of vehicle
glm::vec3 vehicleDir;								// Forward vector for vehicle
float wheelAngle;									// Animates wheel rotation
float moveSpeed;									// How fast the vehicle moves

int cameraMode;										// Determine camera type
bool zoom = false;									// Determines if the arcball can zoom

GLuint environmentDL;                       		// display list for the 'city'

vector<glm::vec3> controlPoints;					//Used to define/draw control points
int numSegments = 100;								//Resolution of curves

bool showCage = true;								//Determines whether to render the control lines
bool showCurve = true;								//Determines whether to show the bezier curve

//*************************************************************************************
//
// Helper Functions

// loadControlPoints() /////////////////////////////////////////////////////////
//
//  Load our control points from file and store them in
//	the global variable controlPoints
//
////////////////////////////////////////////////////////////////////////////////
bool loadControlPoints(char* filename) {
	ifstream fin(filename);
	if (fin.fail()) return false;

	//Find number of points
	int numpoints;
	string temp;
	getline(fin, temp);
	stringstream s(temp);
	s >> numpoints;

	if (numpoints % 16 != 0) {
		fprintf(stdout, "Error: invalid number of points\n");
		return -1;
	}

	//Populate point vector
	for (int i = 0; i < numpoints; i++) {
		int x, y, z;
		string line;
		getline(fin, line);
		stringstream t(line);
		getline(t, temp, ',');
		stringstream sx(temp);
		sx >> x;
		getline(t, temp, ',');
		stringstream sy(temp);
		sy >> y;
		getline(t, temp);
		stringstream sz(temp);
		sz >> z;

		glm::vec3 point(x, y, z);
		controlPoints.push_back(point);
	}

	return true;
}

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() { return rand() / (float)RAND_MAX; }

// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's direction in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta or cameraPhi is updated.
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
	float x = sin(cameraTheta) * sin(cameraPhi);
	float y = -cos(cameraPhi);
	float z = -cos(cameraTheta) * sin(cameraPhi);

	float d = sqrt(x * x + y * y + z * z);
	camDir = glm::vec3(x / d, y / d, z / d);
}

//Recalculates the direction vector of the vehicle
void calculateFacing() {
	float x = sin(vehicleHeading);
	float y = 0;
	float z = cos(vehicleHeading);


	float d = sqrt(x * x + y * y + z * z);
	vehicleDir = glm::vec3(x / d, y / d, z / d);
}

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

static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
	if( action == GLFW_PRESS ) {
		switch( key ) {
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
				exit(EXIT_SUCCESS);
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT_CONTROL) zoom = false;

	if (camDistance <= 0) camDistance = .01;
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
	if( leftMouseButton == GLFW_PRESS ) {
		double deltaX = x - mousePos.x;
		cameraTheta += .005 * deltaX;	//Recommended scale factor
		double deltaY = mousePos.y - y;	//Prevent inversion

		if (zoom) {
			camDistance -= .05 * deltaY;
		}
		else {
			cameraPhi += .005 * deltaY;
			if (cameraPhi <= 0) cameraPhi = .005;			//Prevent vertical flipping
			if (cameraPhi >= M_PI) cameraPhi = M_PI - .005;
			recomputeOrientation();     // update camera (x,y,z) based on (theta,phi)
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
	camDistance -= yoffset;
	if (camDistance <= 0) camDistance = .01;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!


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

// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution.
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution) {
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= resolution; i++) {
		glm::vec3 point = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * i / resolution);
		glVertex3f(point.x, point.y, point.z);
	}

	glEnd();
}

// evaluateBezierSurface() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier Surface.
//
////////////////////////////////////////////////////////////////////////////////
glm::vec3 evaluateBezierSurface(vector<glm::vec3> points, float u, float v) {
	glm::vec3 point(0, 0, 0);

	point = evaluateBezierCurve(
		evaluateBezierCurve(points.at(0), points.at(1), points.at(2), points.at(3), u),
		evaluateBezierCurve(points.at(4), points.at(5), points.at(6), points.at(7), u),
		evaluateBezierCurve(points.at(8), points.at(9), points.at(10), points.at(11), u),
		evaluateBezierCurve(points.at(12), points.at(13), points.at(14), points.at(15), u),
		v);

	return point;
}

// renderBezierSurface() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Surface as defined by 16 control points.
//  Breaks the curve into n segments as specified by the resolution.
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierSurface(vector<glm::vec3> points, int resolution) {
	glColor3f(.6, .6, .6);
	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			glBegin(GL_TRIANGLE_STRIP);
			glm::vec3 point1 = evaluateBezierSurface(points, 1.0f * i / resolution, 1.0f * j / resolution);
			glm::vec3 point2 = evaluateBezierSurface(points, 1.0f * (i + 1) / resolution, 1.0f * j / resolution);
			glm::vec3 point3 = evaluateBezierSurface(points, 1.0f * i / resolution, 1.0f * (j + 1) / resolution);
			glm::vec3 point4 = evaluateBezierSurface(points, 1.0f * (i + 1) / resolution, 1.0f * (j + 1) / resolution);

			glVertex3f(point1.x, point1.y, point1.z);
			glVertex3f(point2.x, point2.y, point2.z);
			glVertex3f(point3.x, point3.y, point3.z);
			glVertex3f(point4.x, point4.y, point4.z);

			glEnd();
		}
	}
}

//Draws the curve and all associated control items
void drawCurve() {
	glLineWidth(3.0f);

	if (showCage) {
		//Draw control points
		for (unsigned int i = 0; i < controlPoints.size(); i++) {
			glm::mat4 transMtx = glm::translate(glm::mat4(), controlPoints.at(i));
			glMultMatrixf(&transMtx[0][0]);
			glColor3f(0, .2, 0);
			CSCI441::drawSolidSphere(.15, 10, 10);
			glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		}

		//Draw cage
		glBegin(GL_LINE_STRIP);
		glColor3f(.9, .8, .5);
		for (unsigned int i = 0; i < controlPoints.size(); i++) {
			glVertex3f(controlPoints.at(i).x, controlPoints.at(i).y, controlPoints.at(i).z);
		}

		glEnd();
	}

	glColor3f(0, 0, 1);

	//Draw curves
	if (showCurve) {
		for (unsigned int i = 0; i < controlPoints.size() - 1; i += 3) {
			renderBezierCurve(controlPoints.at(i), controlPoints.at(i + 1), controlPoints.at(i + 2), controlPoints.at(i + 3), numSegments);
		}
	}

	glLineWidth(1.0f);
}

// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawGrid() {
	/*
     *	We will get to why we need to do this when we talk about lighting,
     *	but for now whenever we want to draw something with an OpenGL
     *	Primitive - like a line, quad, point - we need to disable lighting
     *	and then reenable it for use with the CSCI441 3D Objects.
     */
	glDisable( GL_LIGHTING );

	glBegin(GL_LINES);

	glColor3f(1, 1, 1);

	for (int i = -50; i <= 50; i++) {
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);

		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);
	}

	glEnd();

	/*
     *	As noted above, we are done drawing with OpenGL Primitives, so we
     *	must turn lighting back on.
     */
	glEnable( GL_LIGHTING );
}

// drawCity() //////////////////////////////////////////////////////////////////
//
//  Function to draw a random city using CSCI441 3D Cubes
//
////////////////////////////////////////////////////////////////////////////////
void drawCity() {
	for (int i = -50; i <= 50; i++) {
		for (int j = -50; j <= 50; j++) {
			if (i % 2 == 0 && j % 2 == 0 && getRand() < .05) {
				glColor3f(getRand(), getRand(), getRand());
				float scale = getRand() * 9 + 1;

				glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(i, scale/2, j));
				glMultMatrixf(&transMtx[0][0]);
				glm::mat4 scaleMtx = glm::scale (glm::mat4(), glm::vec3(1, scale, 1));
				glMultMatrixf(&scaleMtx[0][0]);
				CSCI441::drawSolidCube(1);
				glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
				glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
			}
		}
	}
}

// generateEnvironmentDL() /////////////////////////////////////////////////////
//
//  This function creates a display list with the code to draw a simple
//      environment for the user to navigate through.
//
//  And yes, it uses a global variable for the display list.
//  I know, I know! Kids: don't try this at home. There's something to be said
//      for object-oriented programming after all.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironmentDL() {
	environmentDL = glGenLists(GLsizei (1));

	glNewList(environmentDL, GL_COMPILE);

	//drawGrid();
	//drawCity();
	renderBezierSurface(controlPoints, numSegments);

	glEndList();

}

//
//	void renderScene()
//
//		This method will contain all of the objects to be drawn.
//
void renderScene(void)  {
	glCallList(environmentDL);
}

//*************************************************************************************
//
// Setup Functions

//
//  void setupGLFW()
//
//      Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
GLFWwindow* setupGLFW() {
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback( error_callback );

	// initialize GLFW
	if( !glfwInit() ) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW initialized\n" );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );	// request OpenGL v2.X
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	// request OpenGL v2.1
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );		// do not allow our window to be able to be resized

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, "Super Mario Kart", NULL, NULL );
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(window);		// make the created window the current window
	glfwSwapInterval(1);				     	// update our screen after at least 1 screen refresh

	glfwSetKeyCallback( window, keyboard_callback );				// set our keyboard callback function
	glfwSetCursorPosCallback( window, cursor_callback );			// set our cursor position callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function
	glfwSetScrollCallback(window, scroll_callback);					// set the scroll wheel callback function

	return window;						       // return the window that was created
}

//
//  void setupOpenGL()
//
//      Used to setup everything OpenGL related.  For now, the only setting
//	we need is what color to make the background of our window when we clear
//	the window.  In the future we will be adding many more settings to this
//	function.
//
void setupOpenGL() {
	// tell OpenGL to perform depth testing with the Z-Buffer to perform hidden
	//		surface removal.  We will discuss this more very soon.
	glEnable( GL_DEPTH_TEST );

	//******************************************************************
	// this is some code to enable a default light for the scene;
	// feel free to play around with this, but we won't talk about
	// lighting in OpenGL for another couple of weeks yet.
	float lightCol[4] = { 1, 1, 1, 1};
	//Increased ambient light becuase control cage appears too dark
	float ambientCol[4] = { 0.5, 0.5, 0.5, 1.0 };
	float lPosition[4] = { 10, 10, 10, 1 };
	glLightfv( GL_LIGHT0, GL_POSITION,lPosition );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,lightCol );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	// tell OpenGL not to use the material system; just use whatever we
	// pass with glColor*()
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	//******************************************************************

	// tells OpenGL to blend colors across triangles. Once lighting is
	// enabled, this means that objects will appear smoother - if your object
	// is rounded or has a smooth surface, this is probably a good idea;
	// if your object has a blocky surface, you probably want to disable this.
	glShadeModel( GL_SMOOTH );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// set the clear color to black
}

//
//  void setupScene()
//
//      Used to setup everything scene related.  Give our camera an
//	initial starting point and generate the display list for our city
//
void setupScene() {
	// give the camera a scenic starting point.
	targetPos.x = 0;
	targetPos.y = 1.5;
	targetPos.z = 0;
	cameraTheta = -M_PI / 3.0f;
	cameraPhi = M_PI / 2.8f;
	camDistance = 25;
	recomputeOrientation();

	cameraMode = 0;

	srand( time(NULL) );	// seed our random number generator
	generateEnvironmentDL();
}

///*************************************************************************************
//
// Our main function

//
//	int main( int argc, char *argv[] )
//
//		Really you should know what this is by now.  We will make use of the parameters later
//
int main( int argc, char *argv[] ) {
	//Read in control points for curve from file
	if (argc != 2) {
		fprintf(stdout, "Error: must specify a data file\n");
		return -1;
	}

	if (!loadControlPoints(argv[1])) {
		fprintf(stdout, "Error: invalid data file\n");
		return -1;
	}


	// GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();										// initialize all of the OpenGL specific information
	setupScene();											// initialize objects in our scene

	//  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.
	while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
		glDrawBuffer( GL_BACK );				// work with our back frame buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

		// update the projection matrix based on the window size
		// the GL_PROJECTION matrix governs properties of the view coordinates;
		// i.e. what gets seen - use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projMtx = glm::perspective( 45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.001f, 1000.0f );
		glMatrixMode( GL_PROJECTION );	// change to the Projection matrix
		glLoadIdentity();				// set the matrix to be the identity
		glMultMatrixf( &projMtx[0][0] );// load our orthographic projection matrix into OpenGL's projection matrix state

		// Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.
		GLint framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

		// update the viewport - tell OpenGL we want to render to the whole window
		glViewport( 0, 0, framebufferWidth, framebufferHeight );

		glMatrixMode( GL_MODELVIEW );	// make the ModelView matrix current to be modified by any transformations
		glLoadIdentity();							// set the matrix to be the identity

		// set up our look at matrix to position our camera
		glm::mat4 viewMtx = glm::lookAt( targetPos - camDistance * camDir,		// camera is located at given position
										 targetPos,							// camera is looking in correct direction
										 glm::vec3(  0,  1,  0 ) );			// up vector is (0, 1, 0) - positive Y
		// multiply by the look at matrix - this is the same as our view martix
		glMultMatrixf( &viewMtx[0][0] );

		renderScene();					// draw everything to the window

		glMultMatrixf(&(glm::inverse(viewMtx))[0][0]);

		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen
	}

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;				// exit our program successfully!
}

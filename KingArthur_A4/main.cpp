/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: A3
 *  File: main.cpp
 *
 *	Coleman A Hoyt
 *
 *  Description:
 *      A moving vehicle with animation and an arcball
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
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>				// for cos(), sin() functionality
#include <stdio.h>			// for printf functionality
#include <stdlib.h>			// for exit functionality
#include <time.h>			  // for time() functionality
using namespace std;
//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 640, windowHeight = 480;
float PHI_MAX = M_PI;
int leftMouseButton;    	 						// status of the mouse button
glm::vec2 mousePos;			              		  	// last known X and Y of the mouse
glm::vec3 vehiclePos, vehicleDir;                   // vehicle position and direction
glm::vec3 faerieLocation;
float arcballDistance; 
//glm::vec3 camPos;            						// camera position in cartesian coordinates
float cameraTheta, cameraPhi;               		// camera DIRECTION in spherical coordinates
glm::vec3 camDir; 			                    	// camera DIRECTION in cartesian coordinates
float vehicleTheta;
float wheelPhi;
float step_size;
GLuint environmentDL;                       		// display list for the 'city'
std::vector<glm::vec3> controlPoints;
float MAX_XZ = 50;
float MIN_XZ = -50;
int faerieSegment;
int faerieStep;
int curveResolution;

// THIS IS GOING TO BE UGLY! (but we're in it together)
int CtrlState;
int Astate;
int Wstate;
int Sstate;
int Dstate;
bool hideCage;
bool hideBezierCurve;

glm::mat4 identity = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
//*************************************************************************************
//
// Helper Functions

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() { return rand() / (float)RAND_MAX; }

vector<glm::vec3> getCoefficients(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	vector<glm::vec3> coefficients;
	coefficients.push_back(-p0 + 3.0f*p1 - 3.0f*p2 + p3);
	coefficients.push_back(3.0f*p0 - 6.0f*p1 + 3.0f*p2);
	coefficients.push_back(-3.0f*p0 + 3.0f*p1);
	coefficients.push_back(p0);
	return coefficients;
}
glm::vec3 evaluateBezierPoint( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, float t) {
	return pow(t, 3.0f) * a + pow(t, 2.0f) * b + t * c + d;
}
void renderBezierSegment( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 p3, int resolution) {
	// NOTE: I thought it would be more efficient to compute these ONCE since they are the same for
	// each evaluation, just as the slides suggest. 
	// They are the new parameters that evaluateBezierCurve takes in.
	float dt = 1.0f/resolution;
	glBegin(GL_LINE_STRIP);
    for(float t = 0; t < 1; t += dt) {
		glm::vec3 vertex = evaluateBezierPoint(a, b, c, d, t);
		glVertex3f(vertex.x, vertex.y, vertex.z);
	}
	glVertex3f(p3.x, p3.y, p3.z);
	glEnd();
}

float getRandRange(float lower, float upper) {
	
	return getRand()*(upper - lower) + lower;
}
// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's direction in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta or cameraPhi is updated.
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
	camDir = glm::vec3(
		sin(cameraTheta)*sin(cameraPhi),
		-cos(cameraPhi),
		-cos(cameraTheta)*sin(cameraPhi));
	
	// and NORMALIZE this directional vector!!!
	glm::normalize(camDir);
	//camPos = arcballDistance*camDir + vehiclePos;
}

void recomputeVehicleDirection() {
	vehicleDir = glm::vec3(-sin(vehicleTheta), 0, cos(vehicleTheta));
	glm::normalize(vehicleDir);
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

bool keyisdown (int keystate) {
	return (keystate == GLFW_PRESS || keystate == GLFW_REPEAT);
}

static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
	if( action == GLFW_PRESS ) {
		switch( key ) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				exit(EXIT_SUCCESS);
		}
	}
	switch(key) {
		case GLFW_KEY_W: {
			Wstate = action;
			break;
		}
		case GLFW_KEY_A: {
			Astate = action;
			break;
		}
		case GLFW_KEY_S: {
			Sstate = action;
			break;
		}
		case GLFW_KEY_D: {
			Dstate = action;
			break;
		}
		case GLFW_KEY_C: {
			if (action == GLFW_PRESS) hideCage = !hideCage;
			break;
		}
		case GLFW_KEY_B: {
			if (action == GLFW_PRESS) hideBezierCurve = !hideBezierCurve;
			break;
		}
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL: {
			CtrlState = action;
			break;
		}
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
	if (leftMouseButton == GLFW_PRESS && keyisdown(CtrlState)) {
		arcballDistance += (y - mousePos.y)*(0.005);
	} 
	else if( leftMouseButton == GLFW_PRESS ) {
		cameraTheta += (x - mousePos.x)*(0.005);
		cameraPhi -= (mousePos.y - y)*(0.005);
		if (cameraPhi > PHI_MAX) cameraPhi = PHI_MAX;
		recomputeOrientation();     // update camera direction based on (theta,phi)
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

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawGrid() {
	glDisable( GL_LIGHTING );
	glColor3f(0.5, 1, 1);
	glBegin(GL_LINES);
	for (int i = -50; i < 50; i += 1) {
		glColor3f(0.5, 0.5, 1);
		glVertex3d(i, 0, 50);
		glVertex3d(i, 0, -50);
		
		glColor3f(1, 0.5, 0.5);
		glVertex3d(50, 0, i);
		glVertex3d(-50, 0, i);
	}
	glEnd();
	glEnable( GL_LIGHTING );
}

// drawVehicle() //////////////////////////////////////////////////////////////////
//
//  Function to draw a random city using CSCI441 3D Cubes
//
////////////////////////////////////////////////////////////////////////////////
void drawWheel(float wheelRadius) {
	glm::mat4 rotMtx1 = glm::rotate(identity, wheelPhi, glm::vec3(0, 0, -1));
	glMultMatrixf(&rotMtx1[0][0]);
	CSCI441::drawSolidTorus(1, wheelRadius - 1, wheelRadius , 8);
	glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
}
void drawSeat() {
	glColor3f(1, 0.2, 1);
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	glVertex3f(0,3.8,2.5);
	glVertex3f(1,4,-1);
	glVertex3f(-1,4,-1);
	glEnd();
	glEnable(GL_LIGHTING);
}
void drawFaerie() {
	glEnable(GL_LIGHTING);
	glColor3f(0, 1, 0);
	glm::mat4 rotMtx = glm::rotate(identity, faerieStep * 0.04f, faerieLocation);
	glMultMatrixf(&rotMtx[0][0]);
	CSCI441::drawSolidTeapot(0.5);
	glm::mat4 scalMtx = glm::scale(identity, glm::vec3(0.2, 0.2, 0.3));
	glMultMatrixf(&scalMtx[0][0]);
	drawSeat();
	glMultMatrixf(&(glm::inverse(scalMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	glDisable(GL_LIGHTING);
}
void drawBezierCurve() {

	if (!hideCage) {
			glColor3f(0, 1, 0);
		for (int i = 0; i < controlPoints.size(); i++) {
			glm::mat4 transMtx = glm::translate(identity, controlPoints[i]);
			glMultMatrixf(&transMtx[0][0]);
			CSCI441::drawSolidSphere(0.4, 20, 20);
			glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		}
		glDisable(GL_LIGHTING);
		glColor3f(1, 1, 0);
		glLineWidth(3.0f);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < controlPoints.size(); i++) {
			glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
		}
		glEnd();
	}
	glColor3f(0, 0, 1);
	for(int i = 0; i + 3 < controlPoints.size(); i+=3) {
		vector<glm::vec3> bezierCoefficients = getCoefficients(
			controlPoints[i],
			controlPoints[i + 1],
			controlPoints[i + 2],
			controlPoints[i + 3]
		);
		if (i / 3 == faerieSegment) {
			faerieLocation = evaluateBezierPoint(
				bezierCoefficients[0],
				bezierCoefficients[1],
				bezierCoefficients[2],
				bezierCoefficients[3],
				faerieStep * 0.001
			);
			faerieStep++;
			if (faerieStep * 0.001 > 1) {
				faerieStep = 0;
				faerieSegment++;
				if (faerieSegment*3 > controlPoints.size() - 3) faerieSegment = 0;
			}

		}
		if (!hideBezierCurve) {
			renderBezierSegment(
				bezierCoefficients[0],
				bezierCoefficients[1],
				bezierCoefficients[2],
				bezierCoefficients[3],
				controlPoints[i + 3],
				30
			);
		}
	}
	glm::mat4 transMtx = glm::translate(identity, faerieLocation);
	glMultMatrixf(&transMtx[0][0]);
	drawFaerie();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	glEnable(GL_LIGHTING);
}
void drawVehicle() {

	float wheelRadius = 3;
	
	glm::mat4 transMtx1 = glm::translate(identity, vehiclePos);
	glMultMatrixf(&transMtx1[0][0]);
	glm::mat4 scalMtx = glm::scale(identity, glm::vec3(1.5, 1.5, 1.5));
	glm::mat4 rotMtx = glm::rotate(identity, vehicleTheta + (float) M_PI/2, glm::vec3(0, -1, 0));
	glMultMatrixf(&rotMtx[0][0]);
	glMultMatrixf(&scalMtx[0][0]);
	drawBezierCurve();
	glMultMatrixf(&(glm::inverse(scalMtx))[0][0]);
	glm::mat4 transMtx2 = glm::translate(identity, glm::vec3(0, wheelRadius, 0));
	glMultMatrixf(&transMtx2[0][0]);
	drawWheel(wheelRadius);
	drawSeat();
	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx2))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx1))[0][0]);
}
// generateEnvironmentDL() /////////////////////////////////////////////////////
// This function creates a display list with the code to draw a simple
//      environment for the user to navigate through.
//
//  And yes, it uses a global variable for the display list.
//  I know, I know! Kids: don't try this at home. There's something to be said
//      for object-oriented programming after all.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironmentDL() {
	environmentDL = glGenLists(1);
	glNewList(environmentDL, GL_COMPILE);
	drawGrid();
	//drawVehicle();
	glEndList();
}

//
//	void renderScene()
//
//		This method will contain all of the objects to be drawn.
//
void renderScene(void)  {
	drawVehicle();
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
	GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, "Vroom Vroom!", NULL, NULL );
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(window);		// make the created window the current window
	glfwSwapInterval(1);				     	// update our screen after at least 1 screen refresh

	glfwSetKeyCallback( window, keyboard_callback );							// set our keyboard callback function
	glfwSetCursorPosCallback( window, cursor_callback );					// set our cursor position callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function

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
	float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
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
void updateScene() {
	if (keyisdown(Wstate)) {
		vehiclePos += vehicleDir*step_size;
		wheelPhi += step_size;
	}
	if (keyisdown(Sstate)) {
		vehiclePos -= vehicleDir*step_size;
		wheelPhi -= step_size;
	}
	if(vehiclePos.x > MAX_XZ) vehiclePos.x = MAX_XZ;
	if(vehiclePos.x < MIN_XZ) vehiclePos.x = MIN_XZ;
	if(vehiclePos.z > MAX_XZ) vehiclePos.z = MAX_XZ;
	if(vehiclePos.z < MIN_XZ) vehiclePos.z = MIN_XZ;
	if (keyisdown(Astate)) {
		vehicleTheta -= step_size;
	}
	if (keyisdown(Dstate)) {
		vehicleTheta += step_size;
	}

	recomputeVehicleDirection();
}
void setupScene() {
	// give the camera a scenic starting point.
	arcballDistance = 20;
	vehiclePos = glm::vec3(0, 0, 0);
	vehicleDir = glm::vec3(1, 0, 0);
	vehicleTheta = 1;
	cameraTheta = -M_PI / 3.0f;
	cameraPhi = 2.37753;
	wheelPhi = 0;
	step_size = 0.01;
	faerieSegment = 0;
	faerieStep = 0;
	curveResolution = 30;
	hideBezierCurve = false;
	hideCage = false;
	recomputeVehicleDirection();
	recomputeOrientation();

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
		// TODO #01: make sure a control point CSV file was passed in.  Then read the points from file
	// GLFW sets up our OpenGL context so must be done first
	// initialize all of the GLFW specific information releated to OpenGL and our window
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();					// initialize all of the OpenGL specific information
	setupScene();					// initialize objects in our scene
	if (argc != 2) {
		cerr << "need a csv file" << endl;
		return -1;
	}
	ifstream fin;
	fin.open(argv[1]);
	if (!fin) {
		cerr << "error with file" << endl;
		return -1;
	}
	int numPoints;
	fin >> numPoints;
	while (!fin.eof()) {
		glm::vec3 vertex;
		fin >> vertex.x;
		fin.ignore();
		fin >> vertex.y;
		fin.ignore();
		fin >> vertex.z;
		controlPoints.push_back(vertex);
	}
	fin.close();
	// GLFW sets up our OpenGL context so must be done first
	

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
		// TODO #6: Change how our lookAt matrix gets constructed
		glm::mat4 viewMtx = glm::lookAt( camDir*arcballDistance + vehiclePos, // camera is located at camPos
										 vehiclePos,		// camera is looking a point directly ahead
										 glm::vec3(  0,  1,  0 ) );		// up vector is (0, 1, 0) - positive Y
		// multiply by the look at matrix - this is the same as our view martix
		glMultMatrixf( &viewMtx[0][0] );

		renderScene();					// draw everything to the window

		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen
		updateScene();
	}

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;				// exit our program successfully!
}
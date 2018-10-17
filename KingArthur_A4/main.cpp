/*
 *  CSCI 441, Computer Graphics, Fall 2018
 *
 *  Project: SMK Continued
 *  File: main.cpp
 *
 *	Clantastic
 *
 *  Description:
 *      
 */

// include the OpenGL library header

#include "Globals.h"

#include "Heroes/Kepler.h"
#include "Heroes/Hero.h"
#include "Track.h"
#include "Heroes/Ollie.h"
#include "Heroes/KingArthurHero.h"
#include "Textures.h"
#include "RacetrackInput.h"

#include "Setup.h"
#include "Event_Handlers.h"
#include "Bezier.h"
#include "Track.h"

using namespace std;

static vector<Drawable*> objects;     // object that are drawable


//*************************************************************************************
//
// Helper Functions

// setupLights()
//
// Set the types and colors of our lights
//
void setupLights() {
    glEnable( GL_LIGHTING );            // we are now using lighting
    glEnable( GL_LIGHT0 );              // and turn on Light 0 please (and thank you)
    float diffuseCol[4] = { 1.0, 1.0, 1.0, 1.0 };           // white diffuse light
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseCol );
    float specularCol[4] = { 1.0, 1.0, 1.0, 1.0 };          // white specular light
    glLightfv( GL_LIGHT0, GL_SPECULAR, specularCol );
    float ambientCol[4] = { 0.8, 0.8, 0.8, 0.5 };           // soft gray ambient light
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
	// harsh ambient for skybox!
	glLightfv ( GL_LIGHT1, GL_AMBIENT, diffuseCol );
}



// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() { return rand() / (float)RAND_MAX; }



float getRandRange(float lower, float upper) {

	return getRand()*(upper - lower) + lower;
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
	// rvi
	environmentDL = glGenLists(1);
	glNewList(environmentDL, GL_COMPILE);
	for ( int i = 0; i < curveCount; i++ ){
		// clear out existing CPs
		controlPoints.clear();
		for ( int j = 0; j < 16; j++ ){
			// dump new control points in sets of 16 into CP
			controlPoints.push_back(surface.at(16 * i + j));
		}
		// render the surface and repeat
		renderBezierSurface(controlPoints, curveResolution);
	}

	for (unsigned int i = 0; i < track.size() - 1; i += 3) {
		drawTraceSurface(track.at(i), track.at(i + 1), track.at(i + 2), track.at(i + 3), 100);
	}

	//Generate curve distances
	glm::vec3 prevPoint(track.at(0));
	for (unsigned int t = 0; t < (curveResolution * (track.size() - 1) / 3); t++) {
		int curveNum = t / curveResolution;
		glm::vec3 tPoint = evaluateBezierCurve(track.at(curveNum * 3), track.at(curveNum * 3 + 1),
			track.at(curveNum * 3 + 2), track.at(curveNum * 3 + 3),
			1.0f * (t % curveResolution) / curveResolution);

		float dist = glm::length(tPoint - prevPoint);
		totalDist += dist;

		curveDist[totalDist] = t;

		prevPoint = tPoint;
	}

	// add skybox and do corrective lighting
	glDisable( GL_LIGHT0 );
	glEnable ( GL_LIGHT1 );
	glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, skyTexHandle );
	//glColor3f(0.0f,0.5f,1.0f);
	glColor3f(1.0,1.0,1.0);
	CSCI441::drawSolidSphere(500,25,25);
	glDisable( GL_LIGHT1 );
	glEnable ( GL_LIGHT0 );
	glDisable( GL_TEXTURE_2D );
	// FIXME -- add objects and shit here too
	for ( Drawable* object : objects ) {
		glPushMatrix();
		glMultMatrixf( &(*object).position[0][0] ); {
			glMultMatrixf( &(*object).orientation[0][0] ); {
				glMultMatrixf( &(*object).scale[0][0] ); {
					PineTree* temp = static_cast<PineTree*>(object);
					(*temp).draw();
				} glMultMatrixf( &(glm::inverse( (*object).scale ))[0][0] );
			} glMultMatrixf( &(glm::inverse( (*object).orientation ))[0][0] );
		} glMultMatrixf( &(glm::inverse( (*object).position ))[0][0] );
	}
	glEndList();
}

//
//	void renderScene()
//
//		This method will contain all of the objects to be drawn.
//
void renderScene(void)  {
	glCallList(environmentDL);

	glm::mat4 lookMtx = glm::inverse(glm::lookAt(hero1Position, hero1NextPoint, hero1Normal));
	glMultMatrixf(&lookMtx[0][0]);

	glm::mat4 transMtx2 = glm::translate(glm::mat4(), hero1Normal * 2.2f);
	glMultMatrixf(&transMtx2[0][0]);

	glm::mat4 rotMtx = glm::rotate(glm::mat4(), glm::radians(180.0f), hero1Normal);
	glMultMatrixf(&rotMtx[0][0]);

	ds.draw(true);

	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx2))[0][0]);
	glMultMatrixf(&(glm::inverse(lookMtx))[0][0]);

	//Draw hero 2
	lookMtx = glm::inverse(glm::lookAt(hero2Position, hero2NextPoint, hero2Normal));
	glMultMatrixf(&lookMtx[0][0]);

	rotMtx = glm::rotate(glm::mat4(), glm::radians(90.0f), hero2Normal);
	glMultMatrixf(&rotMtx[0][0]);

	ka.draw(true);

	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(lookMtx))[0][0]);

	// allways apply gravity to Ollie
	// or always be sitting on surface????

	glm::vec3 OllieNextPoint = evaluateBezierSurface(controlPoints, (float)(ollieParams.z + ollieDir.z) / curveResolution, (float)(ollieParams.x + ollieDir.x) / curveResolution);
	lookMtx = glm::inverse(glm::lookAt(olliePos, OllieNextPoint, ollieNormal));
	glMultMatrixf(&lookMtx[0][0]);

	glm::mat4 scaleMtxOllie = glm::scale(glm::mat4(), glm::vec3(.0625, .0625, .0625));
	glMultMatrixf(&scaleMtxOllie[0][0]);

	glm::mat4 rotMtxOllie = glm::rotate(glm::mat4(), glm::radians(180.0f), glm::vec3(0,1,0));
	glMultMatrixf( &rotMtxOllie[0][0]);

	ollie.draw(true);

	glMultMatrixf(&(glm::inverse(lookMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtxOllie))[0][0]);
	glMultMatrixf(&(glm::inverse(scaleMtxOllie))[0][0]);
}



//
//  void setupScene()
//
//      Used to setup everything scene related.  Give our camera an
//	initial starting point and generate the display list for our city
//
void updateScene() {

	if(keyisdown(upState)) {
		ollieParams.x += ollieDir.x*ollieStep;
		ollieParams.y += ollieDir.y*ollieStep;
		ollieParams.z += ollieDir.z*ollieStep;

		if(ollieParams.x <= 1) {
			ollieParams.x = 1;
		} else if(ollieParams.x >= 99) {
			ollieParams.x = 99;
		}

		if(ollieParams.z <= 1) {
			ollieParams.z = 1;
		} else if(ollieParams.z >= 99) {
			ollieParams.z = 99;
		}

	}

	if(keyisdown(leftState)) {
		ollieTheta -= .05;
		recomputeOllieOrientation();
	}

	if(keyisdown(downState)) {
		ollieParams.x -= ollieDir.x*ollieStep;
		ollieParams.y -= ollieDir.y*ollieStep;
		ollieParams.z -= ollieDir.z*ollieStep;

		if(ollieParams.x <= 1) {
			ollieParams.x = 1;
		} else if(ollieParams.x >= 99) {
			ollieParams.x = 99;
		}

		if(ollieParams.z <= 1) {
			ollieParams.z = 1;
		} else if(ollieParams.z >= 99) {
			ollieParams.z = 99;
		}
	}

	if(keyisdown(rightState)) {
		ollieTheta += .05;
		recomputeOllieOrientation();
	}

	if (keyisdown(Wstate)) {
		if(cameraType == 2) {
			camPos.x += camDir.x*step;
			camPos.y += camDir.y*step;
			camPos.z += camDir.z*step;
		}
	}
	if (keyisdown(Sstate)) {
		if(cameraType == 2) {
			camPos.x -= camDir.x*step;
			camPos.y -= camDir.y*step;
			camPos.z -= camDir.z*step;
		}
	}
	// values gathered from input file
	if(vehiclePos.x > xyz_min_max[3]) vehiclePos.x = xyz_min_max[3];
	if(vehiclePos.x < xyz_min_max[0]) vehiclePos.x = xyz_min_max[0];
	if(vehiclePos.z > xyz_min_max[5]) vehiclePos.z = xyz_min_max[5];
	if(vehiclePos.z < xyz_min_max[2]) vehiclePos.z = xyz_min_max[2];
	if (keyisdown(Astate)) {
		if(cameraType == 2) {
			// camPos.x += camDir.x*step;
			// camPos.z += camDir.z*step;
		}
	}
	if (keyisdown(Dstate)) {
		if(cameraType == 2) {
			// camPos.x -= camDir.x*step;
			// camPos.z -= camDir.z*step;
		}
	}

	olliePos = evaluateBezierSurface(
		controlPoints,
		(float) ollieParams.z / curveResolution,
		(float) ollieParams.x / curveResolution);

	glm::vec3 normPoint1 = evaluateBezierSurface( controlPoints, (float) ollieParams.z / curveResolution, (float) ollieParams.x / curveResolution);
	glm::vec3 normPoint2 = evaluateBezierSurface( controlPoints, (float) (ollieParams.z) / curveResolution, (float) (ollieParams.x + 1) / curveResolution );
	glm::vec3 normPoint3 = evaluateBezierSurface( controlPoints, (float) (ollieParams.z + 1) / curveResolution, (float) (ollieParams.x) / curveResolution );

	glm::vec3 normV1 = normPoint2 - normPoint1;
	glm::vec3 normV2 = normPoint3 - normPoint1;


	glm::vec3 surfNormVec = glm::cross(normV2, normV1);
	ollieNormal = glm::normalize(surfNormVec);
	//if(surfNormVec.y < 0) {
	//	surfNormVec = -surfNormVec;
	//}
	//ollieNormRotAxis = glm::cross(glm::vec3(0, 1, 0), surfNormVec);
	//ollieThetaChange = acos(glm::dot(glm::vec3(0, 1, 0), surfNormVec));

	// Move Ollie along track curve every time updateScene is called

}

void setupScene() {
	// give the camera a scenic starting point.
	ollieParams = glm::vec3(5, 0, 5);
	olliePos = glm::vec3(5,0,5);
	ollieNormal = glm::vec3(0, 1, 0);
	arcballDistance = 20;
	vehiclePos = glm::vec3(0, 0, 0);
	vehicleDir = glm::vec3(1, 0, 0);
	vehicleTheta = 1;
	cameraTheta = -M_PI / 3.0f;
	cameraPhi = 2.37753;
	ollieCamTheta = -M_PI / 3.0f;
	ollieCamPhi = 2.37753;
	recomputeOllieCamOrientation();
	ollieTheta = 0;
	recomputeOllieOrientation();
	camPos = glm::vec3(50, 50, 50);
	camDir = glm::normalize(glm::vec3(-1, -1, -1));
	wheelPhi = 0;
	step_size = 0.01;
	curveResolution = 100;
	hideBezierCurve = false;
	hideCage = false;
	recomputeVehicleDirection();

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

	if (!loadRaceTrack(argv[1], &curveCount, &surface, &trackPoints, &track, &objects, xyz_min_max)) {
		fprintf(stdout, "Error: invalid data file\n");
		return -1;
	}
	else {
		cout << "Curves: " << curveCount << endl;
		cout << "Points: " << surface.size() << endl;
		cout << "Tracks: " << trackPoints << endl;
		cout << "Points: " << track.size() << endl;
	}
	// GLFW sets up our OpenGL context so must be done first
	// initialize all of the GLFW specific information releated to OpenGL and our window
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();					// initialize all of the OpenGL specific information
	setupTextures();                  // load our textures into memory and register with OpenGL
	setupLights();
	setupScene();					// initialize objects in our scene

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
		glm::mat4 viewMtx;
		if(cameraType == 1) {
			switch (heroChoice) {
				// arcball camera for Ollie
				case 4: {
					viewMtx = glm::lookAt( ollieCamDir*arcballDistance + olliePos, // camera is located at camPos
													 olliePos,		// camera is looking a point directly ahead
													 glm::vec3(  0,  1,  0 ) );		// up vector is (0, 1, 0) - positive Y
					// multiply by the look at matrix - this is the same as our view martix
					//glMultMatrixf( &viewMtx[0][0] );
					break;
				}

				// arcball camera for DarkSlayer
				case 5: {
					viewMtx = glm::lookAt( ollieCamDir*arcballDistance + hero1Position,
													 hero1Position,
													 glm::vec3( 0, 1, 0 ) );
					// multiply by the look at matrix - this is the same as our view martix
					//glMultMatrixf( &viewMtx[0][0] );
					break;
				}

				// arcball camera for King Arthur
				case 6: {
					viewMtx = glm::lookAt( ollieCamDir*arcballDistance + hero2Position,
													 hero2Position,
													 glm::vec3( 0, 1, 0 ) );
					// multiply by the look at matrix - this is the same as our view martix
					//glMultMatrixf( &viewMtx[0][0] );
					break;
				}
			}

		} else if(cameraType == 2) {
			// set up our look at matrix to position our camera
			viewMtx = glm::lookAt( camPos,
											 camDir + camPos,
											 glm::vec3(  0,  1,  0 ) );

			// multiply by the look at matrix - this is the same as our view martix
			//glMultMatrixf( &viewMtx[0][0] );
		}

		// create position for light0 and set light0 to this position in scene
		float lightPosition[4] = { 15.0, 15.0, 15.0, 1.0 };
        glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

		glMultMatrixf(&viewMtx[0][0]);

		renderScene();					// draw everything to the window

		glLoadIdentity();

		//Draw first person view in corner
		if (fpsCam) {
			//Create the viewport
			glViewport(2.0 * framebufferWidth / 3,				//Viewport takes up 1/9 of the screen
				2.0 * framebufferHeight / 3,
				1.0 * framebufferWidth / 3,				//Viewport in upper right corner
				1.0 * framebufferHeight / 3);

			//Clear out viewport background
			glScissor(2.0 * framebufferWidth / 3,
				2.0 * framebufferHeight / 3,
				1.0 * framebufferWidth / 3,
				1.0 * framebufferHeight / 3);
			glEnable(GL_SCISSOR_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Set camera based on current target
			switch (heroChoice) {
				// First person camera for Ollie
			case 4: {
				viewMtx = glm::lookAt(olliePos + glm::vec3(0, 4.5, 1), // camera is located at camPos
					olliePos + glm::vec3(0, 4.5, 1) + ollieDir,		// camera is looking a point directly ahead
					ollieNormal);		// up vector is (0, 1, 0) - positive Y

										// multiply by the look at matrix - this is the same as our view martix
				break;
			}

					// First person camera for DarkSlayer
			case 5: {
				glm::vec3 hero1DirectionVec = hero1NextPoint - hero1Position;

				viewMtx = glm::lookAt(hero1Position + glm::vec3(0, 5, 0), // camera is located at camPos
					hero1Position + glm::vec3(0, 5, 0) + hero1DirectionVec,		// camera is looking a point directly ahead
					hero1Normal);		// up vector is (0, 1, 0) - positive Y

										// multiply by the look at matrix - this is the same as our view martix
				break;
			}

					// First person camera for King Arthur
			case 6: {
				glm::vec3 hero2DirectionVec = hero2NextPoint - hero2Position;

				viewMtx = glm::lookAt(hero2Position + glm::vec3(0, 8, 0), // camera is located at camPos
					hero2Position + glm::vec3(0, 8, 0) + hero2DirectionVec,		// camera is looking a point directly ahead
					hero2Normal);		// up vector is (0, 1, 0) - positive Y

										// multiply by the look at matrix - this is the same as our view martix

				break;
			}
			}
			glMultMatrixf(&viewMtx[0][0]);

			renderScene();					//Draw to the viewport

			glMultMatrixf(&(glm::inverse(viewMtx))[0][0]);

			glDisable(GL_SCISSOR_TEST);
		}

		//Update hero1
		hero1ActiveCurve = hero1TPos / curveResolution;
		hero1Position = evaluateBezierCurve(track.at(hero1ActiveCurve * 3), track.at(hero1ActiveCurve * 3 + 1),
			track.at(hero1ActiveCurve * 3 + 2), track.at(hero1ActiveCurve * 3 + 3),
			1.0f * (hero1TPos % curveResolution) / curveResolution);

		hero1TPos += 1;	//Change to configure speed
		if (hero1TPos >= curveResolution * (track.size() - 1) / 3)		//If hero has gone through whole curve
			hero1TPos = 0;

		int hero1NextCurve = hero1TPos / curveResolution;
		hero1NextPoint = evaluateBezierCurve(track.at(hero1NextCurve * 3), track.at(hero1NextCurve * 3 + 1),
			track.at(hero1NextCurve * 3 + 2), track.at(hero1NextCurve * 3 + 3),
			1.0f * (hero1TPos % curveResolution) / curveResolution);

		glm::vec3 align = hero1NextPoint - hero1Position;
		hero1RotAxis = glm::normalize(glm::cross(hero1PrevAlign, align));
		hero1RotAngle = acos(glm::dot(hero1PrevAlign, align));
		hero1PrevAlign = align;

		glm::vec3 tangent = evaluateBezierTangent(track.at(hero1NextCurve * 3), track.at(hero1NextCurve * 3 + 1),
			track.at(hero1NextCurve * 3 + 2), track.at(hero1NextCurve * 3 + 3),
			1.0f * (hero1TPos % curveResolution) / curveResolution);
		tangent = glm::normalize(tangent + hero1Position);
		glm::vec3 rotationVector = glm::cross(tangent, align);
		glm::vec4 rotVec(rotationVector.x, rotationVector.y, rotationVector.z, 0);
		glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), rotationVector);
		glm::vec4 normal = rotVec * rotMtx1;
		hero1Normal = glm::normalize(glm::vec3(normal));

		//Find hero2's T position
		hero2Dist += 1;
		float prevDist;
		float nextDist;
		float hero2T = 0.0f;
		for (map<float, float>::iterator it = curveDist.begin(); it != curveDist.end(); ++it) {
			if (it->first > hero2Dist) {
				nextDist = it->first;

				//Lerp the t value
				float alpha = (hero2Dist - prevDist) / (nextDist - prevDist);
				hero2T = curveDist[prevDist] * (1 - alpha) + curveDist[nextDist] * alpha;

				break;
			}
			prevDist = it->first;

			if (next(it) == curveDist.end()) {
				hero2Dist = 0;
			}
		}

		//Update hero2's position
		hero2ActiveCurve = hero2T / curveResolution;
		hero2Position = evaluateBezierCurve(track.at(hero2ActiveCurve * 3), track.at(hero2ActiveCurve * 3 + 1),
			track.at(hero2ActiveCurve * 3 + 2), track.at(hero2ActiveCurve * 3 + 3),
			1.0f * (hero2T - hero2ActiveCurve * curveResolution) / curveResolution);

		int hero2nextT = curveDist[nextDist];
		if (hero2nextT >= curveResolution * (track.size() - 1) / 3)		//If hero has gone through whole curve
			hero2nextT = 0;

		int hero2NextCurve = hero2nextT / curveResolution;
		hero2NextPoint = evaluateBezierCurve(track.at(hero2NextCurve * 3), track.at(hero2NextCurve * 3 + 1),
			track.at(hero2NextCurve * 3 + 2), track.at(hero2NextCurve * 3 + 3),
			1.0f * (hero2nextT - hero2ActiveCurve * curveResolution) / curveResolution);

		align = hero2NextPoint - hero2Position;
		hero2RotAxis = glm::normalize(glm::cross(hero2PrevAlign, align));
		hero2RotAngle = acos(glm::dot(hero2PrevAlign, align));
		hero2PrevAlign = align;

		tangent = evaluateBezierTangent(track.at(hero2ActiveCurve * 3), track.at(hero2ActiveCurve * 3 + 1),
			track.at(hero2ActiveCurve * 3 + 2), track.at(hero2ActiveCurve * 3 + 3),
			1.0f * (hero2T - hero2ActiveCurve * curveResolution) / curveResolution);
		tangent = glm::normalize(tangent + hero2Position);
		rotationVector = glm::cross(tangent, align);
		rotVec = glm::vec4(rotationVector.x, rotationVector.y, rotationVector.z, 0);
		rotMtx1 = glm::rotate(glm::mat4(), glm::radians(-90.0f), rotationVector);
		normal = rotVec * rotMtx1;
		hero2Normal = glm::normalize(glm::vec3(normal));

		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen

		updateScene();

	}

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;				// exit our program successfully!
}

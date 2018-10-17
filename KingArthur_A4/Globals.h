// All OpenGL stuff
#include "Batch_Includes.h"

// Heroes
#include "Heroes/Kepler.h"
#include "Heroes/Hero.h"

#include "Heroes/Ollie.h"
#include "Heroes/KingArthurHero.h"

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
static int windowWidth = 1280, windowHeight = 960;
float PHI_MAX = M_PI;
int leftMouseButton;    	 						// status of the mouse button
glm::vec2 mousePos;			              		  	// last known X and Y of the mouse
glm::vec3 vehiclePos, vehicleDir;                   // vehicle position and direction
float arcballDistance;
// arcball camera variables
glm::vec3 camPos;            						// camera position in cartesian coordinates
float cameraTheta, cameraPhi;               		// camera DIRECTION in spherical coordinates
glm::vec3 camDir; 			                    	// camera DIRECTION in cartesian coordinates
bool fpsCam = false;

// ollie camera variablesc

glm::vec3 ollieCamPos;
float ollieCamTheta, ollieCamPhi;
glm::vec3 ollieCamDir;

float vehicleTheta;
float wheelPhi;
float step_size;
GLuint environmentDL;                       		// display list for the 'city'
std::vector<glm::vec3> controlPoints;
int curveResolution;
int cameraType = 2;
static bool keysDown[256] = {0};
glm::vec3 camAngles;
static int xyz_min_max[6] = {100,100,100,0,0,0};
// models

Ollie ollie;
float t = 0;
int curveNumber = 0;
glm::vec3 ollieParams;
glm::vec3 olliePos;
glm::vec3 ollieDir;
float ollieTheta;
glm::vec3 ollieNormal;
float ollieThetaChange;
glm::vec3 ollieNormRotAxis;

// THIS IS GOING TO BE UGLY! (but we're in it together)
int CtrlState, Astate, Wstate, Sstate, Dstate;
bool hideCage;
bool hideBezierCurve;
int upState, leftState, rightState, downState;
int heroChoice = 4;

glm::mat4 identity = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

// Rob's Filey bullshit
char* filename;
int curveCount;               // number of bezier surfaces
static vector<glm::vec3> surface;     // all surface points
int trackPoints;               // number of track control points
static vector<glm::vec3> track;       // track points


static GLuint grassTexHandle, skyTexHandle;

char* fileName;
int textureWidth, textureHeight;
int skyWidth, skyHeight;
unsigned char* imageData;
unsigned char* skyData;

DarkSlayer ds;
int hero1TPos = 0;
int hero1ActiveCurve = 0;
float hero1RotAngle;
glm::vec3 hero1PrevAlign(0, 0, 1);
glm::vec3 hero1RotAxis;
glm::vec3 hero1Position;
glm::vec3 hero1Normal;
glm::vec3 hero1NextPoint;

KingArthur ka;
float hero2Dist = 0;
int hero2ActiveCurve = 0;
float hero2RotAngle;
glm::vec3 hero2PrevAlign(0, 0, 1);
glm::vec3 hero2RotAxis;
glm::vec3 hero2Position;
glm::vec3 hero2Normal;
glm::vec3 hero2NextPoint;

float totalDist = 0;
map<float, float> curveDist;

// step sizes for movement of cam or ollie
float step = 1.0f;
float ollieStep = 1.5f;
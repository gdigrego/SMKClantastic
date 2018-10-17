#include "Globals.h"

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

void recomputeOllieCamOrientation() {
	ollieCamDir = glm::vec3(
		sin(ollieCamTheta)*sin(ollieCamPhi),
		-cos(ollieCamPhi),
		-cos(ollieCamTheta)*sin(ollieCamPhi)
	);

	glm::normalize(ollieCamDir);
}

void recomputeVehicleDirection() {
	vehicleDir = glm::vec3(-sin(vehicleTheta), 0, cos(vehicleTheta));
	glm::normalize(vehicleDir);
}

void recomputeOllieOrientation() {
	// TODO #5: Convert spherical coordinates into a cartesian vector
	// see Wednesday's slides for equations.  Extra Hint: Slide #70
	ollieDir.x =  cosf(ollieTheta);
    ollieDir.z = sinf(ollieTheta);

    // and NORMALIZE this directional vector!!!
    ollieDir = glm::normalize( ollieDir );
}
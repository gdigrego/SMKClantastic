#include "Globals.h"

using namespace std;

int rainbowMax = 25;
int rainbowRed = rainbowMax;
int rainbowGreen = 0;
int rainbowBlue = 0;
float rainbowAlphaValue = 0.2;
void drawControlPoints(vector<glm::vec3> controlPoints, int resolution);
void drawBezierCurve(vector<glm::vec3> controlPoints, int resolution);
bool testing = true;
void TESTING(){
	if (testing){
		testing = false;
	}
	else {
		testing = true;
	}
}
/*NOTE:
 * Before or after each FRAME, set rainbowRed to max and rainbowGreen/rainbowBlue to 0.
 * The functions here can not do this or each curve will restart the color set.
 */

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

// Calculates tangent of a given point
glm::vec3 evaluateBezierTangent(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
	glm::vec3 point(0, 0, 0);

	point = -3.0f * (1 - t) * (1 - t) * p0 + 3.0f * (1 - t) * (1 - t) * p1 - 6.0f * t * (1 - t) * p1 - 3.0f * t * t * p2 + 6.0f * t * (1 - t) * p2 + 3.0f * t * t * p3;

	return point;
}

//Set rainbow color gradient
void rainbowUpdateColor() {
	if (rainbowRed == rainbowMax) {
		if (rainbowBlue > 0) rainbowBlue -= 1;
		else if (rainbowGreen < rainbowMax) rainbowGreen += 1;
		else rainbowRed -= 1;
	}
	else if (rainbowGreen == rainbowMax) {
		if (rainbowRed > 0) rainbowRed -= 1;
		else if (rainbowBlue < rainbowMax) rainbowBlue += 1;
		else rainbowGreen -= 1;
	}
	else if (rainbowBlue == rainbowMax) {
		if (rainbowGreen > 0) rainbowGreen -= 1;
		else if (rainbowRed < rainbowMax) rainbowRed += 1;
		else rainbowBlue -= 1;
	}
	glColor4f(1.0f * rainbowRed / rainbowMax, 1.0f * rainbowGreen / rainbowMax, 1.0f * rainbowBlue / rainbowMax, rainbowAlphaValue);
}

//Reset color
//CALL ONCE PER FRAME if not statically defined
void resetColor() {
	rainbowRed = rainbowMax;
	rainbowBlue = 0;
	rainbowGreen = 0;
}

//Draw surface
void drawTraceSurface(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution) {
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if( testing ) {
		vector<glm::vec3> set;
		(set).push_back(p0);
		(set).push_back(p1);
		(set).push_back(p2);
		(set).push_back(p3);
		drawControlPoints(set, resolution);
		drawBezierCurve(set, resolution);
	}
	for (int i = 0; i < resolution; i++) {
		rainbowUpdateColor();
		glm::vec3 point = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * i / resolution);
		glm::vec3 point2 = evaluateBezierCurve(p0, p1, p2, p3, 1.0f * (i + 1) / resolution);
		glm::vec3 align = point2 - point;
		glm::vec3 tangent = evaluateBezierTangent(p0, p1, p2, p3, 1.0f * i / resolution);
		tangent = glm::normalize(tangent + point);
		glm::vec3 rotationVector = glm::cross(tangent, align);
		float theta = acos(glm::dot(align, tangent) / (align.length()));

		glm::mat4 transMtx = glm::translate(glm::mat4(), point);
		glMultMatrixf(&transMtx[0][0]);

		glm::mat4 rotMtx = glm::rotate(glm::mat4(), theta, rotationVector);
		glMultMatrixf(&rotMtx[0][0]);

		glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(10, 1.5, 1.5));
		glMultMatrixf(&scaleMtx[0][0]);

		CSCI441::drawSolidCube(1);

		glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);

		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}
	glDisable( GL_BLEND );
}


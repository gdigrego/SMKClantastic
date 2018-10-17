#include "Globals.h"

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
	//glColor3f(.6, .6, .6);
	glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, grassTexHandle );
	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			glBegin(GL_TRIANGLE_STRIP);
			glm::vec3 point1 = evaluateBezierSurface(points, 1.0f * i / resolution, 1.0f * j / resolution);
			glm::vec3 point2 = evaluateBezierSurface(points, 1.0f * (i + 1) / resolution, 1.0f * j / resolution);
			glm::vec3 point3 = evaluateBezierSurface(points, 1.0f * i / resolution, 1.0f * (j + 1) / resolution);
			glm::vec3 point4 = evaluateBezierSurface(points, 1.0f * (i + 1) / resolution, 1.0f * (j + 1) / resolution);

			float expectedHeight = 5;
			glColor3f((170 - (point1.y / expectedHeight) * 120) / 256.0, (150 + (point1.y / expectedHeight) * 20) / 256.0, (100 - (point1.y / expectedHeight) * 50) / 256.0);
			glVertex3f(point1.x, point1.y, point1.z);
			glTexCoord2f( 0.0f,  1.0f );
			//glColor3f((170 - (point2.y / expectedHeight) * 120) / 256.0, (150 + (point2.y / expectedHeight) * 20) / 256.0, (100 - (point2.y / expectedHeight) * 50) / 256.0);
			glVertex3f(point2.x, point2.y, point2.z);
			glTexCoord2f( 1.0f,  1.0f );
			//glColor3f((170 - (point3.y / expectedHeight) * 120) / 256.0, (150 + (point3.y / expectedHeight) * 20) / 256.0, (100 - (point3.y / expectedHeight) * 50) / 256.0);
			glVertex3f(point3.x, point3.y, point3.z);
			glTexCoord2f( 1.0f,  0.0f );
			//glColor3f((170 - (point4.y / expectedHeight) * 120) / 256.0, (150 + (point4.y / expectedHeight) * 20) / 256.0, (100 - (point4.y / expectedHeight) * 50) / 256.0);
			glVertex3f(point4.x, point4.y, point4.z);
			glTexCoord2f( 0.0f,  0.0f );
			glEnd();
		}
	}
	glDisable( GL_TEXTURE_2D );
}


void drawBezierCurve() {

	if (!hideCage) {
			glColor3f(0, 1, 0);
		for (unsigned int i = 0; i < controlPoints.size(); i++) {
			glm::mat4 transMtx = glm::translate(identity, controlPoints[i]);
			glMultMatrixf(&transMtx[0][0]);
			CSCI441::drawSolidSphere(0.4, 20, 20);
			glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
		}
		glDisable(GL_LIGHTING);
		glColor3f(1, 1, 0);
		glLineWidth(3.0f);

		glBegin(GL_LINE_STRIP);
		for (unsigned int i = 0; i < controlPoints.size(); i++) {

			glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
		}
		glEnd();
	}
	glColor3f(0, 0, 1);
	for(unsigned int i = 0; i + 3 < controlPoints.size(); i+=3) {
		vector<glm::vec3> bezierCoefficients = getCoefficients(
			controlPoints[i],
			controlPoints[i + 1],
			controlPoints[i + 2],
			controlPoints[i + 3]
		);
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
	glEnable(GL_LIGHTING);
}
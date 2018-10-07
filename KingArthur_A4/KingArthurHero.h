#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <CSCI441/objects.hpp>

glm::mat4 identity = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
float wheelPhi = 0;
float wheelRadius = 3

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
void drawWheel() {
	glm::mat4 rotMtx1 = glm::rotate(identity, wheelPhi, glm::vec3(0, 0, -1));
	glMultMatrixf(&rotMtx1[0][0]);
	CSCI441::drawSolidTorus(1, wheelRadius - 1, wheelRadius , 8);
	glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);
}

class Hero {
	void draw();
	public:
	void draw(bool movement);
};

void Hero::draw() {
	float wheelRadius = 3;
	glm::mat4 scalMtx = glm::scale(identity, glm::vec3(1.5, 1.5, 1.5));
	glMultMatrixf(&scalMtx[0][0]);
	glMultMatrixf(&(glm::inverse(scalMtx))[0][0]);
	glm::mat4 transMtx = glm::translate(identity, glm::vec3(0, wheelRadius, 0));
	glMultMatrixf(&transMtx[0][0]);
	drawWheel();
	drawSeat();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
}

void Hero::draw(bool movement) {
	if (movement) wheelPhi += 0.005;
	draw();
}
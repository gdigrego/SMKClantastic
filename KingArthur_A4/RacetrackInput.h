#include <GLFW/glfw3.h>	// include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

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
		return false;
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
		//controlPoints.push_back(point);
	}

	return true;
}
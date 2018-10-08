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

bool loadRaceTrack(     char* filename, 
                        int* curveCount,                // number of bezier surfaces
                        vector<glm::vec3>* surface,     // all surface points
                        int* trackPoints,               // number of track control points
                        vector<glm::vec3>* track,       // track points
                        vector<glm::vec3>* objects,     // object locations
                        char* objectType,               // object type
                        int* numObjects)                // object count
{
	ifstream fin(filename);
	if (fin.fail()) return false;

    //####################
    //  Surface Collection
    //####################
	//Find number of points for bezier surfaces
	string temp;
	getline(fin, temp);
	stringstream s(temp);
	s >> *curveCount;
    // start error handling here
	// Collect all bezier surface points
	for (int i = 0; i <  ( *curveCount * 4 ); i++) {
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
		(*surface).push_back(point);
	}
    //####################
    //  Racetrack input
    //####################
    //Find number of points for bezier curves
	getline(fin, temp);
	stringstream s(temp);
	s >> *trackPoints;
	// Collect all bezier surface points
	for (int i = 0; i < ( *trackPoints ); i++) {
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
		(*track).push_back(point);
	}
    //####################
    //  Object Collection
    //####################
    //Find number of points for Objects
    getline(fin, temp);
	stringstream s(temp);
	s >> *numObjects;
    // if no objects exit now
    if ( *numObjects == 0 ) return true;

    // else grab all objects
    /**
     * Order of data collection
     * <object type> 
     * <object x, y, z> 
     * <object orientation x, y, z> 
     * <object size>
     */
	
    // FIXME later when we actually have objects and shitttttt

	return true;
}
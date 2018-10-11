// include the OpenGL library header
#ifdef __APPLE__				// if compiling on Mac OS
#include <OpenGL/gl.h>
#else							// if compiling on Linux or Windows OS
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h> // include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h> // for printf functionality
#include "Tree.h"
//#include "Drawable.h"

using namespace std;

int numObjects;

bool loadRaceTrack(char *filename,
                   int *curveCount,            // number of bezier surfaces
                   vector<glm::vec3> *surface, // all surface points
                   int *trackPoints,           // number of track control points
                   vector<glm::vec3> *track,   // track points
                   vector<Drawable> *objects) // objects
{
    ifstream fin(filename);
    if (fin.fail())
        return false;
    try
    {
        //####################
        //  Surface Collection
        //####################
        //Find number of points for bezier surfaces
        string temp;
        getline(fin, temp);
        stringstream s_surface(temp);
        s_surface >> *curveCount;
        // start error handling here
        // Collect all bezier surface points
        for (int i = 0; i < (*curveCount * 16); i++)
        {
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
        stringstream s_track(temp);
        s_track >> *trackPoints;
        // Collect all bezier surface points
        for (int i = 0; i < (*trackPoints); i++)
        {
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
        stringstream s_objects(temp);
        s_objects >> numObjects;
        // if no objects exit now
        if (numObjects == 0)
            return true;

        // else grab all objects
        /**
         * Order of data collection
         * <object type> 
         * <object x, y, z> 
         * <object orientation x, y, z> 
         * <object size>
         */
        for (int i = 0; i < numObjects; i++){
            cout << "OBJECTION YOUR HONOR" << endl;
            int x, y, z;
            string line;
            getline(fin, line);
            stringstream t(line);
            // get object type, create parent class for it later
            char objectType;
            t.get(objectType);
            // get position
            getline(fin, line);
            cout << line << endl;
            stringstream tp(line);
            getline(tp, temp, ',');
            stringstream psx(temp);
            psx >> x;
            getline(tp, temp, ',');
            stringstream psy(temp);
            psy >> y;
            getline(tp, temp);
            stringstream psz(temp);
            psz >> z;
            glm::mat4 position = glm::translate(glm::mat4(), 
                glm::vec3(x,y,z));
            // get orientation
            getline(fin, line);
            cout << line << endl;
            stringstream to(line);
            getline(to, temp, ',');
            stringstream osx(temp);
            osx >> x;
            getline(to, temp, ',');
            stringstream osy(temp);
            osy >> y;
            getline(to, temp);
            stringstream osz(temp);
            osz >> z;
            glm::mat4 orientation = glm::translate(glm::mat4(), 
                glm::vec3(x,y,z));
            // get scale
            getline(fin, line);
            cout << line << endl;
            stringstream st(line);
            getline(st, temp, ',');
            stringstream ssx(temp);
            ssx >> x;
            getline(st, temp, ',');
            stringstream ssy(temp);
            ssy >> y;
            getline(st, temp);
            stringstream ssz(temp);
            ssz >> z;
            glm::mat4 scale = glm::scale(glm::mat4(), 
                glm::vec3(x,y,z));
            cout << "YEET " << objectType << endl;
            // perform rotations and matrix ops
            //glPushMatrix();
            //glMatrixMode(GL_MODELVIEW);
            //glMultMatrixf( &position[0][0] );{
                //glMultMatrixf( &orientation[0][0] );{
                    //glMultMatrixf( &scale[0][0] );{
                        //CSCI441::drawSolidTeapot(10);
                        //someTree.draw();
                        /*switch (objectType){
                            case 't':
                                // its a damn tree
                                cout << "MOTHAFUCKIN HAPPY TREEEEEEESSSSSS" << endl;
                                // but there are only trees
                                PineTree::draw();
                                break;
                            default:
                                PineTree::draw();
                                break;
                        } */
                    //} glMultMatrixf( &(glm::inverse( scale ))[0][0] );
                //} glMultMatrixf( &(glm::inverse( orientation ))[0][0] );
            //} glMultMatrixf( &(glm::inverse( position ))[0][0] );
            //glPopMatrix();
            cout << "fin" << endl;
        }

        // FIXME later when we actually have objects and shitttttt
    }
    catch (const std::exception &exc)
    {
        std::cerr << "GOT SOME FUCKING ERROR AGAIN: " << exc.what() << std::endl;
    }
    return true;
}
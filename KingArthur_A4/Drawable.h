// include the OpenGL library header
#ifdef __APPLE__				// if compiling on Mac OS
#include <OpenGL/gl.h>
#else							// if compiling on Linux or Windows OS
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h> // include GLFW framework header

#include <glm/glm.hpp>

class Drawable {
    public:
    glm::mat4 position, orientation, scale;
    void draw();
};
Group: Clantastic

Coleman Arthur Hoyt	choyt@mymail.mines.edu
Gino Di Gregorio	gdigrego@mymail.mines.edu
Kepler Novotny		knovotny@mymail.mines.edu
Robert Imbler 		rimbler@mymail.mines.edu

Midterm Project - SMK

This program generates a 3D bezier surface, as well as a bezier curve defined in 3D space. 
Two vehicles race around the curve, while a third person is able to wander freely over 
the surface.

The vehicles will move around the track automatically, and can not be slowed or stopped by
the user. By default, the camera is free, but the user can change camera modes easily.
Pressing the 4, 5, and 6 keys changes the individual/vehicle to which the camera is bound
and travels with. Pressing 1 shows the arcball camera, which will be centered around the 
active hero. Pressing 2 will bring up the free cam, which can move around the screen
using the WASD keys independently of any character movement. Pressing 3 brings up a first
person viewport which shows the view from the active character's perspective.

Compiling the code works as normal, simply run the Makefile with the include directory 
linked to the local copy on the user's computer. To run the program correctly, simply
type "./SMK ./DefaultControlPoints.csv", or the version of this command that works on the
user's operating system, making sure to pass DefaultControlPoints.csv in the call.

The file format is a .csv consisting of:

<number of bezier surfaces>*
<1st comma separated control point for surface>
<2nd comma separated control point>
...
<number of points defining bezier curve>
<comma separated position of control point 1>
<comma separated position of control point 2>
...
<number of objects to add to scene>
<object1 type>
<object1 position>
<object1 orientation>
<object1 scale>
<object2 type>
...

Note that the program is only currently expected to use one surface. Using multiple surfaces together as it currently stands may result in unexpected behavior.

The team was relatively unstructured in terms of who was assigned to each task. The breakdown
ended up resembling:
Kepler - Create, define, and render bezier track, draw vehicles.
Robert - Manage file handling, texture generation, and objects.
Gino - Camera management, views, and directions.
Coleman - Freeroaming movement, bezier surface interaction.
Again, these are very loose approximations of the group structure; everyone worked on whatever
needed to be done that they thought they could help with.

As a very rough and undocumented approximation, the project seems to have required in the range
of 60-80 man-hours.

The labs were helpful with this assignment, although the previous assignments were much more
useful. Large portions of other programs written for the course could be recycled, so the
previous assignments were 10/10 useful for completing this assignment.

This assignment was in a lot of ways enjoyable, and the finished product felt very cool to have
working. However, it was a lot of work which was often very tiring, so the assignment gets an
8/10 for fun.
# LowPoly #

This is a simple utility for converting various images (png, jpg, bmp, and similar) to a colorful triangle mesh for the aesthetic. The user specifies the image file to use, and the desired "grid square size", and then generates the image. The user can then take a screenshot to save the new image or tell the program to save the serialized triangle data to a text file. Some examples are below:

![example10by10](/example10by10.png)

A render of a galaxy image with size 10 by 10.

![example5by5](/example5by5.png)

A render of the same image with size 5 by 5, the smaller the grid size the better the resolution.

![example1by1](/example1by1.png)

A render again but with size 1 by 1, there is a cool pixel sorting effect at small sizes (its not actually intended).

## Usage ##

Requires C++11 and SFML graphics library. Tested with Visual Studio 2017 and SFML 2.5.1 but compilation should work with newer versions and on UNIX/macOS as well.

About the grid size and file saving: When the program generates a triangle mesh, this is really just a 2D grid where two triangles are placed into each of the grid squares. The coordinates of the actual grid points are randomized just enough to make it appear like the triangles are all just randomly placed. The reason for this is that this technique is very simple and trying to place every triangle at random would require much more computation and you would probably not get full coverage of the image. When the program asks for the grid square size, this is the size in pixels of each of these grid squares (the width and height can also be different, it doesn't need to be square). Also, I should mention how the file saving works. When you hit Ctrl+S to save an image, it gets saved to a plain-text file where the format is: grid width and height, point count, a list of all points from top left going to the right and then down, triangle count, and a list of all colors for triangles in RGB format. This feature is really only for another project I was working on at the time and you probably won't find it very useful.
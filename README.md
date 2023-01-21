# Spline Curve Mesh

The main program is the surfaceModeller.cpp program, this has the spline curve and various other features (such as exporting mesh, drawing as lines/normals/points, etc). 
 - This program needs 3 files: surfaceModeller.cpp, surfaceModeller.h, and subdivcurve.h. 
 - Along with these, you will need the vertex and fragment shader files in the same directory.

The second program is the meshViewer.cpp program, it imports the mesh created by the main program and renders it.
 - This program consists of only the file meshViewer.cpp.
 - The input mesh file must be called 'outputMesh.txt' (main program exports this file name) and be in the same directory.
 - Along with this file, you will need the vertex and fragment shader files in the same directory.

Drawing Style: (surfaceModeller.cpp):
 - 'l' and 'L' key toggles line drawing, 
 - 'n' and 'N' key toggles normal vector drawing, 
 - 'p' and 'P' key toggles points

Camera Control: (available in both programs)
 - holding down the Left Mouse Button (and dragging) will move the camera around the y-axis (horizontal)
 - holding down the Right Mouse Button (and dragging) will change the camera's elevation (vertical)
 - mouse scroll wheel is used for zooming in/out

Mesh Rendering:
 - Both programs use VAO/VBOs to render the quads.

Mesh Export: (surfaceModeller.cpp)
 - 's' and 'S' key on the main program will export the VBOs into a file called 'outputMesh.txt' (in the same directory)
 - upon export, the string "outputMesh.txt created" will be outputted on the terminal

Mesh Import: (meshViewer.cpp)
 - The program will look for the file 'outputMesh.txt' in the current directory and will open it and populate the VBOs to render the object.

Vertex Shader: 'vertexShader.vert'
Fragment Shader: 'fragmentShader.frag'
Both of these files are used for both programs and must be included in the same directory for the programs to run properly.

'q', 'Q', and 'Esc' exit the program.

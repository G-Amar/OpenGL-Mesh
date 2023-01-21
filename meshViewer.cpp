#include <stdio.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#define M_PI 3.14159265358979323846

GLint glutWindowWidth = 800;
GLint glutWindowHeight = 600;
GLint viewportWidth = glutWindowWidth;
GLint viewportHeight = glutWindowHeight;

// screen window identifiers
int window3D;

int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;


GLuint progID;
GLuint vertID;
GLuint fragID;
GLint vertexLoc;
GLint colorLoc;

void printShaderInfoLog(GLuint obj) {
	int infoLogLength = 0;
	int returnLength = 0;
	char* infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		infoLog = (char*)malloc(infoLogLength);
		glGetShaderInfoLog(obj, infoLogLength, &returnLength, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj) {
	int infoLogLength = 0;
	int returnLength = 0;
	char* infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		infoLog = (char*)malloc(infoLogLength);
		glGetProgramInfoLog(obj, infoLogLength, &returnLength, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

std::string loadShaderSrc(const std::string& filename) {
	std::ifstream is(filename);
	if (is.is_open()) {
		std::stringstream buffer;
		buffer << is.rdbuf();
		return buffer.str();
	}
	cerr << "Unable to open file " << filename << endl;
	exit(1);
}

void setupShaders() {
	GLenum err = glewInit();
	// create shader
	vertID = glCreateShader(GL_VERTEX_SHADER);
	fragID = glCreateShader(GL_FRAGMENT_SHADER);

	// load shader source from file
	std::string vs = loadShaderSrc("vertexShader.vert");
	const char* vss = vs.c_str();
	std::string fs = loadShaderSrc("fragmentShader.frag");
	const char* fss = fs.c_str();

	// specify shader source
	glShaderSource(vertID, 1, &(vss), NULL);
	glShaderSource(fragID, 1, &(fss), NULL);

	// compile the shader
	glCompileShader(vertID);
	glCompileShader(fragID);

	// check for errors
	printShaderInfoLog(vertID);
	printShaderInfoLog(fragID);

	// create program and attach shaders
	progID = glCreateProgram();
	glAttachShader(progID, vertID);
	glAttachShader(progID, fragID);


	// link the program
	glLinkProgram(progID);
	// output error messages
	printProgramInfoLog(progID);


}


double* vertices;
double* normals;
int numIndices;
int numVertices;
GLuint* indices;

//build VBOs with imported file outputMesh.txt
void buildVBOs() {
	FILE* fp;
	
	/* for taking input string as filename
	char* nl;

	char filename[102];
	printf("Enter a filename (max 100 chars):\n");
	if(fgets(filename, sizeof(filename), stdin) == NULL) {
		printf("Input Error\n");
		exit(1);
	}
	//remove trailing '\n' char
	if ((nl = strchr(filename, '\n')) != NULL) *nl = '\0';

	if (fopen_s(&fp, filename, "r") != 0) {
		printf("Error opening file %s or doesn't exist!\n", filename);
		exit(1);
	}
	*/

	printf("Opening file outputMesh.txt for mesh viewing!\n");
	if (fopen_s(&fp, "outputMesh.txt", "r") != 0) {
		printf("Error opening file outputMesh.txt or doesn't exist!\n");
		exit(1);
	}

	fscanf_s(fp, "%d", &numVertices);

	vertices = (double*) malloc(numVertices * sizeof(double));
	normals = (double*) malloc(numVertices * sizeof(double));
	
	for (int i = 0; i < numVertices; i+=3) {
		fscanf_s(fp, "%lf %lf %lf", &vertices[i], &vertices[i+1], &vertices[i+2]);
	}
	for (int i = 0; i < numVertices; i += 3) {
		fscanf_s(fp, "%lf %lf %lf", &normals[i], &normals[i + 1], &normals[i + 2]);
	}

	fscanf_s(fp, "%d", &numIndices);

	indices = (GLuint*)malloc(numIndices * sizeof(GLuint));

	for (int i = 0; i < numIndices; i += 4) {
		fscanf_s(fp, "%u %u %u %u", &indices[i], &indices[i + 1], &indices[i + 2], &indices[i + 3]);
	}

	fclose(fp);
}


 // Ground Mesh material
GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };

GLfloat light_position0[] = { 4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_position1[] = { -4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

//
// Surface of Revolution consists of vertices and quads
//
// Set up lighting/shading and material properties for quadMesh
GLfloat quadMat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.1, 0.35, 0.1, 1.0 };
GLfloat quadMat_shininess[] = { 10.0 };


GLdouble fov = 60.0;

int lastMouseX;
int lastMouseY;

GLdouble eyeX = 0.0, eyeY = 3.0, eyeZ = 10.0;
GLdouble radius = eyeZ;
GLdouble zNear = 0.1, zFar = 40.0;

void init3DSurfaceWindow()
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	glViewport(0, 0, (GLsizei)window3DSizeX, (GLsizei)window3DSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei)w;
	glutWindowHeight = (GLsizei)h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void drawGround()
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-22.0f, -4.0f, -22.0f);
	glVertex3f(-22.0f, -4.0f, 22.0f);
	glVertex3f(22.0f, -4.0f, 22.0f);
	glVertex3f(22.0f, -4.0f, -22.0f);
	glEnd();
	glPopMatrix();
}



void drawQuads()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);

	int stride = 3 * sizeof(double);
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_DOUBLE, stride, vertices);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, stride, normals);

	glDrawElements(GL_QUADS, numIndices, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

void display3D()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Set up the Viewing Transformation (V matrix)	
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glUseProgram(progID);

	drawGround();


	// Draw quad mesh
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);

	drawQuads();

	glPopMatrix();
	glutSwapBuffers();
}

int currentButton;

void mouseButtonHandler3D(int button, int state, int x, int y)
{
	currentButton = button;
	lastMouseX = x;
	lastMouseY = y;
	switch (button)
	{
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
		{
			// Used scroll wheel
		}
		break;
	default:
		break;
	}
}

void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse)
{
	// Fill in this code for zooming in and out
	if (dir > 0)
	{
		// Zoom in, keep radius consistent, important for moving camera
		eyeX = eyeX * 0.8;
		eyeY = eyeY * 0.8;
		eyeZ = eyeZ * 0.8;
		radius = radius * 0.8;
	}
	else
	{
		// Zoom out
		eyeX = eyeX * 1.25;
		eyeY = eyeY * 1.25;
		eyeZ = eyeZ * 1.25;
		radius = radius * 1.25;
	}
	glutPostRedisplay();

}

//create horizontal and vertical angles
int horizontalAngle = 0;
int verticalAngle = 20;

void mouseMotionHandler3D(int x, int y)
{
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;

	if (currentButton == GLUT_LEFT_BUTTON)
	{
		// Fill in this code to control camera "orbiting" around surface
		if (dx > 0) { //drag right, move camera right
			//y stays same, rotate x and z around y axis 
			horizontalAngle += 3;
			if (horizontalAngle > 360)
				horizontalAngle -= 360;
		}
		else if (dx < 0) {
			horizontalAngle -= 3;
			if (horizontalAngle < 0)
				horizontalAngle += 360;
		}
		eyeX = radius * cos(horizontalAngle / 180.0 * M_PI);
		eyeZ = radius * sin(horizontalAngle / 180.0 * M_PI);
	}
	if (currentButton == GLUT_RIGHT_BUTTON)
	{
		// Fill in this code to control camera elevation. Limit the elevation angle
		if (dy > 0) { //drag down, move camera down
			//change y so that it moves along the hemisphere 
			verticalAngle += 1;
			if (verticalAngle > 60) //prevent going over 60
				verticalAngle = 60;
		}
		else if (dy < 0) {
			verticalAngle -= 1;
			if (verticalAngle < 0) //prevent going below 0
				verticalAngle = 0;
		}
		eyeY = radius * sin(verticalAngle / 180.0 * M_PI);
	}
	else if (currentButton == GLUT_MIDDLE_BUTTON)
	{
		// Used the scroll wheel
	}
	lastMouseX = x;
	lastMouseY = y;
	glutPostRedisplay();
}

void keyboardHandler3D(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'q':
	case 'Q':
	case 27:
		// Esc, q, or Q key = Quit 
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutWindowWidth, glutWindowHeight);
	glutInitWindowPosition(600, 100);

	// The 3D Window
	window3D = glutCreateWindow("Surface of Revolution");
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutMouseFunc(mouseButtonHandler3D);
	glutMouseWheelFunc(mouseScrollWheelHandler3D);
	glutMotionFunc(mouseMotionHandler3D);
	glutKeyboardFunc(keyboardHandler3D);
	// Initialize the 3D system
	init3DSurfaceWindow();

	//setup vertex and fragment shaders
	setupShaders();

	//build the VBOs from file outputMesh.txt once at the beginning
	buildVBOs();

	// Annnd... ACTION!!
	glutMainLoop();

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <string>
#include <cmath> 
#include <time.h>
#include <vector>
#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#include "osusphere.cpp"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Ian Collier

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Sample -- Ian Collier" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b

// initial window size:

const int INIT_WINDOW_SIZE = { 600 };

// size of the 3d box:

const float BOXSIZE = { 2.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };

// equivalent mouse movement when we click a the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };



// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

std::vector<std::string> Phrases;

// my globals
const float MOUTH_Y_CLOSED = -1.35;
const float MOUTH_Y_OPEN = -2;
const float MOUTH_Y_SPEED_DEFAULT = 0.04;
const int FIXED_TEXT_APPEND_SPEED = 5;
const int SPEAK_OFF_DELAY = 80;
const float LOOK_AT_SPEED = 0.02;
const float LOOK_AT_CUTOFF = 0.05;
const float LOOK_AT_DEFAULT[3] = {0., 0., 7.};
const float LOOK_AT_Z_ORTHO_DEFAULT = 20.;
const float LOOK_AT_DISTANCE = 4.;
const float EYES_Y_SPEED_DEFAULT = 0.1;
const float EYES_Y_OPEN = 0.75;
const float EYES_Y_CLOSED = 0.1;
const float EYES_COLOR_DEG_DEFAULT = 70.;
const float EYES_COLOR_DEG_INC = 45.;
const float GLANCE_RANDOM = 0.75;
const std::string PHRASES_FILE = "phrases.txt";

int Light0On;
float AnimateC;
GLuint Tex0;
int ScrollZoomOn;
int WindowHeight, WindowWidth;

int MouthAnimOn, MouthAnimOffReq;
float MouthY, MouthYSpeed;

int FixedTextTimer, FixedTextCurChar, FixedTextAppendOn, FixedTextOn;
std::string FixedTextStr, FixedTextDisplayStr;

int SpeakOffReq, SpeakOn;
int SpeakOffTimer;

float LookAtTarget[3], LookAtCur[3];
int LookAtOn;


int EyesAnimOn;
float EyesY, EyesYSpeed, EyesColorDeg, EyesColorTarget;

int BlinkTimer, BlinkNext, SpeakTimer, SpeakNext, GlanceTimer, GlanceNext;

int ColorAtX, ColorAtY, ColorAtReq;

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoShadowMenu();
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
unsigned char *	BmpToTexture( char *, int *, int * );
void			HsvRgb( float[3], float [3] );
int				ReadInt( FILE * );
short			ReadShort( FILE * );

void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);

// My utils
void SetMaterial(float, float, float, float);
void SetPointLight(int, float, float, float, float, float, float);
void SetSpotLight(int, float, float, float,  float, float, float, float, float, float);
void BindTex(GLuint*, const char*, GLint, GLint);
void DoRasterStringCpp(float, float, float, std::string);

float RandomFloat(float, float);
int RandomInt(int, int);
void FaceSpeak(std::string);
void FaceLookAt(float, float);
void FaceBlink();
void FaceIncEyeColor(float);
void FaceSpeakRandomLine();
void FaceGlanceRandom();
void BlinkTimerReset();
void GlanceTimerReset();
void SpeakTimerReset();
void MouseEntry(int);

void LoadPhrasesFile(std::string filename) {
    std::ifstream phrases_fp(filename);
	std::string phrases_line;

    if(!phrases_fp) {
        fprintf(stderr, "Phrases file '%s' not found\n", filename.c_str());
		Phrases.push_back("Phrases file not found.");
    }
    while (std::getline(phrases_fp, phrases_line)) {
        Phrases.push_back(phrases_line);
    }
	if(Phrases.size() <= 0) {
		fprintf(stderr, "Phrases file '%s' is empty.\n", filename.c_str());
		Phrases.push_back("Phrases file is empty.");
	}
}

// main program:

int
main( int argc, char *argv[ ] )
{
	srand (time(NULL));
	LoadPhrasesFile(PHRASES_FILE);
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never returns
	// this line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	if (MouthAnimOn) {
		MouthY += MouthYSpeed;
		if(MouthY <= MOUTH_Y_OPEN) {
			// Reverse at mouth open
			MouthYSpeed *= -1;
		} else if (MouthY >= MOUTH_Y_CLOSED) {
			// Reverse at mouth closed
			MouthYSpeed *= -1;
			// Stop animation if requested next time mouth is closed
			if (SpeakOffReq && SpeakOffTimer >= SPEAK_OFF_DELAY) {
				MouthAnimOn = 0;
				SpeakOffReq = 0;
				FixedTextOn = 0;
				SpeakOn = 0;
			}
		}
	}

	// Append character to fixed text at an interval
	if(FixedTextAppendOn) {
		++FixedTextTimer;
		if(FixedTextTimer >= FIXED_TEXT_APPEND_SPEED) {
			if(FixedTextCurChar >= FixedTextStr.length()) {
				// Stop appending and end speech when whole string is drawn
				FixedTextAppendOn = 0;
				SpeakOffReq = 1;
				SpeakOffTimer = 0;
			}
			// Append one character at a time
			FixedTextTimer = 0;
			FixedTextDisplayStr += FixedTextStr[FixedTextCurChar];
			++FixedTextCurChar;
		}
	}

	if(SpeakOffReq) {
		++SpeakOffTimer;
	}

	if(LookAtOn) {
		//LookAtCur[1] += LOOK_AT_SPEED;
		float x_dist = LookAtTarget[0] - LookAtCur[0];
		float y_dist = LookAtTarget[1] - LookAtCur[1];

		LookAtCur[0] += x_dist * LOOK_AT_SPEED;
		LookAtCur[1] += y_dist * LOOK_AT_SPEED;
		/*
		if(std::abs(y_dist) < LOOK_AT_CUTOFF) {
			LookAtCur[0] = LookAtTarget[0];
		}
		if(std::abs(x_dist) < LOOK_AT_CUTOFF) {
			LookAtCur[1] = LookAtTarget[1];
		}
		*/
	}
	
	if(EyesAnimOn) {
		EyesY += EyesYSpeed;
		if (EyesY <= EYES_Y_CLOSED) {
			EyesYSpeed *= -1;
			EyesColorDeg = EyesColorTarget;
		} else if (EyesY >= EYES_Y_OPEN) {
			EyesYSpeed = 0.;
			EyesAnimOn = 0;
		}
	}

	++BlinkTimer;
	if(BlinkTimer >= BlinkNext) {
		FaceBlink();
		BlinkTimerReset();
	}

	++SpeakTimer;
	if(SpeakTimer >= SpeakNext) {
		GlanceTimer = 0;
		FaceSpeakRandomLine();
		SpeakTimerReset();
	}

	++GlanceTimer;
	if(GlanceTimer >= GlanceNext) {
		FaceGlanceRandom();
		GlanceTimerReset();
	}

	// force a call to Display( ) next time it is convenient:
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

float RandomFloat(float min, float max){
   return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

int RandomInt(int min, int max){
   return (rand() % (max - min + 1)) + min;
}

void FaceSpeak(std::string s) {
	SpeakOffReq = 0;
	MouthAnimOn = 1;
	SpeakOn = 1;
	FixedTextCurChar = 0;
	FixedTextAppendOn = 1;
	FixedTextOn = 1;
	FixedTextStr = s;
	FixedTextDisplayStr = "";
}

void FaceLookAt(float x, float y) {
	LookAtOn = 1;
	LookAtTarget[0] = x * LOOK_AT_DISTANCE;
	LookAtTarget[1] = y * LOOK_AT_DISTANCE;
}

void FaceBlink() {
	EyesAnimOn = 1;
	EyesYSpeed = -EYES_Y_SPEED_DEFAULT;
}

void FaceIncEyeColor(float deg_inc) {
	EyesColorTarget = EyesColorDeg + deg_inc;
	FaceBlink();
}

void FaceGlanceRandom() {
	FaceLookAt(RandomFloat(-GLANCE_RANDOM, GLANCE_RANDOM), RandomFloat(-GLANCE_RANDOM, GLANCE_RANDOM));
}

void FaceSpeakRandomLine() {
	SpeakTimer = 0;
	int phrases_size = Phrases.size();
	if(!SpeakOn && phrases_size > 0) {
		int phrase_i = RandomInt(0, phrases_size - 1);
		FaceSpeak(Phrases[phrase_i]);
	}
}

void BlinkTimerReset() {
	BlinkTimer = 0;
	BlinkNext = RandomInt(250, 300);
}

void SpeakTimerReset() {
	SpeakTimer = 0;
	SpeakNext = RandomInt(2000, 3000);
	
}

void GlanceTimerReset() {
	GlanceTimer = 0;
	GlanceNext = RandomInt(500, 800);
}

void MouseEntry(int state) {
	if(state == GLUT_LEFT) {
		FaceLookAt(0., 0.);
	}
}

// draw the complete scene:

void
Display( )
{


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	WindowWidth = vx;
	WindowHeight = vy;
	
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// set the eye position, look-at position, and up-vector:

	gluLookAt( LookAtCur[0], LookAtCur[1], LookAtCur[2],     0., 0., 0.,     0., 1., 0. );


	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );


	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );

	// Light 1: fixed point light
	glPushMatrix();
		if(Light0On) {
			glColor3f(1., 1., 1.);
		} else {
			glColor3f(.2, .2, .2);
		}
		glTranslatef(LookAtCur[0], LookAtCur[1], LookAtCur[2]);
		SetPointLight(GL_LIGHT0, 0., 0., 0., 0.5, .5, .5);
		//OsuSphere(.1, 10, 10);
	glPopMatrix();

	// Enable lighting
	glEnable(GL_LIGHTING);
	if(Light0On) {
		glEnable(GL_LIGHT0);
	} else {
		glDisable(GL_LIGHT0);
	}
	
	glShadeModel( GL_SMOOTH );

	
	SetMaterial(1., 0., 0., 5.);

	// Main head
	glPushMatrix();
		glTranslatef(0., 0.75, -3.);
		glScalef(1.25, 0.75, 1.);
		glutSolidCube(6.);
	glPopMatrix();

	// Right cheekbone
	glPushMatrix();
		glTranslatef(2.5, -2., -3.);
		glScalef(0.5, 0.75, 2.15);
		glutSolidCube(3.);
	glPopMatrix();

	// Left cheekbone
	glPushMatrix();
		glTranslatef(-2.5, -2., -3.);
		glScalef(0.5, 0.75, 2.15);
		glutSolidCube(3.);
	glPopMatrix();
	
	// Hair
	glPushMatrix();
		glTranslatef(0., 3., -3.);
		glScalef(1.25, 0.25, 1.);
		glutSolidCube(5.);
	glPopMatrix();

	// Jaw
	SetMaterial(0.5, 0., 0., 5.);
	glPushMatrix();
		glTranslatef(0., -2., -3.5);
		glScalef(1.25, 0.70, 1.75);
		glutSolidCube(3.);
	glPopMatrix();

	// Nose
	SetMaterial(0., 1., 1., 5.);
	glPushMatrix();
		glTranslatef(0., 0.5, 0.);

		// Nose bridge
		glPushMatrix();
			//glScalef(1., 1., 1.);
			glTranslatef(0., 0.5, 0.);
			glutSolidCube(1.);
		glPopMatrix();

		// Nostrils
		glPushMatrix();
			glScalef(2., 1., 1.);
			glTranslatef(0., -0.5, 0.5);
			glutSolidCube(1.);
		glPopMatrix();
	glPopMatrix();

	// Eyes
	float eyes_color[] = {0., 0., 0.};
	float eyes_hsv[] = {EyesColorDeg, 1., 1.};
	HsvRgb(eyes_hsv, eyes_color);
	SetMaterial(eyes_color[0], eyes_color[1], eyes_color[2], 5.);
	// Right eye
	glPushMatrix();
		glTranslatef(2.15, 1.9, 0.);
		glScalef(1.25, EyesY, 1.);
		glutSolidCube(1.);
	glPopMatrix();

	// Left eye
	glPushMatrix();
		glTranslatef(-2.15, 1.9, 0.);
		glScalef(1.25, EyesY, 1.);
		glutSolidCube(1.);
	glPopMatrix();

	// Chin
	SetMaterial(1., .5, 0., 5.);
	glPushMatrix();
		glScalef(3.5, 1.75, 1.25);
		glTranslatef(0., MouthY, -0.25);
		glutSolidCube(1.);
	glPopMatrix();

	glDisable(GL_LIGHTING);

	// Draw fixed text
	if(FixedTextOn) {
		glDisable( GL_DEPTH_TEST );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		gluOrtho2D( 0., 100., 0., 100. );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity( );
		glColor3f( 1., 1., 1. );
		DoRasterStringCpp( 5., 5., 0., FixedTextDisplayStr );
	}

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();

	// Detect if object is clicked on
	if(ColorAtReq) {
		ColorAtReq = 0;
		// Get color at mouse xy
		glFinish();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		unsigned char fb_pixel_color[4];
		glReadPixels(ColorAtX, ColorAtY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, fb_pixel_color);
		// If the color is not the same as the clear color it's a hit
		if(BACKCOLOR[0] != fb_pixel_color[0] && BACKCOLOR[1] != fb_pixel_color[1] && BACKCOLOR[2] != fb_pixel_color[2] && BACKCOLOR[3] != fb_pixel_color[3]) {
			FaceSpeakRandomLine();
		}
		if(DebugOn) fprintf(stderr, "Color at (%d, %d): [%u, %u, %u, %u]\n", ColorAtX, ColorAtY, fb_pixel_color[0], fb_pixel_color[1], fb_pixel_color[2], fb_pixel_color[3]);
	}
}

void
DoScrollZoomMenu( int id )
{
	ScrollZoomOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;
	if(WhichProjection == ORTHO) {
		LookAtCur[2] = LOOK_AT_Z_ORTHO_DEFAULT;
	} else {
		LookAtCur[2] = LOOK_AT_DEFAULT[2];
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoShadowsMenu(int id)
{
	ShadowsOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}

// Wrapper function for C++ strings

void
DoRasterStringCpp( float x, float y, float z, std::string cpp_str )
{
	// Create a mutable C string copy
	char *s = new char[cpp_str.size() + 1];
	std::copy(cpp_str.begin(), cpp_str.end(), s);
	s[cpp_str.size()] = '\0';
	DoRasterString(x, y, z, s);
	delete[] s;
}

// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int scrollzoommenu = glutCreateMenu( DoScrollZoomMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int shadowsmenu = glutCreateMenu(DoShadowsMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Scroll Zoom",   scrollzoommenu);

	glutAddSubMenu(   "Axes",          axesmenu);
	//glutAddSubMenu(   "Colors",        colormenu);
	//glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( MouseEntry );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	BindTex(&Tex0, "proj4lib/worldtex.bmp", GL_REPEAT, GL_LINEAR);
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	glutSetWindow( MainWindow );

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'f':
		case 'F':
			MouthAnimOffReq = 1;
			break;

		case '0':
			Light0On =! Light0On;
			break;

		case (char)0x3: // Ctrl-C
		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			ColorAtReq = 1;
			ColorAtX = x;
			ColorAtY = y;
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			//FaceIncEyeColor(EYES_COLOR_DEG_INC);
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			if (ScrollZoomOn) {
				Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
				// keep object from turning inside-out or disappearing:
				if (Scale < MINSCALE)
					Scale = MINSCALE;
			}
			break;

		case SCROLL_WHEEL_DOWN:
			if (ScrollZoomOn) {
				Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
				// keep object from turning inside-out or disappearing:
				if (Scale < MINSCALE)
					Scale = MINSCALE;
			}
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	//if( DebugOn )
		//fprintf( stderr, "MouseMotion: %d, %d\n", x, y );

	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if(WindowWidth > 0 && WindowHeight > 0) {
		float x_rel = (x / (float)WindowWidth - 0.5) * 2;
		float y_rel = (y / (float)WindowHeight - 0.5) * -2;
		FaceLookAt(-x_rel, -y_rel);
		GlanceTimer = 0;
	}

	/*
	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	*/
	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	AnimateC = .0;
	MouthY = MOUTH_Y_CLOSED;
	MouthYSpeed = -MOUTH_Y_SPEED_DEFAULT;
	Light0On = 1;
	MouthAnimOn = 0; 
	MouthAnimOffReq = 0;
	FixedTextTimer = FixedTextCurChar = 0;
	FixedTextStr = FixedTextDisplayStr = "";
	FixedTextAppendOn = FixedTextOn = 0;
	SpeakOffReq = SpeakOffTimer = 0;
	LookAtCur[0] = LOOK_AT_DEFAULT[0];
	LookAtCur[1] = LOOK_AT_DEFAULT[1];
	LookAtCur[2] = LOOK_AT_DEFAULT[2];
	std::fill(LookAtTarget, LookAtTarget + 3, 0);
	LookAtOn = 0;
	WindowHeight = WindowWidth = 0;
	EyesY = EYES_Y_OPEN;
	EyesYSpeed = EYES_Y_SPEED_DEFAULT;
	EyesAnimOn = 0;
	EyesColorDeg = EYES_COLOR_DEG_DEFAULT;
	EyesColorTarget = EYES_COLOR_DEG_DEFAULT;
	BlinkTimerReset();
	SpeakTimerReset();
	GlanceTimerReset();
	ColorAtX = ColorAtY = ColorAtReq = 0;
	ScrollZoomOn = 0;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}

// My utils
float *Array3( float a, float b, float c ) {
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

float *MulArray3( float factor, float array0[3] ) {
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void SetMaterial( float r, float g, float b,  float shininess ) {
	static float white[3] = {1., 1., 1.};

	glMaterialfv( GL_BACK, GL_EMISSION, Array3( 0., 0., 0. ) );
	glMaterialfv( GL_BACK, GL_AMBIENT, MulArray3( .4f, white ) );
	glMaterialfv( GL_BACK, GL_DIFFUSE, MulArray3( 1., white ) );
	glMaterialfv( GL_BACK, GL_SPECULAR, Array3( 0., 0., 0. ) );
	glMaterialf (  GL_BACK, GL_SHININESS, 2.f );

	glMaterialfv( GL_FRONT, GL_EMISSION, Array3( 0., 0., 0. ) );
	glMaterialfv( GL_FRONT, GL_AMBIENT, Array3( r, g, b ) );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, Array3( r, g, b ) );
	glMaterialfv( GL_FRONT, GL_SPECULAR, MulArray3( .8f, white ) );
	glMaterialf ( GL_FRONT, GL_SHININESS, shininess );
}

void SetPointLight( int ilight, float x, float y, float z,  float r, float g, float b ) {
	glLightfv( ilight, GL_POSITION,  Array3( x, y, z ) );
	glLightfv( ilight, GL_AMBIENT,   Array3( 0., 0., 0. ) );
	glLightfv( ilight, GL_DIFFUSE,   Array3( r, g, b ) );
	glLightfv( ilight, GL_SPECULAR,  Array3( r, g, b ) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1. );
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0. );
	glEnable( ilight );
}

void SetSpotLight( int ilight, float x, float y, float z,  float xdir, float ydir, float zdir, float r, float g, float b ) {
	glLightfv( ilight, GL_POSITION,  Array3( x, y, z ) );
	glLightfv( ilight, GL_SPOT_DIRECTION,  Array3(xdir,ydir,zdir) );
	glLightf(  ilight, GL_SPOT_EXPONENT, 1. );
	glLightf(  ilight, GL_SPOT_CUTOFF, 45. );
	glLightfv( ilight, GL_AMBIENT,   Array3( 0., 0., 0. ) );
	glLightfv( ilight, GL_DIFFUSE,   Array3( r, g, b ) );
	glLightfv( ilight, GL_SPECULAR,  Array3( r, g, b ) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1. );
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0. );
	glEnable( ilight );
}

void BindTex(GLuint* tex, const char* bmp_file, GLint tex_wrap, GLint tex_filter) {
	int tex_w, tex_h;
	unsigned char* tex_arr = BmpToTexture((char*)bmp_file, &tex_w, &tex_h);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_filter);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_arr);
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}

// read a BMP file into a Texture:

#define VERBOSE		false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB			0
#define BI_RLE8			1
#define BI_RLE4			2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;



// read a BMP file into a Texture:

unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{
	FILE *fp;
#ifdef _WIN32
        errno_t err = fopen_s( &fp, filename, "rb" );
        if( err != 0 )
        {
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
        }
#else
	fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if( VERBOSE ) fprintf( stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
			FileHeader.bfType, FileHeader.bfType&0xff, (FileHeader.bfType>>8)&0xff );
	if( FileHeader.bfType != BMP_MAGIC_NUMBER )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}


	FileHeader.bfSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize );

	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );

	FileHeader.bfOffBytes = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfOffBytes = %d\n", FileHeader.bfOffBytes );


	InfoHeader.biSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSize = %d\n", InfoHeader.biSize );
	InfoHeader.biWidth = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biWidth = %d\n", InfoHeader.biWidth );
	InfoHeader.biHeight = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biHeight = %d\n", InfoHeader.biHeight );

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biPlanes = %d\n", InfoHeader.biPlanes );

	InfoHeader.biBitCount = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount );

	InfoHeader.biCompression = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression );

	InfoHeader.biSizeImage = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage );

	InfoHeader.biXPixelsPerMeter = ReadInt( fp );
	InfoHeader.biYPixelsPerMeter = ReadInt( fp );

	InfoHeader.biClrUsed = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed );

	InfoHeader.biClrImportant = ReadInt( fp );


	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );


	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char *texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\n" );
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ( ( InfoHeader.biBitCount*InfoHeader.biWidth + 31 ) / 32 );
	if( VERBOSE )	fprintf( stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes );

	int myRowSizeInBytes = ( InfoHeader.biBitCount*InfoHeader.biWidth + 7 ) / 8;
	if( VERBOSE )	fprintf( stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes );

	int oldNumExtra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;
	if( VERBOSE )	fprintf( stderr, "Old NumExtra padding = %d\n", oldNumExtra );

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if( VERBOSE )	fprintf( stderr, "New NumExtra padding = %d\n", numExtra );


	// this function does not support compression:

	if( InfoHeader.biCompression != 0 )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}
	

	// we can handle 24 bits of direct color:
	if( InfoHeader.biBitCount == 24 )
	{
		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if( InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256 )
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32 *colorTable = new struct rgba32[ InfoHeader.biClrUsed ];

		rewind( fp );
		fseek( fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET );
		for( int c = 0; c < InfoHeader.biClrUsed; c++ )
		{
			colorTable[c].r = fgetc( fp );
			colorTable[c].g = fgetc( fp );
			colorTable[c].b = fgetc( fp );
			colorTable[c].a = fgetc( fp );
			if( VERBOSE )	fprintf( stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a );
		}

		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				int index = fgetc( fp );
				*(tp+0) = colorTable[index].r;	// r
				*(tp+1) = colorTable[index].g;	// g
				*(tp+2) = colorTable[index].b;	// b
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}

		delete[ ] colorTable;
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	const unsigned char b2 = fgetc( fp );
	const unsigned char b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short
ReadShort( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}

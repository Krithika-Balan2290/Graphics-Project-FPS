/*
 *  CSCI 5229 - Final Project
 *
 *  First Person Shooter Game
 *
 *  Key bindings:
 *  m							Switch view mode
 *  <>							Strafe left or right
 *  +/-							Change field of view
 *  arrows					Move forward or Backward, turn left or right
 *	spacebar				Shoot
 * s/S 							Toggle music (If background music enabled)
 *  PgDn/PgUp			Zoom in and out
 *  0								Reset view angle
 *  ESC						Exit
 */
#include "CSCIx229.h"
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

int mode=1;       //  Projection mode
int shot=0;			//	Gun just fired?
//int play_music=1;	//	Background music? (uncomment if music enabled)

int th=90;         //  Azimuth of view angle
int ph=15;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=50;   //  Size of world

// Light values
int distance  =   200;  // Light distance
int inc       =  10;  // Ball increment
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   40;  // Specular intensity (%)
int zh        =  90;  // Light azimuth
float ylight  =   23.5;  // Elevation of light
int delay = 0;		

//First Person Navigation
int cam_th=0;       //  Azimuth of camera angle
int cam_ph=0;		//  Elevation of camera angle
double Ex = 0;	//	x coordinate of eye position for First Person Navigation
double Ey = 10;	//	y coordinate of eye position for First Person Navigation
double Ez = 0;	//	z coordinate of eye position for First Person Navigation

double Height = 700;
double Width = 700;
 
GLfloat normals[9][3] = {{0,0,-1},{5,0,-9},{11,0,-7},{11,0,7},{5,0,9},{0,0,1},{-1,0,1},{-1,0,0},{-1,0,-1}};
GLfloat body_base[9][3] = {{0,0,0},{17,0,0},{22,0,2},{25,0,7},{22,0,12},{17,0,14},{0,0,14},{-3,0,11},{-3,0,3}};
GLfloat body_top[9][3] = {{0,30,0},{14,30,1},{18,30,3},{20,30,7},{18,30,11},{14,30,13},{0,30,14},{-3,30,11},{-3,30,3}};

float obj_pos_z[53][2] = {{24,46},	{24,46},	{-11,11},	{-46,-24},	{104,126},{104,126},	{104,126},	{59,81},		{9,31},		{-61,-39},	{-126,-104},	{-126,-104},	{-126,-104},	{-61,-39},		{-11,11},		{49,71},			{5.5,10.5},			{7.5,12.5},		{-2.5,2.5},	 {-7.5,-2.5},		{37.5,42.5},	{-47.5,-42.5}, {27.5,32.5},	{107.5,112.5},	{117.5,122.5},	{167.5,172.5},	{169.5,174.5},	{127.5,132.5},	{97.5,102.5},		{27.5,32.5},		{-102.5,-97.5},	{-152.5,-147.5}, {-154.5,-149.5}, {-172.5,-167.5},	{-117.5,-112.5},{-107.5,-102.5},{-119.5,-114.5},  {-132.5,-127.5},{-167.5,-162.5},	{-104.5,-99.5},	{-12.5,-7.5},	 {-22.5,-17.5},	{65.5,70.5},	 {107.5,112.5},	{157.5,162.5},{137.5,142.5}, {12.5,17.5},		{-22.5,-17.5},		{-43,-30},{50,63},	{-77,-69},{-10,3}, {84,92}};
float obj_pos_x[53][2] = {{-31,-9},	{9,31},	{24,46},		{-26,-4},		{-90,-69},	{-31,-9},		{39,61},			{104,126}, {104,126},{104,126},	{39,61},			{-21,1},			{-71,-49},		{-126,-104},	{-126,-104},	{-126,-104},	{-20.5,-15.5},	{-10.5,-5.5},	{-12.5,-7.5},{-47.5,-42.5},	{-47.5,-42.5}, {22.5,27.5},	{42.5,47.5},	{27.5,32.5},		{2.5,7.5},			{-52.5,-47.5},	{-132.5,-127.5},{-122.5,-117.5},{-152.5,-147.5},{-112.5,-107.5},{-127.5,-122.5},{-99.5,-104.5}, {-160.5,-155.5}, {-42.5,-37.5},			{22.5,27.5},		 {77.5,82.5},		 {81.5,85.5},		 	{97.5,102.5},	 {117.5,122.5},		{162.5,167.5},	{102.5,107.5},{147.5,152.5},	{142.5,147.5},{87.5,92.5}, {82.5,87.5},	 {157.5,162.5}, {-170.5,-165.5}, {-142.5,-137.5},	{-90,-83},{-67,-60},	{32,45},{83,90},{20,33}};

float dalek_range_z[5][2] = {{-30,0},	 {20,50},	{-77,-69},	{-40,-10}, {83,92}};
float dalek_range_x[5][2] = {{-90,-83},{-67,-60},	{45,75},		{83,90},{-10,20}};

float shot_range_z[5][2] = {{-30,0},	 {20,50},	{-75,-71},	{-40,-10}, 	{85.5,89.5}};
float shot_range_x[5][2] = {{-88.5,-84.5},{-65.5,-61.5},	{45,75},		{84.5,88.5},		{-10,20}};

int disp_dalek[5] = {1,1,1,1,1};
float opacity_dalek[5] = {1.0,1.0,1.0,1.0,1.0};

unsigned int texture[9];

//Mix_Music* music; (uncomment if adding background music)
Mix_Chunk* gunshot;

void idle();

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z, double dx,double dy,double dz, double th, double ph, int tex)
{
   //  Set specular color to white
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);
   
   //Enable Texture is needed
   if (tex >= 0)
   {
	   glEnable(GL_TEXTURE_2D);
	   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	   glBindTexture(GL_TEXTURE_2D, texture[tex]);
   }
   else
	   glDisable(GL_TEXTURE_2D);
   
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1,-1, 1);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1,-1, 1);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1,+1, 1);
   //  Back
   
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0.0,0.0);	glVertex3f(+1,-1,-1);
   glTexCoord2f(1.0,0.0);	glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,1.0);	glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,1.0);	glVertex3f(+1,+1,-1);
   //  Right
  
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(+1,-1,+1);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1,-1,-1);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,1.0);	glVertex3f(+1,+1,+1);
   //  Left
   
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,0.0);	glVertex3f(-1,-1,+1);
   glTexCoord2f(1.0,1.0);	glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1,+1,-1);
   //  Top
  
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1,+1,+1);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1,+1,+1);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1,+1,-1);
   //  Bottom
   
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1,-1,-1);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1,-1,+1);
   
   //  End
   glEnd();
   
   // Done with texture
   if (tex >= 0)
	   glDisable(GL_TEXTURE_2D);
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x,double y,double z,double r)
{
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

static void hemisphere(double x,double y,double z,double r,double angle_x, double angle_y)
{
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(angle_y,0,1,0);
   glRotated(angle_x,1,0,0);
   glScaled(r,r,r);
   //  Bands of latitude
   for (ph=0;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

void cylinder(double xx, double yy, double zz, double dx, double dy, double dz,	double th, double ph,	GLfloat r, GLfloat h, int tex)
{

	//	Save Transformations
    glPushMatrix();
	
	//	Offset, Rotate and Scale
    glTranslated(xx,yy,zz);
    glRotated(th,0,1,0);
    glRotated(ph,1,0,0);
    glScaled(dx,dy,dz);
	
	glPushMatrix();
	
	//	Offset, Rotate and Scale
    glScaled(r,r,h);
	
    GLint angle    = 0;
    GLint stepsize = 2;

	 if (tex >= 0)
   {
	   glEnable(GL_TEXTURE_2D);
	   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	   glBindTexture(GL_TEXTURE_2D, texture[tex]);
   }
   else
	   glDisable(GL_TEXTURE_2D);
   
    // Body of cylinder
    glBegin(GL_QUADS);
    for (angle=0; angle<360; angle+=stepsize)
    {
		glNormal3f(Cos(angle+(stepsize/2)), Sin(angle+(stepsize/2)) , 0.0);
		
        glTexCoord2f(0.0,1.0);	glVertex3f(Cos(angle),Sin(angle),0);
		glTexCoord2f(0.0,0.0);	glVertex3f(Cos(angle),Sin(angle),1);
		glTexCoord2f(1.0,0.0);	glVertex3f(Cos(angle+stepsize),Sin(angle+stepsize),1);
		glTexCoord2f(1.0,1.0);	glVertex3f(Cos(angle+stepsize),Sin(angle+stepsize),0);
    }
    glEnd();
	
	glDisable(GL_TEXTURE_2D);

    // Top of the cylinder
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,1);
    for (angle=0; angle<=360; angle+=stepsize)
    {
		glVertex3f(Cos(angle),Sin(angle),1);
    }
    glEnd();

    // Bottom of the cylinder
    glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,0,-1);
    for (angle=0; angle<=360; angle+=stepsize)
    {
		glVertex3f(Cos(angle),Sin(angle),0);
    }
    glEnd();
	
	//	Restore Transformations
    glPopMatrix();
	glPopMatrix();
}

void cone(double xx, double yy, double zz,	double dx, double dy, double dz,	double th, double ph,	GLfloat r, GLfloat h, int tex)
{

	//	Save Transformations
    glPushMatrix();
	
	//	Offset, Rotate and Scale
    glTranslated(xx,yy,zz);
    glRotated(th,0,1,0);
    glRotated(ph,1,0,0);
    glScaled(dx,dy,dz);

	glPushMatrix();
	
	//	Offset, Rotate and Scale
    glScaled(r,r,h);
	
    //GLfloat x      = 0.0;
    //GLfloat y      = 0.0;
    GLint angle    = 0;
    GLint stepsize = 2;

	 if (tex >= 0)
   {
	   glEnable(GL_TEXTURE_2D);
	   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	   glBindTexture(GL_TEXTURE_2D, texture[tex]);
   }
   else
	   glDisable(GL_TEXTURE_2D);
   
    // Body of cone
    glBegin(GL_TRIANGLES);
	//glVertex3f(0,0, height);
    for (angle=0; angle<=360; angle+=(2*stepsize))
    {	
		glNormal3f(Cos(angle),Sin(angle),0);
		glTexCoord2f(0.0,0.0);
		glVertex3f(Cos(angle),Sin(angle),0);
		
		glNormal3f(Cos(angle+stepsize),Sin(angle+stepsize),1);
		glTexCoord2f(0.5,15);
		glVertex3f(0,0,1);
		
		glNormal3f(Cos(angle+(2*stepsize)),Sin(angle+(2*stepsize)),0);
		glTexCoord2f(1.0,0.0);
		glVertex3f(Cos(angle+(2*stepsize)),Sin(angle+(2*stepsize)),0);
    }
    glEnd();
	
	glDisable(GL_TEXTURE_2D);

	if (tex >= 0)
   {
	   glEnable(GL_TEXTURE_2D);
	   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	   glBindTexture(GL_TEXTURE_2D, texture[tex]);
   }
   else
	   glDisable(GL_TEXTURE_2D);
   
    // Bottom of the cone
	glColor3f(0.53,0.16,0.02);
    glBegin(GL_TRIANGLES);
	glNormal3f(0,0,-1);
    for (angle=0; angle<=360; angle+=stepsize)
    {
		glTexCoord2f(0.5,1.0);
		glVertex3f(0,0,0);
		
		glTexCoord2f(0.0,0.0);
		glVertex3f(Cos(angle),Sin(angle),0);
		
		glTexCoord2f(1.0,0.0);
		glVertex3f(Cos(angle+stepsize),Sin(angle+stepsize),0);
    }
    glEnd();
	
	glDisable(GL_TEXTURE_2D);
	
	//	Restore Transformations
    glPopMatrix();
	glPopMatrix();
}

void tree(double xx, double yy, double zz, double dx, double dy, double dz, double th, double ph)
{
		
		float green[] = {0,0.5,0,1};
		float black[] = {0,0,0,1};
		float brown[] = {0.53,0.16,0.02,1};
		
		// Save Transformations
		glPushMatrix();
		
		//	Offset, Rotate and Scale
		glTranslated(xx,yy,zz);
		glRotated(th,0,1,0);
		glRotated(ph,1,0,0);
		glScaled(dx,dy,dz);
		
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,brown);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
		
		//	Tree Trunk
		glColor3f(0.53,0.16,0.02);
		cylinder(0,0,0,1,1,1,0,90,3,25,5);
		
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,green);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
		
		//	Tree leaves
		glColor3f(0,0.5,0);
		cone(0,0,0,3,3,5,0,-90,3,10,6);
		
		//	Reset Transformations
		glPopMatrix();
	}

static void dalek(double xx,double yy,double zz, double dx,double dy,double dz, double th, double opacity)
{
   double x,y,z,X,Z;
   double x_diff,z_diff;
   int i;
   
   float white[] = {1,1,1,opacity};
   float black[] = {0,0,0,opacity};
   float blue[] = {0,0,0.99,opacity};
   //float yellow[] = {1.0,1.0,0.0,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(xx,yy,zz);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   //  Dalek
   //  Base
   glColor4f(0.2,0.2,0.2, opacity);
   //	Bottom Surface
   glBegin(GL_POLYGON);
   
   glNormal3f(0,-1,0);
   glVertex3f(-1,-2,-1);
   glVertex3f(18,-2,-1);
   glVertex3f(22.5,-2,1.5);
   glVertex3f(26,-2,7);
   glVertex3f(22.5,-2,12.5);
   glVertex3f(18,-2,15);
   glVertex3f(-1,-2,15);
   glVertex3f(-4,-2,12);
   glVertex3f(-4,-2,2);
   //  End
   glEnd();
   
   ErrCheck("dalek1");

   //	Top Surface
   glBegin(GL_POLYGON);

   glNormal3f(0,1,0);
   glVertex3f(-1,0,-1);
   glVertex3f(18,0,-1);
   glVertex3f(22.5,0,1.5);
   glVertex3f(26,0,7);
   glVertex3f(22.5,0,12.5);
   glVertex3f(18,0,15);
   glVertex3f(-1,0,15);
   glVertex3f(-4,0,12);
   glVertex3f(-4,0,2);
   //  End
   glEnd();
   
   //	Sides
   glBegin(GL_QUADS);

   glNormal3f(0,0,-1);
   glVertex3f(-1,-2,-1);
   glVertex3f(-1,0,-1);
   glVertex3f(18,0,-1);
   glVertex3f(18,-2,-1);
   
   glNormal3f(5,0,-9);
   glVertex3f(18,-2,-1);
   glVertex3f(18,0,-1);
   glVertex3f(22.5,0,1.5);
   glVertex3f(22.5,-2,1.5);
   
   glNormal3f(11,0,-7);
   glVertex3f(22.5,-2,1.5);
   glVertex3f(22.5,0,1.5);
   glVertex3f(26,0,7);
   glVertex3f(26,-2,7);
   
   glNormal3f(11,0,7);
   glVertex3f(26,-2,7);
   glVertex3f(26,0,7);
   glVertex3f(22.5,0,12.5);
   glVertex3f(22.5,-2,12.5);
   
   glNormal3f(5,0,9);
   glVertex3f(22.5,-2,12.5);
   glVertex3f(22.5,0,12.5);
   glVertex3f(18,0,15);
   glVertex3f(18,-2,15);
   
   glNormal3f(0,0,1);
   glVertex3f(18,-2,15);
   glVertex3f(18,0,15);
   glVertex3f(-1,0,15);
   glVertex3f(-1,-2,15);
   
   glNormal3f(-1,0,1);
   glVertex3f(-1,-2,15);
   glVertex3f(-1,0,15);
   glVertex3f(-4,0,12);
   glVertex3f(-4,-2,12);
   
   glNormal3f(-1,0,0);
   glVertex3f(-4,-2,12);
   glVertex3f(-4,0,12);
   glVertex3f(-4,0,2);
   glVertex3f(-4,-2,2);
   
   glNormal3f(-1,0,-1);
   glVertex3f(-4,-2,2);
   glVertex3f(-4,0,2);
   glVertex3f(-1,0,-1);
   glVertex3f(-1,-2,-1);
   	
   
   //  End
   glEnd();
   
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,7));
   
   // Body
   glBegin(GL_POLYGON);
   //  Bottom
   glColor4f(0.61,0.4,0.12, opacity);
   glNormal3f(0,-1,0);
   for (i=0;i<9;i++)
   	glVertex3fv(body_base[i]);
   //  End
   glEnd();

   glBegin(GL_POLYGON);
   //  Top
   glColor4f(0.81,0.6,0.32, opacity);
   glNormal3f(0,1,0);
   for (i=0;i<9;i++)
   	glVertex3fv(body_top[i]);
   //  End
   glEnd();

   glBegin(GL_QUADS);
   //  Fill in
   glColor4f(0.61,0.4,0.12,opacity);
   for (i=0;i<9;i++)
   {
		glNormal3fv(normals[i]); 
		glVertex3fv(body_base[i]);
		glVertex3fv(body_top[i]);
		glVertex3fv(body_top[(i+1)%9]);
		glVertex3fv(body_base[(i+1)%9]);
   }
   //  End
   glEnd();

   glBegin(GL_POLYGON);
   //  Mid Section
   glColor4f(0.81,0.6,0.32, opacity);
   glNormal3f(0,1,0);
   glVertex3f(1,40,0);
   glVertex3f(7,40,0);
   glVertex3f(11,40,4);
   glVertex3f(12,40,7);
   glVertex3f(11,40,10);
   glVertex3f(7,40,14);
   glVertex3f(1,40,14);
   glVertex3f(-3,40,10);
   glVertex3f(-3,40,4);
   //  End
   glEnd();

   glBegin(GL_QUADS);
   //  Body of MidSection
   glColor4f(0.81,0.6,0.32, opacity);
   
   glNormal3f(0,-1,-10);
   glVertex3fv(body_top[0]);	//	0,30,0
   glVertex3f(1,40,0);
   glVertex3f(7,40,0);
   glVertex3fv(body_top[1]);	//	14,30,1
   
   glNormal3f(35,19.5,-50);
   glVertex3fv(body_top[1]);
   glVertex3f(7,40,0);
   glVertex3f(12,40,3.5);
   glVertex3fv(body_top[2]);	//	18,30,3
   
   glNormal3f(35,21.5,-10);
   glVertex3fv(body_top[2]);
   glVertex3f(12,40,3.5);
   glVertex3f(13,40,7);
   glVertex3fv(body_top[3]);	//	20,30,7
   
   glNormal3f(35,24.5,10);
   glVertex3fv(body_top[3]);
   glVertex3f(13,40,7);
   glVertex3f(12,40,10.5);
   glVertex3fv(body_top[4]);	//	18,30,11
   
   glNormal3f(35,23.5,50);
   glVertex3fv(body_top[4]);
   glVertex3f(12,40,10.5);
   glVertex3f(7,40,14);
   glVertex3fv(body_top[5]);	//	14,30,13
   
   glNormal3f(0,-1,10);
   glVertex3fv(body_top[5]);
   glVertex3f(7,40,14);
   glVertex3f(1,40,14);
   glVertex3fv(body_top[6]);	//	0,30,14
   
   glNormal3f(-10,1,10);
   glVertex3fv(body_top[6]);
   glVertex3f(1,40,14);
   glVertex3f(-3,40,10);
   glVertex3fv(body_top[7]);	//	-3,30,11
   
   glNormal3f(-1,0,0);
   glVertex3fv(body_top[7]);
   glVertex3f(-3,40,10);
   glVertex3f(-3,40,4);
   glVertex3fv(body_top[8]);	//	-3,30,3
   
   glNormal3f(-10,1,-10);
   glVertex3fv(body_top[8]);
   glVertex3f(-3,40,4);
   glVertex3f(1,40,0);
   glVertex3fv(body_top[0]);
   
   //  End
   glEnd();

   // Neck
   glColor4f(0,0,0, opacity);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
   cylinder(4,44.5,7,1,1,1,0,90,5,4.5,-1);
   
   glColor4f(0.61,0.4,0.12, opacity);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,7));
   cylinder(4,40.5,7,1,1,1,0,90,7,0.5,-1);
   cylinder(4,42,7,1,1,1,0,90,7,0.5,-1);
   cylinder(4,43.5,7,1,1,1,0,90,7,0.5,-1); 
   
   // The Orbs
   glColor4f(0.81,0.6,0.32, opacity);
   for (i=0;i<9;i++)
   {
      Z = abs(body_base[(i+1)%9][2] - body_base[i][2]);
      X = abs(body_base[(i+1)%9][0] - body_base[i][0]);
      if (sqrt((X*X) + (Z*Z)) <= 11)
      { 
		z_diff = (body_top[i][2] + body_top[(i+1)%9][2])/2.0;
      	x_diff = (body_top[i][0] + body_top[(i+1)%9][0])/2.0;
		z = (body_base[i][2] + body_base[(i+1)%9][2])/2.0;
      	x = (body_base[i][0] + body_base[(i+1)%9][0])/2.0;
		x_diff = (x-x_diff)/3;
		z_diff = (z-z_diff)/3;
		y = 5;
		z -= z_diff;
		x -= x_diff;
		sphere(x,y,z,2);
		y = 15;
		z -= z_diff;
		x -= x_diff;
		sphere(x,y,z,2);
		y = 25;
		z -= z_diff;
		x -= x_diff;
		sphere(x,y,z,2);
      }
      else if (body_base[(i+1)%9][0] > body_base[i][0])
      {
		z_diff = (body_top[i][2] + body_top[(i+1)%9][2])/2.0;
		x_diff = (body_top[i][0] + body_top[(i+1)%9][0])/2.0;
		x = (body_base[i][0] + body_base[(i+1)%9][0])/2.0;
		z = (body_base[i][2] + body_base[(i+1)%9][2])/2.0;
		x_diff = (x-x_diff)/3;
		z_diff = (z-z_diff)/3; 

		X = body_base[i][0] + (2 + (5/4));
        do
        {
			x = X;
			z = (body_base[i][2] + body_base[(i+1)%9][2])/2.0;
			y = 5;
			z -= z_diff;
			x -= x_diff;
			sphere(x,y,z,2);
			y = 15;
			z -= z_diff;
			x -= x_diff;
			sphere(x,y,z,2);
			y = 25;
			z -= z_diff;
			x -= x_diff;
			sphere(x,y,z,2);
      	   	X = X + (4 + (5/4));
        }while((X+2+(5/4))<(body_base[(i+1)%9][0]));
      }
      else if (body_base[(i+1)%9][0] < body_base[i][0])
      {
        z_diff = (body_top[i][2] + body_top[(i+1)%9][2])/2.0;
      	x_diff = (body_top[i][0] + body_top[(i+1)%9][0])/2.0;
      	x = (body_base[i][0] + body_base[(i+1)%9][0])/2.0;
		z = (body_base[i][2] + body_base[(i+1)%9][2])/2.0;
		x_diff = (x-x_diff)/3;
		z_diff = (z-z_diff)/3; 

		X = body_base[i][0] - (2 + (5/4));
        do
        {
			x = X;
			z = (body_base[i][2] + body_base[(i+1)%9][2])/2.0;
			y = 5;
			z -= z_diff;
			x -= x_diff;
			sphere(x,y,z,2);
			y = 15;
			z -= z_diff;
			x -= x_diff;
			sphere(x,y,z,2);
			y = 25;
			z -= z_diff;
			x -= x_diff;
			sphere(x,y,z,2);
      	   	X = X - (4 + (5/4));
        }while((X-(2+(5/4)))>(body_base[(i+1)%9][0]));
      }
   }

   // Head
   glColor4f(0.35,0.35,0.35, opacity);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
   hemisphere(4.25,44.5,7,7,-90,0);

   // Eye
   glColor4f(0.8,0.8,0.8, opacity);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,7));
   cylinder(9,47,7,0.45,0.45,2,90,0,3,4.5,-1);
   
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
   glColor4f(0.2,0.2,0.2, opacity);
   hemisphere(20,47.5,7,3.5,0,-90);
   
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,7));
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,blue);
   glColor4f(0.35,0.45,0.91, opacity);
   hemisphere(18,47.5,7,2,0,90);

   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   // Head Lights
   glColor4f(0.6,0.91,0.93, (0.5 * opacity));
   //glColor4f(1,1,1,0.0);
   cylinder(5,50,11,0.5,0.5,1,0,-45,3,2.5,-1);
   cylinder(5,50,3,0.5,0.5,1,0,-135,3,2.5,-1);

   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   // Plunger
   glColor4f(0.8,0.8,0.8, opacity);
   cylinder(14,35,11.5,0.45,0.45,2,90,-15,1.8,5.95,-1);
   
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,0));
   glColor4f(0,0,0, opacity);
   hemisphere(27.9,38.75,11.5,2.5,15,-90);

   // Blaster
   glColor4f(0.7,0.7,0.7, opacity);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,3));
   cylinder(14,35,3,0.45,0.45,2,90,0,1,4.5,-1);
   glColor4f(0.7,0.7,0.7,(0.4 * opacity));
   cylinder(14,35,3,0.45,0.45,2,90,0,2,4.5,-1);
   //  Undo transofrmations
   glPopMatrix();
}

static void house(double x,double y,double z, double dx,double dy,double dz, double th,  double ph)
{
	   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   float blue[] = {0.6,0.91,0.93,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,2));
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);
   //  Cube
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   cube(0,0,0,1,1,1,0,0,0);
   // Roof
   	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);   
 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	
   glBegin(GL_TRIANGLES);
	
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1.25,+1,-1.25);
   glTexCoord2f(1.0,0.0);	glVertex3f(-1.25,+1,+1.25);
   glTexCoord2f(0.5,1.0);	glVertex3f(-1.25,+1.75,0);
   
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(1.25,+1,-1.25);
   glTexCoord2f(1.0,0.0);	glVertex3f(1.25,+1,+1.25);
   glTexCoord2f(0.5,1.0);	glVertex3f(1.25,+1.75,0);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
	
   glColor3f(0.53,0.16,0.02);
   
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	
	glBegin(GL_QUADS);
   
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1.25,+1,+1.25);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1.25,+1,+1.25);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1.25,+1,-1.25);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1.25,+1,-1.25);
   
   glNormal3f( 0, 1, 1);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1.25,+1,+1.25);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1.25,+1,+1.25);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1.25,+1.75,0);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1.25,+1.75,0);
  
	glNormal3f(0, 1, -1);
   glTexCoord2f(0.0,0.0);	glVertex3f(-1.25,+1.75,0);
   glTexCoord2f(1.0,0.0);	glVertex3f(+1.25,+1.75,0);
   glTexCoord2f(1.0,1.0);	glVertex3f(+1.25,+1,-1.25);
   glTexCoord2f(0.0,1.0);	glVertex3f(-1.25,+1,-1.25);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glDisable(GL_POLYGON_OFFSET_FILL);
   
   glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	
   // Door
   glBegin(GL_QUADS);
   glNormal3f( -1, 0, 0);
   glColor3f(0.53,0.16,0.02);
   
   glTexCoord2f(0.2,0.2);	glVertex3f(-1,-1,-0.25);
   glTexCoord2f(0.2,0.95);	glVertex3f(-1,0,-0.25);
   glTexCoord2f(1.0,0.95);	glVertex3f(-1,0,0.25);
   glTexCoord2f(1.0,0.2);	glVertex3f(-1,-1,0.25);
   
   glEnd();
   
   glDisable(GL_TEXTURE_2D);
   
   glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,blue);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,pow(2.0,2));
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	
   // Windows
   glBegin(GL_QUADS);
   glNormal3f( -1, 0, 0);
   
   glColor3f(0.6,0.91,0.93);
   
   glTexCoord2f(0.07,0.0);	glVertex3f(-1,0.3,-0.7);
   glTexCoord2f(0.07,1.0);	glVertex3f(-1,0.6,-0.7);
   glTexCoord2f(1.12,1.0);	glVertex3f(-1,0.6,-0.4);
   glTexCoord2f(1.12,0.0);	glVertex3f(-1,0.3,-0.4);
   
   glTexCoord2f(0.07,0.0);	glVertex3f(-1,0.3,0.4);
   glTexCoord2f(0.07,1.0);	glVertex3f(-1,0.6,0.4);
   glTexCoord2f(1.12,1.0);	glVertex3f(-1,0.6,0.7);
   glTexCoord2f(1.12,0.0);	glVertex3f(-1,0.3,0.7);
   
   glEnd();
   glDisable(GL_TEXTURE_2D);
   
   //  Undo transofrmations
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
	int i,j, num_daleks;
	double ref_x =  -2*dim*Sin(cam_th)*Cos(cam_ph);
	double ref_z =  +2*dim*Cos(cam_th)*Cos(cam_ph);
	
	
   glClearColor(0.09,0.75,0.86,1.0);
	
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   
   //  First Person Navigation
   if (mode)
   {
	  //dim = 50;
      gluLookAt(Ex,Ey,Ez , Ex+ref_x,Ey,Ez+ref_z , 0,Cos(cam_ph),0);
   }
   else
   {
	  double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }

   if (shot)
	{
		glColor3f(1.0,0.0,0.0);
	    sphere(Ex+(ref_x/2.8),-5,Ez+(ref_z/2.8),0.75);
		glColor3f(1.0,1.0,0.0);
	    sphere(Ex+(ref_x/2.875),-4.55,Ez+(ref_z/2.875),0.5);
		shot = 0;
		
		Mix_PlayChannel(-1,gunshot,0);
	}
	
   //  Flat or smooth shading
   glShadeModel(GL_SMOOTH);

   //  Light
	//  Translate intensity to color vectors
	float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
	float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
	float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
	//  Light position
	float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
	//  Draw light position as ball (still no lighting here)
	glColor3f(1,1,1);
	sphere(Position[0],Position[1],Position[2] , 1);
	//  OpenGL should normalize normal vectors
	glEnable(GL_NORMALIZE);
	//  Enable lighting
	glEnable(GL_LIGHTING);
	//  Location of viewer for specular calculations
	//  glColor sets ambient and diffuse color materials
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	//  Enable light 0
	glEnable(GL_LIGHT0);
	//  Set ambient, diffuse, specular components and position of light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
	glLightfv(GL_LIGHT0,GL_POSITION,Position);

   //  Draw scene
   glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	
	if (mode)
	{
		//Draw the pistol
		glPushMatrix();
		glTranslated(Ex+(ref_x/3),-7,Ez+(ref_z/3));
		glRotated(180-cam_th,0,1,0);
		glRotated(20,1,0,0);
		glScaled(0.5,1,2);
	   glColor3f(0.3, 0.3, 0.3);
	   glBegin(GL_QUADS);
	   glNormal3f( 0,+1, 0);
	   glTexCoord2f(0.3,0.0);	glVertex3f(-1,+1,+1);
	   glTexCoord2f(1.3,0.0);	glVertex3f(+1,+1,+1);
	   glTexCoord2f(1.3,1.0);	glVertex3f(+1,+1,-1);
	   glTexCoord2f(0.3,1.0);	glVertex3f(-1,+1,-1);
	   glEnd();
	   glPopMatrix();
	}
   
   glDisable(GL_TEXTURE_2D);
   
   // Grassy Area
   glColor3f(0.5,1,0.5);
   for(i=-49;i<=49;i+=2)
	   for(j=-49;j<=49;j+=2)
			cube(i,-10,j,1,1,1,0,0,4);
		
	for(i=-199;i<=-101;i+=2)
	   for(j=-199;j<=199;j+=2)
			cube(i,-10,j,1,1,1,0,0,4);
	
	for(i=101;i<=199;i+=2)
	   for(j=-199;j<=199;j+=2)
			cube(i,-10,j,1,1,1,0,0,4);
		
	for(i=-99;i<=99;i+=2)
	   for(j=-199;j<=-101;j+=2)	
			cube(i,-10,j,1,1,1,0,0,4);
		
	for(i=-99;i<=99;i+=2)
	   for(j=99;j<=199;j+=2)	
			cube(i,-10,j,1,1,1,0,0,4);
   
   //Streets
   glColor3f(0.2,0.2,0.2);
   
   for(i=-99;i<=-51;i+=2)
	   for(j=-99;j<=99;j+=2)
			cube(i,-10,j,1,1,1,0,0,7);
		
	for(i=51;i<=99;i+=2)
	   for(j=-99;j<=99;j+=2)
			cube(i,-10,j,1,1,1,0,0,7);

   for(i=-99;i<=99;i+=2)
	   for(j=-99;j<=-51;j+=2)
			cube(i,-10,j,1,1,1,0,0,7);
		
	for(i=-99;i<=99;i+=2)
	   for(j=51;j<=99;j+=2)
			cube(i,-10,j,1,1,1,0,0,7);
   
   //  Draw Houses
   glColor3f(0.5,0.25,0);
   house(-20,0,35 ,10,10,10 , 90, 0);		//	1
   glColor3f(1,0,0.5);
   house(20,0,35 ,10,10,10 , 90, 0);			//	2
   glColor3f(1,0.5,0.25);
   house(35,0, 0 ,10,10,10 , 180, 0);			//	3
   glColor3f(0.5,0.5,0.5);
   house(-15,0,-35 ,10,10,10 , 270, 0);		//	4
   glColor3f(0.26,0.5,0.6);
   house(-80,0,115 ,10,10,10 , 270, 0);	//	5
   glColor3f(0.6,0.26,0.5);
   house(-20,0,115 ,10,10,10 , 270, 0);	//	6
   glColor3f(0.5,0.6,0.26);
   house(50,0,115 ,10,10,10 , 270, 0);		//	7
   glColor3f(0.5,0,0.9);
   house(115,0,70 ,10,10,10 , 0, 0);			//	8
   glColor3f(1,0.5,0);
   house(115,0,20 ,10,10,10 , 0, 0);			// 	9
   glColor3f(1,0.75,0.5);
   house(115,0,-50 ,10,10,10 , 0, 0);		//	10
   glColor3f(1,0.25,0.75);
   house(50,0, -115 ,10,10,10 , 90, 0);		//	11
   glColor3f(0.5,0.5,0.5);
   house(-10,0,-115 ,10,10,10 , 90, 0);		//	12
   glColor3f(0.68,0.5,0.4);
   house(-60,0,-115 ,10,10,10 , 90, 0);		//	13
   glColor3f(0.5,0.4,0.68);
   house(-115,0,-50 ,10,10,10 , 180, 0);	//	14
   glColor3f(0.4,0.68,0.5);
   house(-115,0,0 ,10,10,10 , 180, 0);		//	15
   glColor3f(0.8,0.4,0.4);
   house(-115,0,60 ,10,10,10 , 180, 0);	//	16
   
   //   Draw Trees
   tree(-18,2.5,8,0.5,0.5,0.5,0,0);
   tree(-8,7,10,0.5,0.7,0.5,0,0);
   tree(-10,4.5,0,0.5,0.6,0.5,0,0);
   tree(-45,8,-5,0.5,0.75,0.5,0,0); 
   tree(-45,2.5,40,0.5,0.5,0.5,0,0);
   tree(25,2.5,-45,0.5,0.5,0.5,0,0);
   tree(45,4.5,30,0.5,0.6,0.5,0,0);
   tree(30,2.5,110,0.5,0.5,0.5,0,0);
   tree(5,2.5,120,0.5,0.5,0.5,0,0);
   tree(-50,2.5,170,0.5,0.5,0.5,0,0);
   tree(-130,4.5,172,0.5,0.6,0.5,0,0);
   tree(-120,2.5,130,0.5,0.5,0.5,0,0);
   tree(-150,2.5,100,0.5,0.5,0.5,0,0);
   tree(-110,7,30 ,0.5,0.7,0.5,0,0);
   tree(-125,2.5,-100,0.5,0.5,0.5,0,0);
   tree(-102,2.5,-150,0.5,0.5,0.5,0,0);
   tree(-158,4.5,-152,0.5,0.6,0.5,0,0);
   tree(-40,2.5,-170,0.5,0.5,0.5,0,0);
   tree(25,2.5,-115,0.5,0.5,0.5,0,0);
   tree(80,4.5,-105,0.5,0.6,0.5,0,0);
   tree(83,7,-117,0.5,0.7,0.5,0,0);
   tree(100,7,-130,0.5,0.7,0.5,0,0);
   tree(120,2.5,-165,0.5,0.5,0.5,0,0);
   tree(165,2.5,-102,0.5,0.5,0.5,0,0);
   tree(105,4.5,-10,0.5,0.6,0.5,0,0);
   tree(150,2.5,-20,0.5,0.5,0.5,0,0);
   tree(145,2.5,68,0.5,0.5,0.5,0,0);
   tree(90,7,110,0.5,0.7,0.5,0,0);
   tree(85,2.5,160,0.5,0.5,0.5,0,0);
   tree(160,2.5,140,0.5,0.5,0.5,0,0);
   tree(-168,4.5,15,0.5,0.6,0.5,0,0);
   tree(-140,2.5,-20,0.5,0.5,0.5,0,0);
  
  //  Draw daleks
   if (disp_dalek[0])
		dalek(-85,-7,-40 , 0.3,0.3,0.3 , -90, opacity_dalek[0]);
	
	if (disp_dalek[1])
		dalek(-65,-7,60, 0.3,0.3,0.3 , 90, opacity_dalek[1]);
	
	if (disp_dalek[2])
		dalek(35,-7,-75,0.3,0.3,0.3 , 0, opacity_dalek[2]);
	
	if (disp_dalek[3])
		dalek(85,-7,0, 0.3,0.3,0.3 , 90, opacity_dalek[3]);
	
	if (disp_dalek[4])
		dalek(30,-7,90, 0.3,0.3,0.3 , 180, opacity_dalek[4]);
	
   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   
   if (mode)
   {
	   glColor3f(0.99,0.99,0.99);
	   glWindowPos2i(5,5);
	   num_daleks = 0;
	   for (i=0; i<5; i++)
		   num_daleks += disp_dalek[i];
	   Print("Mode : Game; Daleks Left : %d", num_daleks);
   }
   else
   {
	   //  Display parameters
		glWindowPos2i(5,5);
		Print("Angle : %d,%d;  Dim : %.1f; FOV : %d; Mode : View Scene",th,ph,dim,fov);
   }
	
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void restart(int button, int state, int x, int y)
{
		int i;
		
		if ((button ==  GLUT_LEFT_BUTTON) && (state = GLUT_UP))
		{
			if ((x > ((Width/2)-35)) && (x < ((Width/2)+35))  && (y > ((Height/2) + 35)) && (y < ((Height/2) + 50)))
			{
				glClearColor(0.09,0.75,0.86,1.0);
   
				//  Enable transparency
			   glEnable (GL_BLEND);
			   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			   
			   Project(fov,asp,dim);
			   
			   //  Set callbacks
			   glutDisplayFunc(display);
			   glutIdleFunc(idle);
			   
			   glutMouseFunc(NULL);
			   
			   for (i=0; i<5 ; i++)
			   {
				   disp_dalek[i] = 1;
				   opacity_dalek[i] = 1.0;
			   }
			   
			   shot = 0;
			   
			   cam_th = cam_ph = 0;
			   Ex = 0;
			   Ey = 10;
			   Ez = 0;			   
			}
		}
}
	
void deadDisplay()
{
	glClearColor(0.0,0.0,0.0,1.0);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   
   glColor3f(1.0,0.0,0.0);
   glWindowPos2i((Width/2)-50,(Height/2));
   Print("YOU DEAD");
   
   glColor3f(0.7,0.7,0.7);
   glWindowPos2i((Width/2)-35,(Height/2) - 50);
   Print("Restart?");
   
   glutMouseFunc(restart);
   
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void winDisplay()
{
	glClearColor(0.0,0.0,0.0,1.0);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   
   glColor3f(0.0,1.0,0.0);
   glWindowPos2i((Width/2)-50,(Height/2));
   Print("YOU WIN!!");
   
   glColor3f(0.7,0.7,0.7);
   glWindowPos2i((Width/2)-35,(Height/2) - 50);
   Print("Restart?");
   
   glutMouseFunc(restart);
   
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void updateDisplay(int i)
{
	double ref_x =  -2*dim*Sin(cam_th)*Cos(cam_ph);
	double ref_z =  +2*dim*Cos(cam_th)*Cos(cam_ph);
	//printf("Hello");
	
	 if (i < 0)
		glutDisplayFunc(display);
	else if  ((disp_dalek[i] == 1) && (Clip(Ex+ (ref_x/3), Ez+ (ref_z/3), Ex+ (ref_x/3), Ez+ (ref_z/3), dalek_range_x[i][0], dalek_range_x[i][1], dalek_range_z[i][0], dalek_range_z[i][1])))
		glutDisplayFunc(deadDisplay);
}

void checkShot()
{
		int i;
		double ref_x =  -2*dim*Sin(cam_th)*Cos(cam_ph);
		double ref_z =  +2*dim*Cos(cam_th)*Cos(cam_ph);
		
		//shot = 1;
		
		for (i=0; i<5; i++)
			if  ((disp_dalek[i] == 1) && (Clip(Ex+ (ref_x/3), Ez+ (ref_z/3), Ex+ (ref_x/3), Ez+ (ref_z/3), shot_range_x[i][0], shot_range_x[i][1], shot_range_z[i][0], shot_range_z[i][1])))
			{
				opacity_dalek[i] -= 0.25;
			}
}

void idle()
{
	int i, win=1;
		
	//if shot()
	double ref_x =  -2*dim*Sin(cam_th)*Cos(cam_ph);
	double ref_z =  +2*dim*Cos(cam_th)*Cos(cam_ph);
	
	for (i=0; i<5; i++)
	{
		if (disp_dalek[i] == 1)
			win = 0;
		
		if (opacity_dalek[i] <= 0.0)
			disp_dalek[i] = 0;
		
		if ((disp_dalek[i] == 1) && (opacity_dalek[i] < 1.0))
			opacity_dalek[i] -= 0.25;
		
		if ((disp_dalek[i] == 1) && (Clip(Ex+ (ref_x/3), Ez+ (ref_z/3), Ex+ (ref_x/3), Ez+ (ref_z/3), dalek_range_x[i][0], dalek_range_x[i][1], dalek_range_z[i][0], dalek_range_z[i][1])))
		{
			break;
		}
	}
	
	if (win)
		glutDisplayFunc(winDisplay);
	else
		glutTimerFunc(2000, updateDisplay, i);
	
	glutPostRedisplay();			
}

 bool collision(int key)
{
	int i;
	double temp_Ex;
	double temp_Ez;
	double ref_x =  -2*dim*Sin(cam_th)*Cos(cam_ph);
	double ref_z =  +2*dim*Cos(cam_th)*Cos(cam_ph);
	
	if (key == 0)
	{
		temp_Ex = Ex - (Sin(cam_th-90)*Cos(cam_ph));
		temp_Ez = Ez + (Cos(cam_th-90)*Cos(cam_ph));
	}
	else if (key == 1)
	{
		temp_Ex = Ex - (Sin(cam_th+90)*Cos(cam_ph));
		temp_Ez = Ez + (Cos(cam_th+90)*Cos(cam_ph));
	}
	else if (key == GLUT_KEY_UP)
	{
		temp_Ex = Ex - (Sin(cam_th)*Cos(cam_ph));
		temp_Ez = Ez + (Cos(cam_th)*Cos(cam_ph));
	}
	else if (key == GLUT_KEY_DOWN)
	{
		temp_Ex = Ex + (Sin(cam_th)*Cos(cam_ph));
		temp_Ez = Ez - (Cos(cam_th)*Cos(cam_ph));
	}
	else if (key == GLUT_KEY_LEFT)
	{
		ref_x =  -2*dim*Sin(cam_th-5)*Cos(cam_ph);
		ref_z =  +2*dim*Cos(cam_th-5)*Cos(cam_ph);
		
		temp_Ex = Ex;
		temp_Ez = Ez;		
	}
	else
	{
		ref_x =  -2*dim*Sin(cam_th+5)*Cos(cam_ph);
		ref_z =  +2*dim*Cos(cam_th+5)*Cos(cam_ph);
		
		temp_Ex = Ex;
		temp_Ez = Ez;
	}
	
	for (i=0; i<53;  i++)
		//if (Clip(temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3), temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3)+4, obj_pos_x[i][0], obj_pos_x[i][1], obj_pos_z[i][0], obj_pos_z[i][1]))
		if (i < 48)
		{
			if (Clip(temp_Ex, temp_Ez, temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3), obj_pos_x[i][0], obj_pos_x[i][1], obj_pos_z[i][0], obj_pos_z[i][1]) && Clip(temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3), temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3)+4, obj_pos_x[i][0], obj_pos_x[i][1], obj_pos_z[i][0], obj_pos_z[i][1]))
				return true;
		}
		else
		{
				if ((disp_dalek[i - 48] == 1) && Clip(temp_Ex, temp_Ez, temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3), obj_pos_x[i][0], obj_pos_x[i][1], obj_pos_z[i][0], obj_pos_z[i][1]) && Clip(temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3), temp_Ex+ (ref_x/3), temp_Ez+ (ref_z/3)+4, obj_pos_x[i][0], obj_pos_x[i][1], obj_pos_z[i][0], obj_pos_z[i][1]))
					return true;
		}
		
	return false;
} 

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
   {
	   if (mode)
	   {
		   if (!collision(key))
				cam_th += 5;
	   }
		else
			th += 5;
   }
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
    {
		if (mode)
		{
		   if (!collision(key))
			cam_th -= 5;
		}
		else
			th -= 5;
   }
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
   {
	   if (mode)
	   {
		   if (!collision(key))
		   {
			   Ex += -1 * Sin(cam_th)*Cos(cam_ph);
			   Ez += Cos(cam_th)*Cos(cam_ph);
		   }
		}
		else
			ph += 5;
   }
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
   {
	    if (mode)
	   {
		   if (!collision(key))
		   {
			   Ex -= -1 * Sin(cam_th)*Cos(cam_ph);
			   Ez -= Cos(cam_th)*Cos(cam_ph);
		   }
		}
		else
			ph -= 5;
   }
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN && (mode == 0))
      dim += 0.5;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1 && (mode == 0))
      dim -= 0.5;
   //  Keep angles to +/-360 degrees
   cam_th %= 360;
   cam_ph %= 360;
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}


/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
  
   //  Reset view angle
   else if (ch == '0')
   {
	   if (mode)
	   {
		   dim = 50;
		   cam_th = cam_ph = 0;
		   Ex = 0;
		   Ey = 10;
		   Ez = 0;
	   }
	   else
	   {
		   dim = 200;
		   th = 90;
		   ph = 15;
	   }
   }
   
   else if ((ch == 'm') || (ch == 'M'))
   {
	   mode = 1-mode;
	   if (mode)
	   {
		   dim = 50;
		   glutIdleFunc(idle);
	   }
	   else
	   {
		   dim = 200;
		   glutIdleFunc(NULL);
	   }
   }
   
   /* Uncomment if adding option for background music
   else if ((ch == 's') || (ch == 'S'))
   {
	   play_music = 1 - play_music;
	   if (play_music)
		   Mix_ResumeMusic();
	   else
		   Mix_PauseMusic();
   }
   */
   
   else if (ch == '<')
   {
	   if (mode)
		   if (!collision(0)) 
		   {
			   Ex += -1 * Sin(cam_th-90)*Cos(cam_ph);
			   Ez += Cos(cam_th-90)*Cos(cam_ph);
		   }
   }
   
   else if (ch == '>')
   {
	   if (mode)
		   if (!collision(1))
		   {
			   Ex += -1 * Sin(cam_th+90)*Cos(cam_ph);
			   Ez += Cos(cam_th+90)*Cos(cam_ph);
		   }
   }
   
   else if ((ch == ' ') && (mode == 1))
   {
	   shot = 1;
	   checkShot();
   }
 /*   
   else if ((ch == 'a') || (ch == 'A'))
   {
	   animate();
   } */
  
   //  Change field of view angle
   else if (ch == '-' && ch>1 && (mode == 0))
      fov--;
   else if (ch == '+' && ch<179 && (mode == 0))
      fov++;
  
   //  Reproject
   Project(fov,asp,dim);
   
   //  Animate if requested
   //glutIdleFunc(move?idle:NULL);
   
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Width = width;
   Height = height;
   Project(fov,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
	//  Initialize GLUT
   glutInit(&argc,argv);
   
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(700,700);
   glutCreateWindow("Final - Krithika Balan");
   
    //	Set Background Sky color
   glClearColor(0.09,0.75,0.86,1.0);
   
    //  Enable transparency
   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   Project(fov,asp,dim);
   
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   
   glutMouseFunc(NULL);
   
   //	Load Textures
   texture[0] = LoadTexBMP("wall1.bmp");
   texture[1] = LoadTexBMP("roof.bmp");
   texture[2] = LoadTexBMP("door.bmp");
   texture[3] = LoadTexBMP("window.bmp");
   texture[4] = LoadTexBMP("grass.bmp");
   texture[5] = LoadTexBMP("trunk.bmp");
   texture[6] = LoadTexBMP("tree.bmp");
   texture[7] = LoadTexBMP("road.bmp");
   texture[8] = LoadTexBMP("gun.bmp");
   
   
   //  Initialize audio
   if (Mix_OpenAudio(44100,AUDIO_S16SYS,2,4096)) Fatal("Cannot initialize audio\n");
   //  Load Gunshot sound
   gunshot = Mix_LoadWAV("gunshot.wav");
   if (!gunshot) Fatal("Cannot load gunshot\n");
   
   /* Uncomment if adding background music
   
   // Load Background Music
   music = Mix_LoadMUS("I'm The Doctor.mp3");
   if (!music) Fatal("Cannot load music\n");
   Mix_PlayMusic(music,-1);
   
   //	Set Background Volume
   Mix_VolumeMusic(MIX_MAX_VOLUME/4);
   
   */
   
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   
      //  Shut down SDL
   Mix_CloseAudio();
   SDL_Quit();
   
   return 0;
}

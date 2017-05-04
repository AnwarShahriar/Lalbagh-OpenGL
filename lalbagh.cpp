#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Vector.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
using namespace std;
#define GROUND 1, 1, 1
#define PI 3.1416
#define BLACK .4, .5, 1
#define SKY_FRONT 0
#define SKY_RIGHT 1
#define SKY_LEFT 2
#define SKY_BACK 3
#define SKY_UP 4
#define SKY_DOWN 5
#define ANGLE 0.2
#define SCALE 0.9
#define RAND 0.01
GLuint base_tex,tower_f;

typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;

//make a global variable -- for tracking the anglular position of camera
double cameraAngle;			//in radian
double cameraAngleDelta;
double selfRot=0, uniRot=0,cyl_rotate=0,car_speed,cart_rotate;
double wheel_1_rotate=0,wheel_2_rotate=0,wheel_3_rotate=0,wheel_4_rotate=0;
double cameraHeight;	
double cameraRadius;

double uniRotDelta;	//in degree
double speed;
GLUquadricObj *quadric, *minercylinder;
double wheel_x,wheel_y;
int full_rot,move_key,car1_mov,car2_mov;
double car1_spd,car2_spd,shp_spd=-390;
double opn_angle,sky_rot,shp_rot_angle,e_x,e_y,e_z,wtr_y_trnslt,wtr_x_trnslt;
int   brdg_opn,cam_rot,shp_dir=-1;

float i_i;

float zoom = 350;
float rightAngle=0.0, leftAngle=0.0;

bool increasing,decreasing,show_strctr,shp_rot,lft_mov,frt_mov;

//////////////////////////////////////////////

GLuint texid1,texid2,texid3,texid4,texid5,texid6,texid7,texid8,texid9,texid10;
GLuint texid11,texid12,texid13,texid14,texid15,texid16,texid17,texid18,texid19,texid20;
GLuint texid21,texid22,texid23,texid24,texid25,texid26,texid27,texid28,texid29,texid30;
GLuint texid31,texid32,texid33,texid34,texid35,texid36,texid37,texid38,texid39,texid40;
int skybox[6];
int num_texture=-1;

typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagRGBTRIPLE {
  BYTE rgbtBlue;
  BYTE rgbtGreen;
  BYTE rgbtRed;
} RGBTRIPLE;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  unsigned int  biWidth;
  unsigned int  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  unsigned int  biXPelsPerMeter;
  unsigned int  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int LoadBitmap(char *filename, int width, int height)
{
    int i,j=0;
    FILE *l_file;
    unsigned char *l_texture;
    BITMAPFILEHEADER fileheader; // not used
    BITMAPINFOHEADER infoheader; // not used
    RGBTRIPLE rgb;

    num_texture++;
    if((l_file=fopen(filename,"rb"))==NULL)return(-1);
    //printf("3333333\n");
    // fread(&fileheader,sizeof(fileheader),1,l_file);
    
    // fseek(l_file,sizeof(fileheader),SEEK_SET);
    
    // fread(&infoheader,sizeof(infoheader),1,l_file);
    
    infoheader.biWidth = width;
    infoheader.biHeight = height;

    l_texture=(unsigned char*)malloc(infoheader.biWidth*infoheader.biHeight*4);

    memset(l_texture,0,infoheader.biWidth * infoheader.biHeight*4);
    //memset(l_texture,0,sizeof(l_texture));
    //printf("4444444444\n");
    for(i=0;i<infoheader.biWidth*infoheader.biHeight;i++)
    {
        fread(&rgb,sizeof(rgb),1,l_file);
        l_texture[j+0]=rgb.rgbtRed;
        l_texture[j+1]=rgb.rgbtGreen;
        l_texture[j+2]=rgb.rgbtBlue;
        l_texture[j+3]=255;
        j+=4;
    }
    fclose(l_file);
    glBindTexture(GL_TEXTURE_2D,num_texture);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    //�glTexParameterf(GL_TEXTURE_2D,�GL_TEXTURE_MIN_FILTER,�GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D,0,4,infoheader.biWidth,infoheader.biHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,l_texture);
    gluBuild2DMipmaps(GL_TEXTURE_2D,4,infoheader.biWidth,infoheader.biHeight,GL_RGBA,GL_UNSIGNED_BYTE,l_texture);
    free(l_texture);
    return(num_texture);
}

class Point3{
public:
	float x,y,z;
	
	void set(float dx,float dy, float dz)
	{
	  x=dx;
	  y=dy;
	  z=dz;
	}

	void set(Point3& p)
	{
	  x=p.x;
	  y=p.y;
	  z=p.z;
	
	}
    
	Point3(float xx,float yy,float zz)
	{
	  x=xx;
	  y=yy;
	  z=zz;
	 
	}
    
	Point3()
	{
	  x=0;
	  y=0;
	  z=0;
	}

};

class Vector3{
  public:
    float x,y,z;
	  
    void set(float dx,float dy, float dz)
    {
          x=dx;
          y=dy;
          z=dz;
    }
	void set(Vector3 const  &v)
	{
    	  x=v.x;
    	  y=v.y;
    	  z=v.z;
	
	}
    void flip()
	{
    	  x=-x;
    	  y=-y;
    	  z=-z;
	
	}
	void setDiff(Point3& a,Point3& b)
    {
         x=a.x-b.x;
    	 y=a.y-b.y;
    	 z=a.z-b.z;
   
    }
    void normalize()
    {
        double sizeSq = x*x+y*y+z*z;
    	
    	float d=(float)sqrt(sizeSq);
    	x /= d;
    	y /= d;
    	z /= d;
   
    }

    Vector3(float xx,float yy,float zz)
	{
    	 x=xx;
    	 y=yy;
    	 z=zz;
	}

    Vector3(Vector3 const &v)
	{
    	  x=v.x;
    	  y=v.y;
    	  z=v.z;
	}
    Vector3()
	{
    	  x=0;
    	  y=0;
    	  z=0;
	}
	float dot(Vector3 b)
    {
          return x*b.x+y*b.y+z*b.z;
    }
  
    Vector3 cross(Vector3 b)
    {
          Vector3 to_ret= Vector3( float(y*b.z - z*b.y) , float(z*b.x - x*b.z) , float(x*b.y - y*b.x) );
          return to_ret;
    }
};

class Camera_class{
   

   public:
       Point3 eye, look,up;
	   Vector3 u,v,n;
	   double viewAngle,aspect,nearDist,farDist;
	   void setModelviewMatrix();
	   Camera_class();
	   void set(Point3 eye,Point3 look,Vector3 up);

	   void slide(float delU,float delV,float delN);

	   void roll(float angle);
       
	   void pitch(float angle);

	   void yaw(float angle);

};

Camera_class::Camera_class()
{
     
}

void Camera_class::setModelviewMatrix(void)
{

        float m[16];
		Vector3 eVec(eye.x,eye.y,eye.z);
		m[0]=u.x; m[4]=u.y;  m[8]=u.z; m[12]=-eVec.dot(u);

		m[1]=v.x; m[5]=v.y;  m[9]=v.z; m[13]=-eVec.dot(v);

		m[2]=n.x; m[6]=n.y;  m[10]=n.z; m[14]=-eVec.dot(n);

		m[3]=0;  m[7]=0;  m[11]=0; m[15]=1;

		glMatrixMode(GL_MODELVIEW);

		glLoadMatrixf(m);
}

void Camera_class::set(Point3 Eye,Point3 look,Vector3 up)
{
       eye.set(Eye);
	   n.set(eye.x-look.x,eye.y-look.y,eye.z-look.z);
	   u.set(up.cross(n));
	   n.normalize();
	   u.normalize();
	   v.set(n.cross(u));
	   //printf("\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",eye.x,eye.y,eye.z,	cam.look.x,cam.look.y,cam.look.z,	cam.up.x,cam.up.y,cam.up.z);
	   //printf("\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",eye.x,eye.y,eye.z,u.x,u.y,u.z,v.x,v.y,v.z,n.x,n.y,n.z);
       setModelviewMatrix();

}


void Camera_class::slide(float delU,float delV,float delN)
{
		eye.x += delU*u.x+ delV*v.x + delN*n.x;
		eye.y += delU*u.y+ delV*v.y + delN*n.y;
		eye.z += delU*u.z+ delV*v.z + delN*n.z;

		setModelviewMatrix();



}


void Camera_class::roll(float angle)
{

		float cs=cos(3.14159265/180*angle);
		float sn=sin(3.14159265/180*angle);

		Vector3 t(u);

		u.set(cs*t.x-sn*v.x,cs*t.y-sn*v.y,cs*t.z-sn*v.z);
		v.set(sn*t.x+cs*v.x,sn*t.y+cs*v.y,sn*t.z+cs*v.z);
		
        setModelviewMatrix();


}


void Camera_class::pitch(float angle)
{


	float cs = cos( 3.14159265/180 * angle ) ;
    float sn = sin( 3.14159265/180 * angle ) ;
    
    Vector3 t( v ) ;

    v.set( cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z ) ;
    n.set( sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z ) ;

	setModelviewMatrix() ;


}



void Camera_class::yaw(float angle)
{



	float cs = cos( 3.14159265/180 * angle ) ;
    float sn = sin( 3.14159265/180 * angle ) ;

    Vector3 t( n ) ;

    n.set( cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z ) ;
    u.set( sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z ) ;

	setModelviewMatrix() ;


}
////////////////////////////////////////////////
GLfloat ctrlpoints[4][3] = {
	{ 62.0, 0, 75.0}, { 4.0, 0, 103.0}, 
	{16.0, 0, 103.0}, {-41.0, 0, 75.0},};

GLfloat ctrlpoints2[4][3] = {
	{ 50.0, 0, 40.0}, { 4.0, 0, 60.0}, 
	{16.0, 0, 60.0}, {-30.0, 0, 40.0}};
// cam;

GLfloat a1=.9,a2=.9,a3=.9;
GLfloat d1=.9,d2=.9,d3=.9;

///////////////////////////////////////////////////



Camera_class cam;

Point3 eye(0,zoom,200);
	
Point3 look(0,0,100);

Vector3 up(0,0,1);

void func(void)
{
    texid1=LoadBitmap("front.bmp", 256, 128);
    texid2=LoadBitmap("left.bmp", 128, 256);
    texid3=LoadBitmap("right.bmp", 128, 256);
    texid4=LoadBitmap("minar_tiles.bmp", 192, 128);
    texid5=LoadBitmap("minar_gliph.bmp", 192, 128);
    texid6=LoadBitmap("dome_gliph.bmp", 256, 128);
    texid7=LoadBitmap("black_stone.bmp", 128, 128);
    texid8=LoadBitmap("big_dome.bmp", 512, 256);
    texid9=LoadBitmap("tree.bmp", 128, 128);
    texid10=LoadBitmap("grass.bmp", 256, 256);
    texid11=LoadBitmap("water.bmp", 512, 256);
    texid12=LoadBitmap("brick.bmp", 128, 128);
    texid13=LoadBitmap("tiles.bmp", 512, 64);
    texid14=LoadBitmap("small_gliphs.bmp", 256, 128);
    texid15=LoadBitmap("red_stone.bmp", 128, 128);
    texid16=LoadBitmap("octagon_side.bmp", 1024, 64);
}

void initSkybox(void)
{
  skybox[SKY_FRONT] = LoadBitmap("txStormydays_front.bmp", 1024, 1024);
  skybox[SKY_RIGHT] = LoadBitmap("txStormydays_right.bmp", 1024, 1024);
  skybox[SKY_LEFT] = LoadBitmap("txStormydays_left.bmp", 1024, 1024);
  skybox[SKY_BACK] = LoadBitmap("txStormydays_back.bmp", 1024, 1024);
  skybox[SKY_UP] = LoadBitmap("txStormydays_up.bmp", 1024, 1024);
  skybox[SKY_DOWN] = LoadBitmap("txStormydays_down.bmp", 1024, 1024);
}

float white[]={1,1,1,1};
double D = 50 * 50;

void drawtex()
{
     
     glEnable(GL_TEXTURE_2D);{
         glPushMatrix();{
         glRotatef(90,1,0,0);
         glTranslatef(0,300,0);                     
         glPushMatrix();{
             glColor3fv(white);
             glBindTexture(GL_TEXTURE_2D,skybox[SKY_RIGHT]);
             glBegin(GL_QUADS);
                 glTexCoord2f(0,0); glVertex3f(-D,-D,-D);
                 glTexCoord2f(1,0); glVertex3f(+D,-D,-D);
                 glTexCoord2f(1,1); glVertex3f(+D,+D,-D);
                 glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
             glEnd();
  
             glBindTexture(GL_TEXTURE_2D,skybox[SKY_FRONT]);
             glBegin(GL_QUADS);
                 glTexCoord2f(0,0); glVertex3f(+D,-D,-D);
                 glTexCoord2f(1,0); glVertex3f(+D,-D,+D);
                 glTexCoord2f(1,1); glVertex3f(+D,+D,+D);
                 glTexCoord2f(0,1); glVertex3f(+D,+D,-D);
             glEnd();
  
             glBindTexture(GL_TEXTURE_2D,skybox[SKY_LEFT]);
             glBegin(GL_QUADS);
                 glTexCoord2f(0,0); glVertex3f(+D,-D,+D);
                 glTexCoord2f(1,0); glVertex3f(-D,-D,+D);
                 glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
                 glTexCoord2f(0,1); glVertex3f(+D,+D,+D);
             glEnd();
             
             glBindTexture(GL_TEXTURE_2D,skybox[SKY_BACK]);
             glBegin(GL_QUADS);
                 glTexCoord2f(0,0); glVertex3f(-D,-D,+D);
                 glTexCoord2f(1,0); glVertex3f(-D,-D,-D);
                 glTexCoord2f(1,1); glVertex3f(-D,+D,-D);
                 glTexCoord2f(0,1); glVertex3f(-D,+D,+D);
             glEnd();

             /* Top and Bottom */
             glBindTexture(GL_TEXTURE_2D,skybox[SKY_UP]);
             glBegin(GL_QUADS);
                 glTexCoord2f(0,0); glVertex3f(-D,+D,-D);
                 glTexCoord2f(1,0); glVertex3f(+D,+D,-D);
                 glTexCoord2f(1,1); glVertex3f(+D,+D,+D);
                 glTexCoord2f(0,1); glVertex3f(-D,+D,+D);
             glEnd();
             
             glBindTexture(GL_TEXTURE_2D,skybox[SKY_DOWN]);
             glBegin(GL_QUADS);
                 glTexCoord2f(1,1); glVertex3f(+D,-D,-D);
                 glTexCoord2f(0,1); glVertex3f(-D,-D,-D);
                 glTexCoord2f(0,0); glVertex3f(-D,-D,+D);
                 glTexCoord2f(1,0); glVertex3f(+D,-D,+D);
             glEnd();
             
         }glPopMatrix();
         }glPopMatrix();
         
         // Wall 1
         glPushMatrix();{
             glRotatef(0,0,0,0);
             glTranslatef(0,0,0);
             
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             /////////////////////////////////Design of the Wall //////////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D,texid1);
             glEnable(GL_BLEND);
             glColor4f(1,1,1,1);
             glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
             glAlphaFunc(GL_GREATER,.1);
             glEnable( GL_ALPHA_TEST );
         
             glPushMatrix(); {                                                       // middle wall                                                            
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-100, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-100, 0, 100);
                     glTexCoord2f(1,1); glVertex3f(100, 0, 100);
                     glTexCoord2f(1,0); glVertex3f(100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid2);
             glPushMatrix(); {                                                       // left wall
                 glNormal3f(0,0,1);
                 glTranslatef(250,0,0);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid3);
             glPushMatrix(); {                                                       // right wall                                                         
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0); glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1); glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
             
         }glPopMatrix();
         
         // Wall 2
         glPushMatrix();{
             glRotatef(90,0,0,1);
             glTranslatef(-167,170,0);
             
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             /////////////////////////////////Design of the Wall //////////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D,texid1);
             glEnable(GL_BLEND);
             glColor4f(1,1,1,1);
             glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
             glAlphaFunc(GL_GREATER,.1);
             glEnable( GL_ALPHA_TEST );
         
             glPushMatrix(); {                                                       // middle wall                                                            
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-100, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-100, 0, 100);
                     glTexCoord2f(1,1); glVertex3f(100, 0, 100);
                     glTexCoord2f(1,0); glVertex3f(100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid2);
             glPushMatrix(); {                                                       // left wall
                 glNormal3f(0,0,1);
                 glTranslatef(250,0,0);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid3);
             glPushMatrix(); {                                                       // right wall                                                         
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
             
         }glPopMatrix();
         
         // Wall 3
         glPushMatrix();{
             glRotatef(90,0,0,1);
             glTranslatef(-167,-170,0);
             
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             /////////////////////////////////Design of the Wall //////////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D,texid1);
             glEnable(GL_BLEND);
             glColor4f(1,1,1,1);
             glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
             glAlphaFunc(GL_GREATER,.1);
             glEnable( GL_ALPHA_TEST );
         
             glPushMatrix(); {                                                       // middle wall                                                            
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-100, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-100, 0, 100);
                     glTexCoord2f(1,1); glVertex3f(100, 0, 100);
                     glTexCoord2f(1,0); glVertex3f(100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid2);
             glPushMatrix(); {                                                       // left wall
                 glNormal3f(0,0,1);
                 glTranslatef(250,0,0);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid3);
             glPushMatrix(); {                                                       // right wall                                                         
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
             
         }glPopMatrix();
         
         // Wall 4
         glPushMatrix();{
             glRotatef(0,0,0,0);
             glTranslatef(0,-330,0);
             
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             /////////////////////////////////Design of the Wall //////////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D,texid1);
             glEnable(GL_BLEND);
             glColor4f(1,1,1,1);
             glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
             glAlphaFunc(GL_GREATER,.1);
             glEnable( GL_ALPHA_TEST );
         
             glPushMatrix(); {                                                       // middle wall                                                            
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-100, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-100, 0, 100);
                     glTexCoord2f(1,1); glVertex3f(100, 0, 100);
                     glTexCoord2f(1,0); glVertex3f(100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid2);
             glPushMatrix(); {                                                       // left wall
                 glNormal3f(0,0,1);
                 glTranslatef(250,0,0);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D,texid3);
             glPushMatrix(); {                                                       // right wall                                                         
                 glNormal3f(0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-150, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(-150, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(-100, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(-100, 0, 0);
                 }glEnd();
             } glPopMatrix();
             
         }glPopMatrix();
         
         
         
         // Ceiling
         glPushMatrix();{
             glBindTexture(GL_TEXTURE_2D,texid15);
             glTranslatef(0,0,90);
             glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-170, 0, 0);
                     glTexCoord2f(0,10);  glVertex3f(-170, -330, 0);
                     glTexCoord2f(10,10); glVertex3f(170, -330, 0);
                     glTexCoord2f(10,0); glVertex3f(170, 0, 0);
             }glEnd();
             
             glPushMatrix();{
                 glPushMatrix();{
                     glTranslatef(90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(-10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(-30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(-50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(-70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(-90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
             }glPopMatrix();
             
             glPushMatrix();{
                 glPushMatrix();{
                     glTranslatef(0,-308,0);
                     glTranslatef(90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);
                     glTranslatef(70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);
                     glTranslatef(-10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(-30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(-50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(-70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glTranslatef(0,-308,0);            
                     glTranslatef(-90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
             }glPopMatrix();
             
             glPushMatrix();{
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);
                     glTranslatef(90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);
                     glTranslatef(70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);            
                     glTranslatef(50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);            
                     glTranslatef(30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);           
                     glTranslatef(10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);
                     glTranslatef(-10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);            
                     glTranslatef(-30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);            
                     glTranslatef(-50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);            
                     glTranslatef(-70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,-148,0);            
                     glTranslatef(-90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
             }glPopMatrix();
             
             glPushMatrix();{
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);
                     glTranslatef(90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);
                     glTranslatef(70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);           
                     glTranslatef(50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);           
                     glTranslatef(30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);           
                     glTranslatef(10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);
                     glTranslatef(-10,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);            
                     glTranslatef(-30,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);            
                     glTranslatef(-50,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);            
                     glTranslatef(-70,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
                 glPushMatrix();{
                     glRotatef(90,0,0,1);
                     glTranslatef(-167,170,0);           
                     glTranslatef(-90,-11,0);
                     glutSolidCube(20);
                 }glPopMatrix();
             }glPopMatrix();
             
         }glPopMatrix();
         
         
         
         
         // Minar 1
         glPushMatrix();{
                         
             glTranslatef(170,0,0);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             ////////////////////////////////Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // lower part of the minar
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,0);
                 gluCylinder(quadric, 20, 20, 100, 6, 20);
             }glPopMatrix();
             
             glBindTexture(GL_TEXTURE_2D, texid5);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 20, 20, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                           // minar carnish
                 glColor3f(0.99,0.62,0.55);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(20, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
             glBindTexture(GL_TEXTURE_2D, texid14);
             glPushMatrix();{                                                        // lower part of the dome
                 glTranslatef(0,0,150);
                 gluCylinder(quadric, 20, 20, 15, 6, 20);
             }glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.99,0.62,0.55);
                 glTranslatef(0,0,172);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 10, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,186);
		         glutSolidCone(10,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,190);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,204);
                 glScalef(2, 2, 2);
		         glutSolidSphere(2, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         
         // Minar 2
         glPushMatrix();{
                         
             glTranslatef(-170,0,0);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             ////////////////////////////////Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // lower part of the minar
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,0);
                 gluCylinder(quadric, 20, 20, 100, 6, 20);
             }glPopMatrix();
             
             glBindTexture(GL_TEXTURE_2D, texid5);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 20, 20, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(20, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
             glBindTexture(GL_TEXTURE_2D, texid5);
             glPushMatrix();{                                                        // lower part of the dome
                 glTranslatef(0,0,150);
                 gluCylinder(quadric, 20, 20, 15, 6, 20);
             }glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,172);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 10, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,186);
		         glutSolidCone(10,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,190);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,204);
                 glScalef(2, 2, 2);
		         glutSolidSphere(2, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Minar 3
         glPushMatrix();{
                         
             glTranslatef(170,-330,0);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             ////////////////////////////////Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // lower part of the minar
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,0);
                 gluCylinder(quadric, 20, 20, 100, 6, 20);
             }glPopMatrix();
             
             glBindTexture(GL_TEXTURE_2D, texid5);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 20, 20, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(20, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
             glBindTexture(GL_TEXTURE_2D, texid5);
             glPushMatrix();{                                                        // lower part of the dome
                 glTranslatef(0,0,150);
                 gluCylinder(quadric, 20, 20, 15, 6, 20);
             }glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,172);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 10, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,186);
		         glutSolidCone(10,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,190);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,204);
                 glScalef(2, 2, 2);
		         glutSolidSphere(2, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Minar 4
         glPushMatrix();{
                         
             glTranslatef(-170,-330,0);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             ////////////////////////////////Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // lower part of the minar
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,0);
                 gluCylinder(quadric, 20, 20, 100, 6, 20);
             }glPopMatrix();
             
             glBindTexture(GL_TEXTURE_2D, texid5);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 20, 20, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(20, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
             glBindTexture(GL_TEXTURE_2D, texid5);
             glPushMatrix();{                                                        // lower part of the dome
                 glTranslatef(0,0,150);
                 gluCylinder(quadric, 20, 20, 15, 6, 20);
             }glPopMatrix();
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,172);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 10, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,186);
		         glutSolidCone(10,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,190);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,204);
                 glScalef(2, 2, 2);
		         glutSolidSphere(2, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         
         
         
         // Small Minar 1
         glPushMatrix();{
                         
             glTranslatef(90,-8,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             
             glPushMatrix();{                                                     // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 2
         glPushMatrix();{
                         
             glTranslatef(-90,-8,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 3
         glPushMatrix();{
                         
             glTranslatef(-90,-321,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 4
         glPushMatrix();{
                         
             glTranslatef(90,-321,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 5
         glPushMatrix();{
             glRotatef(90,0,0,1);            
             glTranslatef(-80,-162,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 6
         glPushMatrix();{
             glRotatef(90,0,0,1);            
             glTranslatef(-257,-162,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 7
         glPushMatrix();{
             glRotatef(90,0,0,1);            
             glTranslatef(-257,162,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Small Minar 8
         glPushMatrix();{
             glRotatef(90,0,0,1);            
             glTranslatef(-80,+162,-10);
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             //////////////////////////////// Small Minar Body Design//////////////////////////////////////////////////////////////
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
             
             glBindTexture(GL_TEXTURE_2D, texid4);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // minar gliphs
                 glRotatef(90,0,0,1);
                 glTranslatef(0,0,100);
                 gluCylinder(quadric, 10, 10, 50, 6, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // minar carnish
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,0,150);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(8, 6, 3);
             }glPopMatrix();
             
             //////////////////////////////////////////////////////////////////////////////////////////////////
             ///////////////////////////////// Small Minar Dome Design///////////////////////////////////////////////
             //////////////////////////////////////////////////////////////////////////////////////////////////
         
             glBindTexture(GL_TEXTURE_2D, texid6);
             glPushMatrix();{                                                        // main dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,156);
                 glScalef(2, 2, 1.8);
		         gluSphere(quadric, 5.2, 20, 20);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,161);
		         glutSolidCone(5,10,20,20);
             }glPopMatrix();
         
             gluQuadricTexture(quadric, GLU_FALSE);
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,0,155);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 25, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,0,174);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Big Dome
         glPushMatrix();{
             glTranslatef(0,0,-10);
             glBindTexture(GL_TEXTURE_2D, texid16);
             gluQuadricNormals(quadric,GLU_SMOOTH);
             gluQuadricTexture(quadric, GLU_TRUE);
             glPushMatrix();{                                                        // octagon 1
                 glTranslatef(0,-170,100);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 80, 80, 10, 8, 2);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // octagon 1 cover
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,-170,110);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(46, 8, 3);
             }glPopMatrix();
             
             glPushMatrix();{                                                        //  octagon 2
                 glTranslatef(0,-170,110);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 70, 70, 10, 8, 2);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // octagon 2 cover
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,-170,120);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(40, 8, 3);
             }glPopMatrix();
             
             glPushMatrix();{                                                        //  octagon 3
                 glTranslatef(0,-170,120);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 60, 60, 10, 8, 2);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // octagon 3 cover
                 glColor3f(0.62,0.21,0.17);
                 glTranslatef(0,-170,130);
                 glScalef(2, 2, 0.1);
		         glutSolidSphere(35, 8, 3);
             }glPopMatrix();
             
             glPushMatrix();{                                                        //  octagon 4
                 glTranslatef(0,-170,130);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 50, 50, 30, 8, 2);
             }glPopMatrix();
             
             glBindTexture(GL_TEXTURE_2D, texid8);
             glPushMatrix();{                                                        // big dome sphere
                 glTranslatef(0,-170,170);
                 glColor3f(0.34,0.91,0.74);
                 gluSphere(quadric, 50, 8, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // cone above the dome
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,-170,215);
		         glutSolidCone(20,20,20,20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // solid stick above the cone
                 glTranslatef(0,-170,230);
                 glColor3f(0.34,0.91,0.74);
                 gluCylinder(quadric, 1, 1, 40, 4, 2);
             }glPopMatrix();
         
             glPushMatrix();{                                                        // sphere in the middle of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,-170,250);
                 glScalef(2, 2, 2);
		         glutSolidSphere(3, 20, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // sphere in the bottom of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,-170,240);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
             
             glPushMatrix();{                                                        // sphere in the top of the stick
                 glColor3f(0.34,0.91,0.74);
                 glTranslatef(0,-170,260);
                 glScalef(2, 2, 2);
		         glutSolidSphere(1.5, 20, 20);
             }glPopMatrix();
         }glPopMatrix();
         
         // extra environment
         glPushMatrix();{
             //Kella Floor            
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid7);
                 glTranslatef(0,-175,0);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-248, -248, 0);
                     glTexCoord2f(0,20);  glVertex3f(-248, 248, 0);
                     glTexCoord2f(20,20);  glVertex3f(248, 248, 0);
                     glTexCoord2f(20,0); glVertex3f(248, -248, 0);
                 }glEnd();
             }glPopMatrix();
             glPushMatrix();{
                 glBindTexture(GL_TEXTURE_2D, texid7);
                 glTranslatef(0,-175,-10);
                 glRotatef(45,0,0,1);
                 gluCylinder(quadric, 350, 350, 10, 4, 1);
             }glPopMatrix();
             
             // red brick boundary
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(0,-175,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-348, -348, 0);
                     glTexCoord2f(0,20);  glVertex3f(-348, 348, 0);
                     glTexCoord2f(20,20);  glVertex3f(348, 348, 0);
                     glTexCoord2f(20,0); glVertex3f(348, -348, 0);
                 }glEnd();
             }glPopMatrix();
             
             //red brick 1
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(97,572,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-60, -400, 0);
                     glTexCoord2f(0,20);  glVertex3f(-60, 400, 0);
                     glTexCoord2f(4,20);  glVertex3f(60, 400, 0);
                     glTexCoord2f(4,0); glVertex3f(60, -400, 0);
                 }glEnd();
             }glPopMatrix();
             
             //red brick 2
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(-97,572,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-60, -400, 0);
                     glTexCoord2f(0,20);  glVertex3f(-60, 400, 0);
                     glTexCoord2f(4,20);  glVertex3f(60, 400, 0);
                     glTexCoord2f(4,0); glVertex3f(60, -400, 0);
                 }glEnd();
             }glPopMatrix();
             
             //red brick 3
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(90,0,0,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(-160,748,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-60, -400, 0);
                     glTexCoord2f(0,20);  glVertex3f(-60, 400, 0);
                     glTexCoord2f(4,20);  glVertex3f(60, 400, 0);
                     glTexCoord2f(4,0); glVertex3f(60, -400, 0);
                 }glEnd();
             }glPopMatrix();
             
             //red brick 4
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(-90,0,0,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(160,748,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-60, -400, 0);
                     glTexCoord2f(0,20);  glVertex3f(-60, 400, 0);
                     glTexCoord2f(4,20);  glVertex3f(60, 400, 0);
                     glTexCoord2f(4,0); glVertex3f(60, -400, 0);
                 }glEnd();
             }glPopMatrix();
             
             //red brick 4
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(180,0,0,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(0,923,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(-60, -400, 0);
                     glTexCoord2f(0,20);  glVertex3f(-60, 400, 0);
                     glTexCoord2f(4,20);  glVertex3f(60, 400, 0);
                     glTexCoord2f(4,0); glVertex3f(60, -400, 0);
                 }glEnd();
             }glPopMatrix();
             
             // grass field 1
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(180,0,0,1);
                 glTranslatef(147,-1000,-11);
                 glBindTexture(GL_TEXTURE_2D, texid10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,10);  glVertex3f(0, 1200, 0);
                     glTexCoord2f(10,10);  glVertex3f(1100, 1200, 0);
                     glTexCoord2f(10,0); glVertex3f(1100, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
             // grass field 2
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(180,0,0,1);
                 glTranslatef(-1255,-1000,-11);
                 glBindTexture(GL_TEXTURE_2D, texid10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,10);  glVertex3f(0, 1200, 0);
                     glTexCoord2f(10,10);  glVertex3f(1100, 1200, 0);
                     glTexCoord2f(10,0); glVertex3f(1100, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
             // grass field 3
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(180,0,0,1);
                 glTranslatef(-53,150,-11);
                 glBindTexture(GL_TEXTURE_2D, texid10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,10);  glVertex3f(0, 1200, 0);
                     glTexCoord2f(10,10);  glVertex3f(1300, 1200, 0);
                     glTexCoord2f(10,0); glVertex3f(1300, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
             // grass field 4
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glRotatef(180,0,0,1);
                 glTranslatef(-1255,150,-11);
                 glBindTexture(GL_TEXTURE_2D, texid10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,10);  glVertex3f(0, 1200, 0);
                     glTexCoord2f(10,10);  glVertex3f(1300, 1200, 0);
                     glTexCoord2f(10,0); glVertex3f(1300, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Pool 1
         glPushMatrix();{
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(0, 300, 0);
                     glTexCoord2f(1,1);  glVertex3f(0, 300, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(-37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(74, 0, 0);
                     glTexCoord2f(1,1);  glVertex3f(74, 0, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(-37,470,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(74, 0, 0);
                     glTexCoord2f(1,1);  glVertex3f(74, 0, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(-37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(0, 300, 0);
                     glTexCoord2f(1,1);  glVertex3f(0, 300, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             //water
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid11);
                 glTranslatef(-37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,4);  glVertex3f(0, 300, 0);
                     glTexCoord2f(1,4);  glVertex3f(74, 300, 0);
                     glTexCoord2f(1,0); glVertex3f(74, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
             //cylinders
             glPushMatrix();{
                 glTranslatef(0,25,0);
                 //pull cylinder 1
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,200,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,200,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 2
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,250,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,250,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 3
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,300,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,300,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 4
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,350,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,350,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 5
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,400,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,400,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
                 
             }glPopMatrix();
             
             //road after pool
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(-37,470,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,2);  glVertex3f(0, 70, 0);
                     glTexCoord2f(2,2);  glVertex3f(74, 70, 0);
                     glTexCoord2f(2,0); glVertex3f(74, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Pool 2
         glPushMatrix();{
             glTranslatef(0,370,0);
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(0, 300, 0);
                     glTexCoord2f(1,1);  glVertex3f(0, 300, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(-37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(74, 0, 0);
                     glTexCoord2f(1,1);  glVertex3f(74, 0, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(-37,470,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(74, 0, 0);
                     glTexCoord2f(1,1);  glVertex3f(74, 0, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid13);
                 glTranslatef(-37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,1);  glVertex3f(0, 300, 0);
                     glTexCoord2f(1,1);  glVertex3f(0, 300, 20);
                     glTexCoord2f(1,0); glVertex3f(0, 0, 20);
                 }glEnd();
             }glPopMatrix();
             
             //water
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid11);
                 glTranslatef(-37,170,-30);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,4);  glVertex3f(0, 300, 0);
                     glTexCoord2f(1,4);  glVertex3f(74, 300, 0);
                     glTexCoord2f(1,0); glVertex3f(74, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
             //cylinders
             glPushMatrix();{
                 glTranslatef(0,25,0);
                 //pull cylinder 1
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,200,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,200,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 2
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,250,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,250,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 3
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,300,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,300,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 4
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,350,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,350,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
             
                 //pull cylinder 5
                 glPushMatrix();{
                     glPushMatrix();{
                         glColor4f(1,1,1,1);
                         glBindTexture(GL_TEXTURE_2D, texid5);
                         glTranslatef(0,400,-30);
                         gluCylinder(quadric, 5, 5, 20, 10, 10);
                     }glPopMatrix();
                     glPushMatrix();{
                         glTranslatef(0,400,-10);
                         gluSphere(quadric, 6, 6, 6);
                     }glPopMatrix();
                 }glPopMatrix();
                 
             }glPopMatrix();
             
             //road after pool
             glPushMatrix();{
                 glColor4f(1,1,1,1);
                 glBindTexture(GL_TEXTURE_2D, texid12);
                 glTranslatef(-37,470,-10);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,4);  glVertex3f(0, 132, 0);
                     glTexCoord2f(2,4);  glVertex3f(74, 132, 0);
                     glTexCoord2f(2,0); glVertex3f(74, 0, 0);
                 }glEnd();
             }glPopMatrix();
             
         }glPopMatrix();
         
         // Trees
         glPushMatrix();{
             // tree 1
             int tr_x = 200, tr_y = 200, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 2
             tr_x = 300, tr_y = 200, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 3
             tr_x = 200, tr_y = 400, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                     
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 4
             tr_x = 200, tr_y = 600, tr_z = -30;
             glPushMatrix(); {                                                                                                             
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 5
             tr_x = 200, tr_y = 800, tr_z = -30;
             glPushMatrix(); {                                                                                                           
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                 
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 6
             tr_x = 400, tr_y = 100, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 7
             tr_x = 400, tr_y = 0, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 8
             tr_x = 400, tr_y = -300, tr_z = -30;
             glPushMatrix(); {                                                                                                             
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 9
             tr_x = 400, tr_y = -400, tr_z = -30;
             glPushMatrix(); {                                                                                                             
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 10
             tr_x = 250, tr_y = -600, tr_z = -30;
             glPushMatrix(); {                                                                                                            
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 11
             tr_x = 150, tr_y = -600, tr_z = -30;
             glPushMatrix(); {                                                                                                           
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 12
             tr_x = 150, tr_y = -800, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 13
             tr_x = 150, tr_y = -1000, tr_z = -30;
             glPushMatrix(); {                                                                                                             
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                     
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 14
             tr_x = 150, tr_y = -1200, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 15
             tr_x = -150, tr_y = -1200, tr_z = -30;
             glPushMatrix(); {                                                                                                            
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                     
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 16
             tr_x = -150, tr_y = -1000, tr_z = -30;
             glPushMatrix(); {                                                                                                            
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                  
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 17
             tr_x = -150, tr_y = -800, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 18
             tr_x = -150, tr_y = -600, tr_z = -30;
             glPushMatrix(); {                                                                                                          
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 19
             tr_x = -250, tr_y = -600, tr_z = -30;
             glPushMatrix(); {                                                                                                           
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                   
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 20
             tr_x = -400, tr_y = -400, tr_z = -30;
             glPushMatrix(); {                                                                                                           
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                     
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 21
             tr_x = -400, tr_y = -300, tr_z = -30;
             glPushMatrix(); {                                                                                                            
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 22
             tr_x = -400, tr_y = 0, tr_z = -30;
             glPushMatrix(); {                                                                                                             
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                  
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 23
             tr_x = -400, tr_y = 100, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 24
             tr_x = -300, tr_y = 200, tr_z = -30;
             glPushMatrix(); {                                                                                                            
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                     
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 25
             tr_x = -200, tr_y = 200, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 26
             tr_x = -200, tr_y = 400, tr_z = -30;
             glPushMatrix(); {                                                                                                             
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 27
             tr_x = -200, tr_y = 600, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                    
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             
             // tree 28
             tr_x = -200, tr_y = 800, tr_z = -30;
             glPushMatrix(); {                                                                                                              
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
             glPushMatrix(); {                                                                                                  
                 glBindTexture(GL_TEXTURE_2D, texid9);
                 glTranslatef(tr_x,tr_y,tr_z);
                 glRotatef(90,0,0,1);
                 glBegin(GL_POLYGON);{
                     glTexCoord2f(0,0);  glVertex3f(0, 0, 0);
                     glTexCoord2f(0,0);  glVertex3f(-15, 0, 30);
                     glTexCoord2f(0,1);  glVertex3f(-20, 0, 60);
                     glTexCoord2f(0,1); glVertex3f(-5, 0, 90);
                     glTexCoord2f(1,1); glVertex3f(5, 0, 90);
                     glTexCoord2f(1,0); glVertex3f(20, 0, 60);
                     glTexCoord2f(1,1); glVertex3f(15, 0, 30);
                 }glEnd();
             } glPopMatrix();
               
         }glPopMatrix();
         
     } glDisable(GL_TEXTURE_2D);
	
	
	



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
    
    if(cam_rot==1)
    {
        eye.x=sqrt(e_x*e_x+e_y*e_y)*cos(cameraAngle);
        eye.y=sqrt(e_x*e_x+e_y*e_y)*sin(cameraAngle);
        cam.set(eye,look,up);
        cameraAngle+=.005;
    }
    else if(cam_rot==-1)
    {
        eye.x=eye.x;
        eye.y=eye.y;
        eye.z=eye.z;
        cam.set(eye,look,up);
        cam_rot=0;
    }
    else if(cam_rot==2)
    {
        cameraAngle+=.005;
        eye.x=sqrt(e_x*e_x+e_y*e_y)*cos(cameraAngle);
        eye.y=sqrt(e_x*e_x+e_y*e_y)*sin(cameraAngle);
        cam.set(eye,look,up);
        cam_rot=-1;
    }
    else if(cam_rot==3)
    {
        cameraAngle-=.005;
        eye.x=sqrt(e_x*e_x+e_y*e_y)*cos(cameraAngle);
        eye.y=sqrt(e_x*e_x+e_y*e_y)*sin(cameraAngle);
        cam.set(eye,look,up);
        cam_rot=-1;
    }
    else if(cam_rot==4)
    {
        cameraAngle+=.005;
        eye.z=sqrt(e_z*e_z+e_y*e_y)*sin(cameraAngle);
        eye.y=sqrt(e_z*e_z+e_y*e_y)*cos(cameraAngle);
        cam.set(eye,look,up);
        cam_rot=-1;
    }
    else if(cam_rot==5)
    {
        cameraAngle-=.005;
        eye.z=sqrt(e_z*e_z+e_y*e_y)*sin(cameraAngle);
        eye.y=sqrt(e_z*e_z+e_y*e_y)*cos(cameraAngle);
        cam.set(eye,look,up);
        cam_rot=-1;
    }
    
    
	//MISSING SOMETHING? -- YES: add the following
	
	glutPostRedisplay();	//this will call the display AGAIN
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':	//reverse the rotation of camera
			brdg_opn=1;
			break;

		case '2':	//increase rotation of camera by 10%
			brdg_opn=-1;
			break;

	
			
		case 'f':	//reverse the rotation of camera
			cam.slide(0,0,-5);
			break;
			
		case 'b':	//reverse the rotation of camera
			cam.slide(0,0,5);
			break;
			
		case 'u':	//toggle grids
			cam.slide(0,5,0);
			break;
			
		case 'd':	//toggle grids
			cam.slide(0,-5,0);
			break;
			
		case 'l':	//toggle grids
			cam.slide(-5,0,0);
			break;
			
		case 'r':	//toggle grids
			cam.slide(5,0,0);
			break;
			
		case 'o':	//increase rotation of camera by 10%
		   cam.roll(2);
			break;

		case 'p':	//decrease rotation
			cam.pitch(2);
			break;

			
		case 'y':	//toggle grids
			cam.yaw(2);
			break;
		
		case 'q':	//increase rotation of camera by 10%
		   cam.roll(-2);
			break;

		case 'a':	//decrease rotation
			cam.pitch(-2);
			break;

			
		case 'z':	//toggle grids
			cam.yaw(-2);
			break;
			
		case 's':	//toggle grids
			show_strctr=1;
			break;
		case 't':	//toggle grids
			show_strctr=0;
			break;
			
		case 'c':	//toggle grids
			//printf("%lf %lf\n",car1_spd,car2_spd);
			//printf("%lf %lf %lf\n",eye.x,eye.y,cameraAngle);
			printf("%d %d %lf %lf\n",shp_dir,shp_rot,shp_spd,shp_rot_angle);
			//shp_spd=-400;
			break;

		case '3':	//ESCAPE KEY -- simply exit
		d1 -= .03;
		d2 -= .03;
		d3 -= .03;

        a1 -=.03;
		a2 -=.03;
		a3 -=.03;
		printf("%lf %lf %lf %lf %lf %lf\n",a1,a2,a3,d1,d2,d3);
		//	if(d1==0||d2==0||d3==0)
			//	d1=d2=d3=1;
			break;
			
		case '4':	//toggle grids
		    d1 += .03;
			d2 += .03;
			d3 += .03;
			a1 +=.03;
			a2 +=.03;
			a3 +=.03;

        	printf("%lf %lf %lf %lf %lf %lf\n",a1,a2,a3,d1,d2,d3);
		//	if(d1==1||d2==1||d3==1)
		//		d1=d2=d3=0;
			break;
			
		case '5':
             eye.x=0;eye.y=150;eye.z=200;
             look.x=0;look.y=0;look.z=100;	
             cameraAngle=3.1416/2;
             cam.set(eye,look,up);
			break;

		case '6':
             eye.x=180;eye.y=0;eye.z=100;
             look.x=0;look.y=0;look.z=30;
             cameraAngle=0;
             cam.set(eye,look,up);
             	
			break;
			
		case '7':
             eye.x=250;eye.y=0;eye.z=45;
             look.x=0;look.y=0;look.z=40;
             cameraAngle=0;
             cam.set(eye,look,up);
             
             
			break;

		case '8':
             eye.x=120;eye.y=0;eye.z=30;
             look.x=0;look.y=0;look.z=60;
             cameraAngle=0;
             cam.set(eye,look,up);	
             
			break;
			
		case '9':
             eye.x=180;eye.y=0;eye.z=10;
             look.x=0;look.y=0;look.z=40;
             cameraAngle=0;
             cam.set(eye,look,up);	
             		
			break;

		case '0':	
             eye.x=0;eye.y=400;eye.z=100;
             look.x=0;look.y=0;look.z=40;
             cameraAngle=3.1416/2;
             cam.set(eye,look,up);
            
			break;
		
		case 'n':
             printf("%lf %f %f\n",cameraAngle,eye.x,eye.y);
             e_z=eye.z;
             e_y=eye.y;
             e_x=eye.x;
             cam_rot=-1;
             
             break;
        
        case 'm':
             
             //cam_rot=1;
             e_z=eye.z;
             e_y=eye.y;
             e_x=eye.x;
             cam_rot=1;
             
             break;
             

		case 27:	//ESCAPE KEY -- simply exit
			exit(0);
			break;

		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
                
		case GLUT_KEY_DOWN:
			 eye.y+=5;
             cam.set(eye,look,up);
             break;
			 
		case GLUT_KEY_UP:
             eye.y-=5;
             cam.set(eye,look,up);
             break;
             
		case GLUT_KEY_RIGHT:
             e_z=eye.z;
             e_y=eye.y;
             e_x=eye.x;
             cam_rot=2;
             break;
			
		case GLUT_KEY_LEFT:
             e_z=eye.z;
             e_y=eye.y;
             e_x=eye.x;
             cam_rot=3;
			 break;

		case GLUT_KEY_PAGE_UP:
		     e_z=eye.z;
             e_y=eye.y;
             e_x=eye.x;
             cam_rot=4;
			 break;
			 
		case GLUT_KEY_PAGE_DOWN:
			 e_z=eye.z;
             e_y=eye.y;
             e_x=eye.x;
             cam_rot=5;
			 break;

		case GLUT_KEY_INSERT:
			 break;

		case GLUT_KEY_HOME:
			 break;
		case GLUT_KEY_END:
			 break;

		default:
			 break;
	}
}

void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				cam.roll(5);	
		}
			break;

		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				cam.pitch(5);	
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

void init(){
	
	cameraAngle = 0;	//// init the cameraAngle
	//cameraAngleDelta = 0.006;
	//rectAngle = 0;
	//canDrawGrid = true;
	cameraHeight = 150;
	cameraRadius = 150;
	
    quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
	glEnable(GL_NORMALIZE);
	glClearColor(GROUND, 0);
	

	glClearDepth(1);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHT1);


  	glEnable(GL_COLOR_MATERIAL);
    
	glShadeModel(GL_FLAT);
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
	
    glEnable(GL_MAP1_VERTEX_3);
	
	glEnable(GL_AUTO_NORMAL);
    glEnable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE);
    glEnable(GL_NORMALIZE);


    func();
    initSkybox();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(70,	1,	0.1,	10000.0);

}

void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(GROUND, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /////////////////////////////////////////////////////////////////////////////////
  	GLfloat qaAmbientLight[]	= {a1, a2, a3, 1.0};
	GLfloat qaDiffuseLight[]	= {d1, d2, d3, 1.0};
	GLfloat qaSpecularLight[]	= {1.0, 1.0, 1.0, 1.0};

   /////////////////////////////////////////////////////////////////////////////////
   	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);


	glLightfv(GL_LIGHT1, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, qaSpecularLight);



    float lightPosition[4] = {500, 500, 500.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


	float lightPosition1[4] = {-500, 500, 500.0, 1.0};
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);

     if(!show_strctr)
     drawtex();

     glDisable (GL_BLEND);
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Lalbag Fort");

    init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	//ADD keyboard listeners:
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	//glutKeyboardUpFunc(keyUp);

	//ADD mouse listeners:
	glutMouseFunc(mouseListener);
	
	cam.set(eye,look,up);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}

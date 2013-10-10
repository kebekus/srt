
#include <QSettings>
#include <QtOpenGL>
#include <QtOpenGL>

#include <math.h>

#include "CoordinateWidget.h"

#define W 2.0
#define L 7.0


CoordinateWidget::CoordinateWidget(QWidget *parent)
    : QGLWidget(parent)
{
}

CoordinateWidget::~CoordinateWidget()
{
  makeCurrent();
  for(int i=0; i<objects.size(); i++)
    glDeleteLists(objects[i], 1);
}


void CoordinateWidget::initializeGL()
{
    static const GLfloat lightPos[4] = { 10.0f, 10.0f, 0.0f, 1.0f };
    static const GLfloat xx[4] = {0.8f, 0.8f, 0.8f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, xx);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    const double Pi = 3.14159265358979323846;
    objects.append(makeLine( QVector3D(0,0,0),  QVector3D(5, 0, 0), 0.3 ));
    objects.append(makeLine( QVector3D(0,0,0),  QVector3D(0, 5, 0), 0.3 ));
    objects.append(makeLine( QVector3D(0,0,0),  QVector3D(0, 0, 5), 0.3 ));

    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void CoordinateWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable (GL_BLEND);
  glEnable (GL_POLYGON_SMOOTH);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_MULTISAMPLE);

  glPushMatrix();
  GLdouble m[16];
  
  m[ 0] = _newX.x();   m[ 1] = _newY.x();   m[ 2] = _newZ.x();   m[ 3] = 0.0;
  m[ 4] = _newX.y();   m[ 5] = _newY.y();   m[ 6] = _newZ.y();   m[ 7] = 0.0;
  m[ 8] = _newX.z();   m[ 9] = _newY.z();   m[10] = _newZ.z();   m[11] = 0.0;
  m[12] = 0.0;         m[13] = 0.0;         m[14] = 0.0;         m[15] = 1.0;

  /*
  m[ 0] = _newX.x();   m[ 1] = _newX.y();   m[ 2] = _newX.z();   m[ 3] = 0.0;
  m[ 4] = _newY.x();   m[ 5] = _newY.y();   m[ 6] = _newY.z();   m[ 7] = 0.0;
  m[ 8] = _newZ.x();   m[ 9] = _newZ.y();   m[10] = _newZ.z();   m[11] = 0.0;
  m[12] = 0.0;         m[13] = 0.0;         m[14] = 0.0;         m[15] = 1.0;
  */

  glMultMatrixd(m);
#warning need to do things here  

  for(int i=0; i<objects.size(); i++)
    drawGear(objects[i], 0.0, 0.0, 0.0, 0.0);
  
  glPopMatrix();
}

void CoordinateWidget::resizeGL(int width, int height)
{
  int side = qMin(width, height);
  glViewport((width - side) / 2, (height - side) / 2, side, side);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0.0, 0.0, -40.0);
}


GLuint CoordinateWidget::makeLine(QVector3D A, QVector3D B, qreal radius)
{
    const double Pi = 3.14159265358979323846;

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    static const GLfloat reflectance1[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const GLfloat reflectance2[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance1);
    glMaterialfv(GL_BACK,  GL_AMBIENT_AND_DIFFUSE, reflectance2);

    glShadeModel(GL_SMOOTH);

    QVector3D H(1,1,1);
    QVector3D N1 = QVector3D::normal( A, B, H );
    QVector3D N2 = QVector3D::normal( A, B, N1 );

    int divisions = 20;

    // Add Stem of spacial arrow: cylinder
    glBegin(GL_QUAD_STRIP);
    for(int j=0; j<=divisions; j++) {
      double Zn = 0 + j*2*Pi/divisions;
      double Zm = 0 + (j+1)*2*Pi/divisions;
      
      QVector3D V1 = radius*sin(Zn)*N1 + radius*cos(Zn)*N2 + A;
      QVector3D V2 = radius*sin(Zn)*N1 + radius*cos(Zn)*N2 + B;
      QVector3D V3 = radius*sin(Zm)*N1 + radius*cos(Zm)*N2 + A;
      QVector3D V4 = radius*sin(Zm)*N1 + radius*cos(Zm)*N2 + B;

      QVector3D N = sin(0.5*(Zn+Zm))*N1 + cos(0.5*(Zn+Zm))*N2;

      glNormal3d( N.x(), N.y(), N.z());
      glVertex3d( V1.x(), V1.y(), V1.z() );
      glVertex3d( V2.x(), V2.y(), V2.z() );
      /*
      glVertex3d( V3.x(), V3.y(), V3.z() );
      glVertex3d( V4.x(), V4.y(), V4.z() );
      */
    }
    glEnd();
    

    // Add Stem of spacial arrow: bottom cap
    glBegin(GL_TRIANGLE_FAN);
    QVector3D normal = (A-B).normalized();
    glNormal3d( normal.x(), normal.y(), normal.z() );
    glVertex3d( A.x(), A.y(), A.z() );
    for(int j=0; j<=divisions; j++) {
      double Zn = 0 + j*2*Pi/divisions;
      QVector3D V1 = radius*sin(Zn)*N1 + radius*cos(Zn)*N2 + A;
      glVertex3d( V1.x(), V1.y(), V1.z() );
    }
    glEnd();

    // Add Arrow tip:  cap
    glBegin(GL_TRIANGLE_FAN);
    normal = (A-B).normalized();
    glNormal3d( normal.x(), normal.y(), normal.z() );
    glVertex3d( B.x(), B.y(), B.z() );
    for(int j=0; j<=divisions; j++) {
      double Zn = 0 + j*2*Pi/divisions;
      
      QVector3D V1 = 2*radius*sin(Zn)*N1 + 2*radius*cos(Zn)*N2 + B;
      glVertex3d( V1.x(), V1.y(), V1.z() );
    }
    glEnd();

    // Add Arrow tip:  cone
    /*
    glBegin(GL_TRIANGLES);
    for(int j=0; j<=divisions; j++) {
      double Zn = 0 + j*2*Pi/divisions;
      double Zm = 0 + (j+1)*2*Pi/divisions;
      
      QVector3D V1 = 2*radius*sin(Zn)*N1 + 2*radius*cos(Zn)*N2 + B;
      QVector3D V3 = 2*radius*sin(Zm)*N1 + 2*radius*cos(Zm)*N2 + B;
      QVector3D tip = B + 2*(B-A).normalized();
      QVector3D N = -QVector3D::crossProduct(tip-V3, V1-V3).normalized();

      glNormal3d( N.x(), N.y(), N.z() );
      glVertex3d( tip.x(), tip.y(), tip.z() );
      glVertex3d( V3.x(), V3.y(), V3.z() );
      glVertex3d( V1.x(), V1.y(), V1.z() );

    }
    glEnd();
     */    
    glBegin(GL_TRIANGLE_FAN);
    QVector3D tip = B + 2*(B-A).normalized();
    glVertex3d( tip.x(), tip.y(), tip.z() );
    for(int j=0; j<=divisions; j++) {
      double Zn = 0 + j*2*Pi/divisions;
      double Zm = 0 + (j+1)*2*Pi/divisions;
      
      QVector3D V1 = 2*radius*sin(Zn)*N1 + 2*radius*cos(Zn)*N2 + B;
      QVector3D V3 = 2*radius*sin(Zm)*N1 + 2*radius*cos(Zm)*N2 + B;
      QVector3D N = QVector3D::crossProduct(tip-V3, V1-V3).normalized();

      glNormal3d( N.x(), N.y(), N.z() );
      glVertex3d( V1.x(), V1.y(), V1.z() );
    }
    glEnd();

    
    glEndList();

    return list;
}

void CoordinateWidget::drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
                        GLdouble angle)
{
  glPushMatrix();
  glCallList(gear);
  glPopMatrix();
}

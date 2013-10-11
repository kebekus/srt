/***************************************************************************
 *   Copyright (C) 2013 Stefan Kebekus                                     *
 *   stefan.kebekus@math.uni-freiburg.de                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <math.h>
#include <QVector3D>

#include "CoordinateWidget.h"


namespace srtQt {

CoordinateWidget::CoordinateWidget(QWidget *parent)
  : QGLWidget(parent)
{
  _backgroundColor = Qt::black;
  _redArrow        = QVector3D(1,0,0);
  _greenArrow      = QVector3D(0,1,0);
  _blueArrow       = QVector3D(0,0,1);
}


CoordinateWidget::~CoordinateWidget()
{
  makeCurrent();
  
  glDeleteLists(_redArrowID, 1);
  glDeleteLists(_greenArrowID, 1);
  glDeleteLists(_blueArrowID, 1);
}


void CoordinateWidget::setArrowVectors(QVector3D redArrow, QVector3D greenArrow, QVector3D blueArrow)
{
  _redArrow   = redArrow;
  _greenArrow = greenArrow;
  _blueArrow  = blueArrow;
  update();
};
  

void CoordinateWidget::setBackgroundColor(QColor backgroundColor) 
{
  _backgroundColor = backgroundColor;
  update();
};

void CoordinateWidget::initializeGL()
{
  // Global GL Options 
  glEnable(GL_DEPTH_TEST);
  
  //
  // Initialize light
  //
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  static const GLfloat lightPos[4] = { 0.0f, 10.0f, 10.0f, 0.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  static const GLfloat light0ambientColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0ambientColor);
  static const GLfloat light0diffuseColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0diffuseColor);

  // Construct three arrow objects with differently colored tips 
  _redArrowID   = makeArrow(QVector3D(5.6, 0.0, 0.0), Qt::red);
  _greenArrowID = makeArrow(QVector3D(0.0, 5.6, 0.0), Qt::green);
  _blueArrowID  = makeArrow(QVector3D(0.0, 0.0, 5.6), Qt::blue);
}


void CoordinateWidget::paintGL()
{
  // Set background color
  glClearColor(_backgroundColor.redF(), _backgroundColor.greenF(), _backgroundColor.blueF(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  
  // Rotate so that arrows point in the appropriate directions
  GLdouble m[16];
  m[ 0] = _redArrow.x();   m[ 1] = _greenArrow.x();   m[ 2] = _blueArrow.x();   m[ 3] = 0.0;
  m[ 4] = _redArrow.y();   m[ 5] = _greenArrow.y();   m[ 6] = _blueArrow.y();   m[ 7] = 0.0;
  m[ 8] = _redArrow.z();   m[ 9] = _greenArrow.z();   m[10] = _blueArrow.z();   m[11] = 0.0;
  m[12] = 0.0;             m[13] = 0.0;               m[14] = 0.0;              m[15] = 1.0;
  glMultMatrixd(m);

  glCallList(_redArrowID);
  glCallList(_greenArrowID);
  glCallList(_blueArrowID);

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


GLuint CoordinateWidget::makeArrow(QVector3D B, QColor tipColor)
{
  const qreal radius    = 0.4;
  const qreal Pi        = 3.14159265358979323846;
  const int   divisions = 10;  
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  
  // Set material: white
  GLfloat materialEmissionColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glMaterialfv(GL_FRONT, GL_EMISSION, materialEmissionColor);
  GLfloat materialColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);

 
  // Construct two vectors N1, N2 that are normal to B
  QVector3D H(1,1,1);
  QVector3D N1 = QVector3D::normal( B, H );
  QVector3D N2 = QVector3D::normal( B, N1 );

  
  // Add Stem of spacial arrow: cylinder
  glBegin(GL_QUAD_STRIP);
  for(int j=0; j<=divisions; j++) {
    double Zn = 0 + j*2*Pi/divisions;
    
    QVector3D V1 = radius*sin(Zn)*N1 + radius*cos(Zn)*N2;
    QVector3D V2 = radius*sin(Zn)*N1 + radius*cos(Zn)*N2 + B;
    QVector3D N  = sin(Zn)*N1 + cos(Zn)*N2;
    
    glNormal3d( N.x(), N.y(), N.z());
    glVertex3d( V1.x(), V1.y(), V1.z() );
    glVertex3d( V2.x(), V2.y(), V2.z() );
  }
  glEnd();
  
  // Add Stem of spacial arrow: bottom cap
  glBegin(GL_TRIANGLE_FAN);
  QVector3D normal = -B.normalized();
  glNormal3d( normal.x(), normal.y(), normal.z() );
  glVertex3d(0.0f, 0.0f, 0.0f);
  for(int j=0; j<=divisions; j++) {
    double Zn = 0 + j*2*Pi/divisions;
    QVector3D V1 = radius*sin(Zn)*N1 + radius*cos(Zn)*N2;
    glVertex3d( V1.x(), V1.y(), V1.z() );
  }
  glEnd();
  
  // Add arrow tip:  cap
  glBegin(GL_TRIANGLE_FAN);
  glVertex3d( B.x(), B.y(), B.z() );
  for(int j=0; j<=divisions; j++) {
    double Zn = 0 + j*2*Pi/divisions;
    QVector3D V1 = 2*radius*sin(Zn)*N1 + 2*radius*cos(Zn)*N2 + B;
    glVertex3d( V1.x(), V1.y(), V1.z() );
  }
  glEnd();
  
  // Add colored arrow tip:  cone
  GLfloat tColor[] = {tipColor.redF(), tipColor.greenF(), tipColor.blueF(), 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tColor);
  glBegin(GL_QUAD_STRIP);
  QVector3D tip = B + 2*B.normalized();
  for(int j=0; j<=divisions; j++) {
    double Zn = 0 + j*2*Pi/divisions;
    
    QVector3D V1 = 2*radius*sin(Zn)*N1 + 2*radius*cos(Zn)*N2 + B;
    QVector3D Vx = cos(Zn)*N1 - sin(Zn)*N2;
    QVector3D N  = QVector3D::crossProduct(tip-V1, Vx).normalized();
    
    glNormal3d( N.x(), N.y(), N.z() );
    glVertex3d( tip.x(), tip.y(), tip.z() );
    glVertex3d( V1.x(), V1.y(), V1.z() );
  }
  glEnd();
  
  glEndList();
  
  return list;
}


} // namespace srtQt

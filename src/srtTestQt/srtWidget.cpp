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
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

#include "srtWidget.h"


srtWidget::srtWidget(QWidget *parent)
  : QFrame(parent), rotationTimer(this)
{
  // Initialize Members
  angle = 0;
  axis  = QVector3D(0,1,0);

  connect(&rotationTimer, SIGNAL(timeout()), this, SLOT(rotate()));

  // Test only
  grabGesture(Qt::TapGesture);
  grabGesture(Qt::TapAndHoldGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::PinchGesture);
  grabGesture(Qt::SwipeGesture);
}


srtWidget::~srtWidget()
{
}


void srtWidget::setScene(srtScene *_scene)
{
  scene = _scene;
  if (scene.isNull())
    return;

  connect(scene, SIGNAL(changed()), this, SLOT(update()));
  connect(&scene->surface, SIGNAL(equationChanged()), &rotationTimer, SLOT(stop()));
}


bool srtWidget::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture) {
    qDebug() << "A Gesture!  Hallelujah!";
    return false;
  }
  return QWidget::event(event);
}


void srtWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  
  painter.fillRect( QRectF(frameRect().left()+frameWidth(), frameRect().top()+frameWidth(), frameRect().width()-2*frameWidth(), frameRect().height()-2*frameWidth()),
		    Qt::black);

  if (!scene.isNull())
    if (!scene->surface.isEmpty() && !scene->surface.hasError()) {
      int width  = frameRect().width()-2*frameWidth();
      int height = frameRect().height()-2*frameWidth();

      QImage img = scene->draw( QSize(width, height) );
      if (!img.isNull()) {
	//	QImage scaled = img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	painter.drawImage( QPoint(frameWidth(), frameWidth()), img);
      }
    }

  painter.end();
  QFrame::paintEvent(event);
}


void srtWidget::mousePressEvent(QMouseEvent *event )
{
  if (event->button() != Qt::LeftButton) 
    return;

  rotationTimer.stop();
  stopWatch.start();

  originalXPos    = event->globalX();
  originalYPos    = event->globalY();
  angle           = 0.0;
  rotationalSpeed = 0.0;

  event->accept();
}


void srtWidget::mouseMoveEvent(QMouseEvent *event )
{
  event->accept();

  int deltaX = originalXPos - event->globalX();
  int deltaY = originalYPos - event->globalY();

  originalXPos = event->globalX();
  originalYPos = event->globalY();

  if ((deltaX == 0) && (deltaY == 0))
    return;

  // Now rotate
  angle = -0.3*sqrt(deltaX*deltaX + deltaY*deltaY);
  axis  = deltaX*scene->camera.upwardDirection() + deltaY*scene->camera.rightDirection();
  int elapsed = stopWatch.restart();
  if (elapsed > 0) {
    rotationalSpeed = angle/elapsed;
  } else
    rotationalSpeed = angle;

  if (scene)
    scene->camera.rotateCamera( QQuaternion::fromAxisAndAngle(axis, angle) );
}


void srtWidget::mouseReleaseEvent(QMouseEvent *event )
{
  event->accept();

  // Start rotation if the last mouse move is no longer than 100msec ago.
  if (stopWatch.elapsed() < 100) {
    stopWatch.start();
    rotationTimer.start(50);
  }
}


void srtWidget::rotate()
{
  angle = stopWatch.restart()*rotationalSpeed;

  if (scene)
    scene->camera.rotateCamera( QQuaternion::fromAxisAndAngle(axis, angle) );
}

#warning set minimum size (100x100)

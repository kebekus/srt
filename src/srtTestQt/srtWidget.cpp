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
  : QFrame(parent), _rotationTimer(this)
{
  // Initialize Members
  _manipulationEnabled = true;
  originalXPos = 0;
  originalYPos = 0;
  _manipulationAngle = 0.0;
  _manipulationAxis = QVector3D(0,1,0);
  _manipulationRotationalSpeed = 0.0;

  _rotationAxis = QVector3D(0,1,0);
  _rotationSpeed = 0.0;

  connect(&_rotationTimer, SIGNAL(timeout()), this, SLOT(performRotation()));

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


void srtWidget::setRotationAxis(QVector3D axis)
{
  if (qFuzzyCompare(axis, QVector3D()))
    return;

  _rotationAxis = axis.normalized();
}


void srtWidget::setRotation(bool rotate)
{
  if (rotate)
    _rotationTimer.start(50);
  else
    _rotationTimer.stop();
}


void srtWidget::mousePressEvent(QMouseEvent *event )
{
  if ((!_manipulationEnabled) || (event->button() != Qt::LeftButton)) {
    event->ignore();
    return;
  }
  event->accept();

  // Stop any rotation movement
  setRotation(false);

  // Record current mouse position, start timer in order to record mouse speed
  stopWatch.start();
  originalXPos    = event->globalX();
  originalYPos    = event->globalY();
}


void srtWidget::mouseMoveEvent(QMouseEvent *event )
{
  if (!_manipulationEnabled) {
    event->ignore();
    return;
  }
  event->accept();


  // Compute axis and rotational speed, reset timer to record mouse speed when
  // the next event takes place
  int deltaX = originalXPos - event->globalX();
  int deltaY = originalYPos - event->globalY();
  originalXPos = event->globalX();
  originalYPos = event->globalY();
  int elapsed = stopWatch.restart();
  _manipulationAngle = -0.3*sqrt(deltaX*deltaX + deltaY*deltaY);
  _manipulationAxis  = deltaX*scene->camera.upwardDirection() + deltaY*scene->camera.rightDirection();
  _manipulationRotationalSpeed = (elapsed > 0) ? _manipulationAngle/elapsed : _manipulationAngle;

  // Now rotate
  if ((scene) && (_manipulationAngle != 0))
    scene->camera.rotateCamera( QQuaternion::fromAxisAndAngle(_manipulationAxis, _manipulationAngle) );
}


void srtWidget::mouseReleaseEvent(QMouseEvent *event )
{
  if (!_manipulationEnabled) {
    event->ignore();
    return;
  }
  event->accept();

  // Start rotation if the last mouse move is no longer than 100msec ago.
  if (stopWatch.elapsed() > 100)
    return;

  setRotationAxis(_manipulationAxis);
  setRotationSpeed(_manipulationRotationalSpeed);
  setRotation(true);
}


void srtWidget::performRotation()
{
  qreal angle = stopWatch.restart()*_rotationSpeed;

  if (scene)
    scene->camera.rotateCamera( QQuaternion::fromAxisAndAngle(_rotationAxis, angle) );
}

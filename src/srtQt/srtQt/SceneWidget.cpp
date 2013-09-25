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

#include "srtQt/SceneWidget.h"


namespace srtQt {

SceneWidget::SceneWidget(QWidget *parent)
  : QFrame(parent), _rotationTimer(this)
{
  // Speed up painting. This widget is opaque, so no need to render a fancy
  // background which is never survive the day.
  setAttribute(Qt::WA_OpaquePaintEvent);

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


void SceneWidget::setScene(srtQt::Scene *_scene)
{
  scene = _scene;
  if (scene.isNull())
    return;

  connect(scene, SIGNAL(changed()), this, SLOT(update()));
}


bool SceneWidget::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture) {
    qDebug() << "A Gesture!  Hallelujah!";
    return false;
  }
  return QWidget::event(event);
}


void SceneWidget::paintEvent(QPaintEvent *event)
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

  /*
  QVector3D xAxis(100,0,0);
  QVector3D yAxis(0,100,0);
  QVector3D zAxis(0,0,100);
  
  QVector3D up   = scene->camera.upwardDirection();
  QVector3D right= scene->camera.rightDirection();

  QPointF xAxisTip(right.x(), up.x());
  QPointF yAxisTip(right.y(), up.y());
  QPointF zAxisTip(right.z(), up.z());

  QPoint origin(100,100);
  painter.setPen(Qt::blue);
  painter.drawLine( origin, origin+100*xAxisTip);
  painter.drawText( origin+110*xAxisTip, "x");
  painter.drawLine( origin, origin+100*yAxisTip);
  painter.drawText( origin+110*yAxisTip, "y");
  painter.drawLine( origin, origin+100*zAxisTip);
  painter.drawText( origin+110*zAxisTip, "z");
  */
  painter.end();
  QFrame::paintEvent(event);
}


void SceneWidget::setRotationAxis(QVector3D axis)
{
  if (qFuzzyCompare(axis, QVector3D()))
    return;

  _rotationAxis = axis.normalized();
}


void SceneWidget::setRotation(bool rotate)
{
  if (rotate) {
    stopWatch.start();
// TODO: maybe use smarter adaptive frame rate
    _rotationTimer.start(50);
  } else
    _rotationTimer.stop();
}


QVariant SceneWidget::settings() const
{
  QVariantMap map;

  map["manipulationEnabled"] = manipulationEnabled();
  map["rotationAxis"] = rotationAxis();
  map["rotationSpeed"] = rotationSpeed();
  map["rotation"] = rotation();
  
  return map;
}


void SceneWidget::load(QVariant variant)
{
  QVariantMap map = variant.value<QVariantMap>();

  if (map.contains("manipulationEnabled"))
    setManipulationEnabled(map["manipulationEnabled"].value<bool>());
  if (map.contains("rotationAxis"))
    setRotationAxis(map["rotationAxis"].value<QVector3D>());
  if (map.contains("rotationSpeed"))
    setRotationSpeed(map["rotationSpeed"].value<qreal>());
  if (map.contains("rotation"))
    setRotation(map["rotation"].value<bool>());

  return;
}


void SceneWidget::mousePressEvent(QMouseEvent *event )
{
  if ((!_manipulationEnabled) || (event->button() != Qt::LeftButton)) {
    event->ignore();
    return;
  }
  event->accept();

  // Stop any rotation movement
  setRotation(false);
  _manipulationRotationalSpeed = 0.0;

  // Record current mouse position, start timer in order to record mouse speed
  stopWatch.start();
  originalXPos    = event->globalX();
  originalYPos    = event->globalY();
}


void SceneWidget::mouseMoveEvent(QMouseEvent *event )
{
  if (!_manipulationEnabled || scene.isNull()) {
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
  _manipulationAngle = -0.2*sqrt(deltaX*deltaX + deltaY*deltaY);
  _manipulationAxis  = deltaX*scene->camera.upwardDirection() + deltaY*scene->camera.rightDirection();
  _manipulationRotationalSpeed = (elapsed > 0) ? _manipulationAngle/elapsed : _manipulationAngle;

  // Now rotate
  if ((scene) && (_manipulationAngle != 0))
    scene->camera.rotateAboutOrigin( QQuaternion::fromAxisAndAngle(_manipulationAxis, _manipulationAngle) );
}


void SceneWidget::mouseReleaseEvent(QMouseEvent *event )
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
  if (_manipulationRotationalSpeed != 0.0)
    setRotation(true);
}


void SceneWidget::wheelEvent(QWheelEvent * event )
{
  if (!_manipulationEnabled || scene.isNull()) {
    event->ignore();
    return;
  }
  event->accept();

  qreal zoom = scene->camera.zoom() * exp(event->delta() / (8.0*360.0));
  scene->camera.setZoom( zoom );
}


void SceneWidget::performRotation()
{
  qreal angle = stopWatch.restart()*_rotationSpeed;

  if (scene)
    scene->camera.rotateAboutOrigin( QQuaternion::fromAxisAndAngle(_rotationAxis, angle) );
}


} // namespace srtQt

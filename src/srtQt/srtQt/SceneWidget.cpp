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
#include "srtQt/private/CoordinateWidget.h"


namespace srtQt {

SceneWidget::SceneWidget(QWidget *parent)
  : QFrame(parent), animationTimer(this), _opacityAnimation(this, "coordsOpacity", this)
{
  coordWidget = new CoordinateWidget();
  coordWidget->show();

  // Speed up painting. This widget is opaque, so no need to render a fancy
  // background which will never survive the day.
  setAttribute(Qt::WA_OpaquePaintEvent);

  // Initialize Members
  _coordsOpacity = 0.0;
  _manipulationEnabled = true;
  originalXPos = 0;
  originalYPos = 0;
  _manipulationAngle = 0.0;
  _manipulationAxis = QVector3D(0,1,0);
  _manipulationRotationalSpeed = 0.0;

  _rotationAxis = QVector3D(0,1,0);
  _rotationSpeed = 0.0;

  connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animate()));

  // Test only
  grabGesture(Qt::TapGesture);
  grabGesture(Qt::TapAndHoldGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::PinchGesture);
  grabGesture(Qt::SwipeGesture);
}


SceneWidget::~SceneWidget()
{
  delete coordWidget;
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
  
  //  painter.fillRect( QRectF(frameRect().left()+frameWidth(), frameRect().top()+frameWidth(), frameRect().width()-2*frameWidth(), frameRect().height()-2*frameWidth()), Qt::black);

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


  if (_coordsOpacity != 0.0) {
    int w=(qMin(width(),height())-2*frameWidth())/4;
    int h=w;
    int x=width()-2*frameWidth()-w-w/5;
    int y=height()-2*frameWidth()-w-w/5;
    
    painter.setOpacity(_coordsOpacity);
    coordWidget->setAxis( -scene->camera.rightDirection(), -scene->camera.upwardDirection(), scene->camera.viewDirection());
    QPixmap pix = coordWidget->renderPixmap(w,h);
    painter.drawPixmap(x,y,pix);
  }

  painter.end();
  QFrame::paintEvent(event);
}

  
void SceneWidget::setCoordsOpacity(qreal coordsOpacity)
{
  _coordsOpacity = qBound(0.0, coordsOpacity, 1.0);
  update();
}


void SceneWidget::setRotationAxis(QVector3D axis)
{
  if (qFuzzyCompare(axis, QVector3D()))
    return;

  _rotationAxis = axis.normalized();
}


void SceneWidget::setRotation(bool rotate)
{
  animate_rotation = rotate;

  if (rotate) {
    stopWatch.start();
    animationTimer.start(0);
  } else
    animationTimer.stop();
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

  // start opacity animation: quickly fade in
  _opacityAnimation.stop();
  _opacityAnimation.setDuration(200);
  _opacityAnimation.setStartValue(_coordsOpacity);
  _opacityAnimation.setEndValue(0.8);
  _opacityAnimation.start();
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

  // start opacity animation: fade out
  _opacityAnimation.stop();
  _opacityAnimation.setDuration(800);
  _opacityAnimation.setStartValue(_coordsOpacity);
  _opacityAnimation.setEndValue(0.0);
  _opacityAnimation.start();


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


void SceneWidget::animate()
{
  if (animate_rotation && scene) {
    qreal angle = stopWatch.restart()*_rotationSpeed;
    scene->camera.rotateAboutOrigin( QQuaternion::fromAxisAndAngle(_rotationAxis, angle) );
  }

}


} // namespace srtQt

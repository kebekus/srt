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

#ifndef SRTWIDGET
#define SRTWIDGET

#include <QFrame>
#include <QPointer>
#include <QTime>
#include <QTimer>

#include "srtScene.h"

class srtWidget : public QFrame
{
  Q_OBJECT

  Q_PROPERTY(bool manipulationEnabled READ manipulationEnabled WRITE setManipulationEnabled);
  Q_PROPERTY(QVector3D rotationAxis READ rotationAxis WRITE setRotationAxis);
  Q_PROPERTY(qreal rotationSpeed READ rotationSpeed WRITE setRotationSpeed);
  Q_PROPERTY(bool rotation READ rotation WRITE setRotation);

 public:
  srtWidget(QWidget *parent = 0);

  ~srtWidget();

  void setScene(srtScene *_scene);

  bool manipulationEnabled() const { return _manipulationEnabled; }
  void setManipulationEnabled(bool enabled) {_manipulationEnabled = enabled; }

  QVector3D rotationAxis() const { return _rotationAxis; }
  void setRotationAxis(QVector3D axis);

  qreal rotationSpeed() const { return _rotationSpeed; }
  void setRotationSpeed(qreal speed) { _rotationSpeed = speed; }

  bool rotation() const { return _rotationTimer.isActive(); }
  void setRotation(bool rotate);

  // Re-implemented event handlers
  bool event(QEvent *event);
  void paintEvent (QPaintEvent * event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

 private slots:
  // Rotates about '_rotationAxis', with an angle determined by '_rotationSpeed'
  // and the elapsed time measured by '_rotationTimer'.
  void performRotation();

 private:
  QPointer<srtScene> scene;

  // Used in mouse manipulation
  bool      _manipulationEnabled;
  int       originalXPos, originalYPos;
  qreal     _manipulationAngle;
  QVector3D _manipulationAxis;
  qreal     _manipulationRotationalSpeed; // in angle/msec
  QTime     stopWatch;

  // Used in rotation
  QVector3D _rotationAxis;
  qreal     _rotationSpeed;
  QTimer    _rotationTimer;
};

#endif

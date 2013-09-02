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

#ifndef SRTCAMERA
#define SRTCAMERA

#include <QObject>
#include <QQuaternion>
#include <QVector3D>


#warning thread safety?

#warning This is a seriously flawed design. The user of libqsrt should never include this.
extern "C" {
#include "camera.h"
}

class srtCamera : public QObject
{
  Q_OBJECT;

 public:
  srtCamera(QObject *parent = 0);

  QVector3D position();
  QVector3D viewDirection();
  QVector3D upwardDirection();
  QVector3D rightDirection();

#warning serialisation and i/o missing
  struct camera _camera;

 public slots:
  void reset();

  void setPosition(QVector3D pos);
  void setView(QVector3D dir, QVector3D up);

  void translate(QVector3D displacement);
  void rotateView(QQuaternion rot);
  void rotateCamera(QQuaternion rot);

 signals:
  /**
   * \brief Emitted whenever any member of this class changes its value
   *
   * This signal is emitted whenever any member of this class changes its value
   */
  void changed();

 public:
  QVector3D _position;
  QVector3D _viewDirection;
  QVector3D _upwardDirection;

};

#endif

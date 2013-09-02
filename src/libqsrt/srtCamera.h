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
#include <QReadWriteLock>
#include <QVariant>
#include <QVector3D>


#warning documentation

namespace qsrt {

/**
 * \brief Camear object used to render a scene
 *
 * This class represents a camera in a given scene. The main properties stored
 * are the position of the camera and the direction of view.
 *
 * All methods of the class are reentrant and thread-safe.
 *
 * @author Stefan Kebekus 
 */


class Camera : public QObject
{
  Q_OBJECT;

 /**
  * \brief Position of the camera
  *
  * This vector specifies the position of the camera in space. The signal
  * changed() is emitted whenever this property changes.
  */
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY changed);

 /**
  * \brief Direction of view
  *
  * This vector specifies in which direction the camera is pointing. The signal
  * changed() is emitted whenever this property changes.
  *
  * @note This vector should always be normalized, and orthogonal to the
  * 'upwardDirection'. 
  */
  Q_PROPERTY(QVector3D viewDirection READ viewDirection WRITE setViewDirection NOTIFY changed);

 /**
  * \brief Direction of view
  *
  * This vector specifies in which direction the camera is pointing. The signal
  * changed() is emitted whenever this property changes.
  *
  * @note This vector should always be normalized, and orthogonal to the
  * 'upwardDirection'. 
  */
  Q_PROPERTY(QVector3D upwardDirection READ upwardDirection WRITE setUpwardDirection NOTIFY changed);

 public:
  Camera(QObject *parent = 0);

  QVector3D position();
  QVector3D viewDirection();
  QVector3D upwardDirection();
  QVector3D rightDirection();

  operator QVariant();
  bool load(QVariant var);

 public slots:
  void reset();

  void setPosition(QVector3D pos);
  void setViewDirection(QVector3D dir);
  void setUpwardDirection(QVector3D up);
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

 private:
  friend bool operator== (Camera& s1, Camera& s2);
  friend bool operator!= (Camera& s1, Camera& s2);

  // Read-Write lock, to be used for all private members that are defined below
  // this point
  QReadWriteLock privateMemberLock;

  QVector3D _position;
  QVector3D _viewDirection;
  QVector3D _upwardDirection;
};

QDataStream & operator<< (QDataStream& out, Camera& camera);
QDataStream & operator>> (QDataStream& in, Camera& camera);

/**
 * \brief Check two cameras for equality
 *
 * Two cameras are considered equal if all their properties agree.
 *
 * @returns true on equality
 */
bool operator== (Camera& s1, Camera& s2);

/**
 * \brief Check two cameras for inequality
 *
 * Two cameras are considered unequal if one property disagrees.
 *
 * @returns true on inequality
 */
bool operator!= (Camera& s1, Camera& s2);


} // namespace qsrt


#endif

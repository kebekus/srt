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
#include <QMap>
#include <QtGlobal>

extern "C" {
#include "camera.h"
}
#include "srtCamera.h"

namespace qsrt {

const qreal Camera::minZoom = 0.01;
const qreal Camera::maxZoom = 100.0;


Camera::Camera(QObject *parent)
  : QObject(parent)
{
  reset();
}


QVector3D Camera::position()
{
  QReadLocker locker(&privateMemberLock);
  return _position;
}


QVector3D Camera::viewDirection()
{
  QReadLocker locker(&privateMemberLock);
  return _viewDirection;
}


QVector3D Camera::upwardDirection()
{
  QReadLocker locker(&privateMemberLock);
  return _upwardDirection;
}


QVector3D Camera::rightDirection()
{
  QReadLocker locker(&privateMemberLock);
  return QVector3D::crossProduct(_upwardDirection,_viewDirection);
}


qreal Camera::zoom()
{
  QReadLocker locker(&privateMemberLock);
  return _zoom;
}


Camera::operator QVariant()
{
  QReadLocker locker(&privateMemberLock);

  QVariantMap map;
  map["position"]        = _position;
  map["viewDirection"]   = _viewDirection;
  map["upwardDirection"] = _upwardDirection;
  map["zoom"]            = _zoom;

  return map;
}


bool Camera::load(QVariant variant)
{
  QVariantMap map = variant.value<QVariantMap>();

  // Check existence of the most important members
  if ( !map.contains("position") || !map.contains("viewDirection") || !map.contains("upwardDirection") || !map.contains("zoom") )
    return false;

  QVector3D n_position        = map["position"].value<QVector3D>();
  QVector3D n_viewDirection   = map["viewDirection"].value<QVector3D>();
  QVector3D n_upwardDirection = map["upwardDirection"].value<QVector3D>();
  qreal     n_zoom            = map["zoom"].value<qreal>();

  QWriteLocker locker(&privateMemberLock);
  _position        = n_position;
  _viewDirection   = n_viewDirection;
  _upwardDirection = n_upwardDirection;
  _zoom            = n_zoom;

  _rectifyMembers();

  return true;
}


void Camera::reset()
{
  QVector3D n_position        = QVector3D(0,0,10);
  QVector3D n_viewDirection   = QVector3D(0,0,-1);
  QVector3D n_upwardDirection = QVector3D(0,1,0);
  qreal     n_zoom            = 1.0;

  if ( (n_position == _position) && (n_viewDirection == _viewDirection) && (n_upwardDirection == _upwardDirection) && (n_zoom == _zoom) )
    return;

  privateMemberLock.lockForWrite();
  _position        = n_position;
  _viewDirection   = n_viewDirection;
  _upwardDirection = n_upwardDirection;
  _zoom            = n_zoom;
  privateMemberLock.unlock();

  emit changed();
}


void Camera::setPosition(QVector3D pos)
{
  if (pos == _position)
    return;

  privateMemberLock.lockForWrite();
  _position = pos;
  privateMemberLock.unlock();

  emit changed();
}
  
  
void Camera::setViewDirection(QVector3D dir)
{
  // Paranoid safety checks
  if (qFuzzyCompare(dir, QVector3D()))
    return;
  if (qFuzzyCompare(dir, _viewDirection))
    return;

  // Adjust members
  privateMemberLock.lockForWrite();
  _viewDirection   = dir;
  _rectifyMembers();
  privateMemberLock.unlock();

  // Emit signal 'changed'
  emit changed();
}


void Camera::setUpwardDirection(QVector3D up)
{
  // Paranoid safety checks
  if (qFuzzyCompare(up, QVector3D()))
    return;
  if (qFuzzyCompare(up, _upwardDirection))
    return;

  // Adjust members
  privateMemberLock.lockForWrite();
  _upwardDirection = up;
  _rectifyMembers();
  privateMemberLock.unlock();

  // Emit signal 'changed'
  emit changed();
}


void Camera::setView(QVector3D dir, QVector3D up)
{
  if (qFuzzyCompare(QVector3D::crossProduct(dir, up), QVector3D()))
    return;

  QVector3D n_viewDirection   = dir.normalized();
  QVector3D n_upwardDirection = (up - QVector3D::dotProduct(n_viewDirection, up)*n_viewDirection).normalized();

  if ( (n_viewDirection == _viewDirection) && (n_upwardDirection == _upwardDirection) )
    return;

  privateMemberLock.lockForWrite();
  _viewDirection   = n_viewDirection;
  _upwardDirection = n_upwardDirection;
  _rectifyMembers();
  privateMemberLock.unlock();

  emit changed();
}


void Camera::setZoom(qreal zoom)
{
  if (zoom == _zoom)
    return;

  privateMemberLock.lockForWrite();
  _zoom = zoom;
  _rectifyMembers();
  privateMemberLock.unlock();

  emit changed();
}


void Camera::translate(QVector3D displacement)
{
  if (qFuzzyCompare(displacement, QVector3D()))
    return;

  privateMemberLock.lockForWrite();
  _position += displacement;
  privateMemberLock.unlock();

  emit changed();
}


void Camera::rotateInPlace(QQuaternion rot)
{
  if (rot.isIdentity())
    return;

  privateMemberLock.lockForWrite();
  _viewDirection   = rot.rotatedVector(_viewDirection);
  _upwardDirection = rot.rotatedVector(_upwardDirection);
  privateMemberLock.unlock();

  emit changed();
}


void Camera::rotateAboutOrigin(QQuaternion rot)
{
  if (rot.isIdentity())
    return;

  privateMemberLock.lockForWrite();
  _position        = rot.rotatedVector(_position);
  _viewDirection   = rot.rotatedVector(_viewDirection);
  _upwardDirection = rot.rotatedVector(_upwardDirection);
  privateMemberLock.unlock();

  emit changed();
}


bool Camera::_rectifyMembers()
{
  bool changed = false;

  // Make sure that zoom is in the limit.
  if ((_zoom < minZoom) || (_zoom > maxZoom)) {
    changed = true;
    _zoom = qBound(minZoom, _zoom, maxZoom);
  }

  // Make sure that viewDirection is not empty
  if (fabs(_viewDirection.length()) < 1e-4) {
    changed = true;
    _viewDirection = QVector3D(0,0,-1);
  }

  // Make sure that viewDirection is normalized
  if (fabs(_viewDirection.length())-1.0 > 1e-4) {
    changed = true;
    _viewDirection.normalize();
  }

  // Make sure that {viewDirection, upwardDirection} is linearly independent, by
  // adjusting upwardDirection if need be, the result is stored in
  // n_upwardDirection
  QVector3D n_upwardDirection = _upwardDirection;
  if (QVector3D::crossProduct(_viewDirection, n_upwardDirection).length() < 1e-4)
    n_upwardDirection = QVector3D(1,0,0);
  if (QVector3D::crossProduct(_viewDirection, n_upwardDirection).length() < 1e-4)
    n_upwardDirection = QVector3D(0,1,0);

  // Make sure that viewDirection and upwardDirection are perpendicular, by
  // adding a suitable multiple of viewDirection to the viewDirection if need be
  n_upwardDirection = (n_upwardDirection - QVector3D::dotProduct(_viewDirection, n_upwardDirection)*_viewDirection).normalized();

  // Update the upwardDirection if the thing really changed
  if ( (n_upwardDirection - _upwardDirection).length() > 1e-4 ) {
    changed = true;
    _upwardDirection = n_upwardDirection;
  }
  
  return changed;
}


QDataStream & operator<< (QDataStream& out, Camera& camera)
{
  out << QVariant(camera);
  return out;
}


QDataStream & operator>> (QDataStream& in, Camera& camera)
{
  QVariant var;
  in >> var;
  camera.load(var);
  return in;
}


bool operator== (Camera& s1, Camera& s2)
{
  // Get read access to private members
  QReadLocker privatMemberLocker1(&s1.privateMemberLock);
  QReadLocker privatMemberLocker2(&s2.privateMemberLock);
  
  if (s1._position != s2._position)
    return false;
  if (s1._viewDirection != s2._viewDirection)
    return false;
  if (s1._upwardDirection != s2._upwardDirection)
    return false;

  return true;
}


bool operator!= (Camera& s1, Camera& s2)
{
  return !(s1 == s2);
}


} // end of namespace qsrt

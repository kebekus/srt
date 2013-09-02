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

#include "srtCamera.h"


srtCamera::srtCamera(QObject *parent)
  : QObject(parent)
{
  reset();
}


QVector3D srtCamera::position()
{
  return _position;
}


QVector3D srtCamera::viewDirection()
{
  return _viewDirection;
}


QVector3D srtCamera::upwardDirection()
{
  return _upwardDirection;
}


void srtCamera::reset()
{
  QVector3D n_position        = QVector3D(0,0,10);
  QVector3D n_viewDirection   = QVector3D(0,0,-1);
  QVector3D n_upwardDirection = QVector3D(0,1,0);

  if ( (n_position == _position) && (n_viewDirection == _viewDirection) && (n_upwardDirection == _upwardDirection) )
    return;

  _position        = n_position;
  _viewDirection   =  n_viewDirection;
  _upwardDirection =  n_upwardDirection;

  emit changed();
}


void srtCamera::setPosition(QVector3D pos)
{
  if (pos == _position)
    return;

  _position = pos;
  emit changed();
}


void srtCamera::setView(QVector3D dir, QVector3D up)
{
  if (qFuzzyCompare(QVector3D::crossProduct(dir, up), QVector3D()))
    return;

  QVector3D n_viewDirection   = dir.normalized();
  QVector3D n_upwardDirection = up - QVector3D::dotProduct(n_viewDirection, up)*n_viewDirection;

  if ( (n_viewDirection == _viewDirection) && (n_upwardDirection == _upwardDirection) )
    return;

  _viewDirection   = n_viewDirection;
  _upwardDirection = n_upwardDirection;
  emit changed();
}


void srtCamera::translate(QVector3D displacement)
{
  if (qFuzzyCompare(displacement, QVector3D()))
    return;

  _position += displacement;
  emit changed();
}

void srtCamera::rotateView(QQuaternion rot)
{
  if (rot.isIdentity())
    return;

  _viewDirection   = rot.rotatedVector(_viewDirection);
  _upwardDirection = rot.rotatedVector(_upwardDirection);
  emit changed();
}


void srtCamera::rotateCamera(QQuaternion rot)
{
  if (rot.isIdentity())
    return;

  _position        = rot.rotatedVector(_position);
  _viewDirection   = rot.rotatedVector(_viewDirection);
  _upwardDirection = rot.rotatedVector(_upwardDirection);
  emit changed();
}

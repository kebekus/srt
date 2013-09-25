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


namespace srtQt {

/**
 * \brief Represents a camera in a given scene
 *
 * This class represents a camera in a given scene. The main properties stored
 * are the position of the camera and the direction of view.
 *
 * All methods of the class are reentrant and thread-safe. This class is
 * contained in the libraries 'libsrt-qt4core' and 'libsrt-qt5core'.
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
  * 'upwardDirection'. See the documentation of the setter method to learn how
  * this is ensured.
  */
  Q_PROPERTY(QVector3D viewDirection READ viewDirection WRITE setViewDirection NOTIFY changed);

 /**
  * \brief Upward direction of view
  *
  * This vector specifies in which direction the camera is pointing. The signal
  * changed() is emitted whenever this property changes.
  *
  * @note This vector should always be normalized, and orthogonal to the
  * 'viewDirection'.  See the documentation of the setter method to learn how
  * this is ensured.
  */
  Q_PROPERTY(QVector3D upwardDirection READ upwardDirection WRITE setUpwardDirection NOTIFY changed);

 /**
  * \brief Zoom factor
  *
  * This number specifies the zoom factor, which must be in the range [0.01,
  * 100.0]. Values larger than one correspond to magnification. The signal
  * changed() is emitted whenever this property changes.
  */
  Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY changed);

 public:
 /**
  * \brief Standard constructor
  *
  * Constructs a camera object and initializes all properties using the reset() method
  *
  * @param parent Standard argument for the QObject constructor
  */
  Camera(QObject *parent = 0);

  /**
   * \brief Getter method for the position property
   */
  QVector3D position();

  /**
   * \brief Getter method for the viewDirection property
   *
   * @returns The direction in which the camera points. The vector returned is
   * normalized, and orthogonal to the upwardDirection.
   */
  QVector3D viewDirection();

  /**
   * \brief Getter method for the upwardDirection property
   *
   * @returns The direction which is 'upward' in the picutre. The vector
   * returned is normalized and orthogonal to the viewDirection.
   */
  QVector3D upwardDirection();

  /**
   * \brief Compute direction with is 'right' in the picture
   * 
   * This method is a shorthand for the cross-product of the upwardDirection and
   * the viewDirection.
   *
   * @returns The direction which is 'upward' in the picutre. The vector
   * returned is normalized, and orthogonal to both the viewDirection and the
   * upwardDirection.
   */
  QVector3D rightDirection();

  /**
   * \brief Getter method for the zoom property
   *
   * @returns The current zoom factor
   */
  qreal zoom();

  /**
   * \brief Write properties into a QVariant object
   *
   * This method writes all properties into a QVariant object. The data can be
   * loaded back into a camera object using the method load().
   *
   * @see load()
   */
  operator QVariant();

  /**
   * \brief Restore properties
   * 
   * This method reads previously saved camera properties and sets these
   * properties in the present object.  The signal changed() will not be
   * emitted.  On error, the surface is remains unchanged.
   *
   * @param variant A QVariant, as produced by the operator QVariant().
   *
   * @returns If the properties could not be read from the QVariant object,
   * 'false' is returned. Otherwise 'true'.
   */
  bool load(QVariant variant);

 public slots:
  /**
   * \brief Sets all properties to default values
   *
   * This method sets the following values
   *
   * * position is set to QVector3D(0,0,10)
   *
   * * viewDirection is set to QVector3D(0,0,-1)
   *
   * * upwardDirection is set to QVector3D(0,1,0)
   *
   * * zoom is set to 1.0
   *
   * The signal changed() is emitted if appropriate.
   */
  void reset();

  /**
   * \brief Setter method for the 'position' property
   *
   * The signal changed() is emitted if appropriate.
   */
  void setPosition(QVector3D pos);

  /**
   * \brief Setter method for the 'viewDirection' property
   *
   * This method sets the 'viewDirection' property. To ensure that the
   * 'viewDirection' and the 'upwardDirection' are orthonormal, the following
   * steps are taken.
   *
   * * The vector 'dir' is normalized. If 'dir' is the zero vector, or very
   *   close to the zero vector, then the method returns immediately without
   *   changing anything.
   *
   * * If 'upwardDirection' and 'dir' are linearly independent, then
   *   'upwardDirection' is corrected by adding a suitable multiple of
   *   'dir'. The corrected upwardDirection will then be orthogonal to 'dir',
   *   and is finally normalized.
   *
   * * If 'upwardDirection' and 'dir' are linearly independent, then
   *   'upwardDirection' is replaced by a random unit vector which is orthogonal
   *   to dir.
   *
   * This procedure ensures that the upwardDirection moves continuously along
   * with the viewDirection, if the viewDirection is changed only by small
   * increments.
   *
   * @param viewDir New view direction of the camera.
   *
   * The signal changed() is emitted if appropriate.
   */
  void setViewDirection(QVector3D viewDir);

  /**
   * \brief Setter method for the 'upwardDirection' property
   *
   * This method sets the 'upwardDirection' property. To ensure that the
   * 'viewDirection' and the 'upwardDirection' are orthonormal, the
   * steps outlined in the description of setViewDirection() are taken.
   *
   * @param upDir New upward direction of the camera.
   *
   * The signal changed() is emitted if appropriate.
   */
  void setUpwardDirection(QVector3D upDir);

  /**
   * \brief Setter method for the 'viewDirection' and 'upwardDirection' property
   *
   * This method sets the 'viewDirection' and 'upwardDirection' property
   * simultaneously. To ensure that the 'viewDirection' and the
   * 'upwardDirection' are orthonormal, the steps outlined in the description of
   * setViewDirection() are taken.
   *
   * @param viewDir New view direction of the camera.
   *
   * @param upDir New upward direction of the camera.
   *
   * The signal changed() is emitted if appropriate.
   */
  void setView(QVector3D viewDir, QVector3D upDir);

  /**
   * \brief Setter method for the 'zoom' property
   *
   * This method sets the 'zoom' property. To ensure that the 'viewDirection'
   * and the 'upwardDirection' are orthonormal, the steps outlined in the
   * description of setViewDirection() are taken.
   *
   * @param zoom New zoom factor of the camera.
   *
   * The signal changed() is emitted if appropriate.
   */
  void setZoom(qreal zoom);

  /**
   * \brief Moves the camera without changing the view direction
   *
   * @param displacement Vector that is added to the 'position' vector.
   *
   * The signal changed() is emitted if appropriate.
   */
  void translate(QVector3D displacement);

  /**
   * \brief Rotates the camera, but does not change the camera's position
   *
   * @param rotation Quaternion that describes rotation axis and -angle.
   *
   * The signal changed() is emitted if appropriate.
   */
  void rotateInPlace(QQuaternion rotation);

  /**
   * \brief Rotates the camera and its position
   *
   * @param rotation Quaternion that describes rotation axis and -angle.
   *
   * The signal changed() is emitted if appropriate.
   */
  void rotateAboutOrigin(QQuaternion rotation);

 signals:
  /**
   * \brief Emitted whenever any member of this class changes its value
   *
   * This signal is emitted whenever any member of this class changes its value
   */
  void changed();

 private:
  static const qreal minZoom;
  static const qreal maxZoom;
  bool _rectifyMembers();

  friend QDataStream & operator<< (QDataStream& out, Camera& camera);
  friend QDataStream & operator>> (QDataStream& in, Camera& camera);
  friend bool operator== (Camera& s1, Camera& s2);
  friend bool operator!= (Camera& s1, Camera& s2);

  // Read-Write lock, to be used for all private members that are defined below
  // this point
  QReadWriteLock privateMemberLock;

  QVector3D _position;
  QVector3D _viewDirection;
  QVector3D _upwardDirection;
  qreal     _zoom;
};

/**
 * \brief Write properties into a QDataStream
 *
 * This method writes all properties of the surface into a QDataStream. The data
 * can be loaded back into a surface object using the operator >>.
 */
QDataStream & operator<< (QDataStream& out, Camera& camera);

/**
 * \brief Read properties from a QDataStream
 * 
 * This method reads previously saved camera properties and sets these
 * properties in the present object. The signal changed() will not be emitted,
 * nor will any other signal.
 *
 * On error, the camera is not changed.
 */
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


} // namespace srtQt


#endif

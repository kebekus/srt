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

namespace qsrt {

/**
 * \brief Displays a scene and allows the user to manipulate the camera position
 * with the mouse or touchpad
 *
 * \image html srtSceneWidget.png
 *
 * This widget displays a scene. If 'manipulationEnabled' is set to 'true', the
 * following mouse gestures can be used to manipulate the scenes' camera object.
 *
 * * By moving the mouse while holding the left mouse button, the user can
 *   rotate the camera about the origin, in order to view the scene from
 *   different angles.
 *
 * * By releasing the left mouse button while moving the mouse, the user starts
 *   an animation where the camera rotates about the origin. A left click will
 *   stop the animation at any time. To avoid starting the animation, the user
 *   must stop the mouse movement before releasing the left mouse button.
 *
 * * Movement of the mouse wheel results in a change of the zoom factor.
 *
 * The methods of the class are reentrant, but not thread-safe. This class is
 * contained in the libraries 'libsrt-qt4widgets' and 'libsrt-qt5widgets'.
 *
 * @author Stefan Kebekus 
 */

class SceneWidget : public QFrame
{
  Q_OBJECT;

  /**
   * \brief Enable or disable camera manipulation with the mouse
   *
   * Set this property to 'true' to allow the user to manipulate the camera
   * position with the mouse. If set to 'false', all mouse gestures will be
   * ignored.
   */
  Q_PROPERTY(bool manipulationEnabled READ manipulationEnabled WRITE setManipulationEnabled);

  /**
   * \brief Run or stop animation
   *
   * If set to 'true', the widget will display an animation where the camera
   * rotates about the axis specified in 'rotationAxis', with an angular speed
   * specified in 'rotationSpeed'. If 'manipulationEnabled' is set to 'true',
   * the user can start and stop the animation with mouse gestures. This
   * property will be updated accordingly.
   */
  Q_PROPERTY(bool rotation READ rotation WRITE setRotation);

  /**
   * \brief Rotation axis for the rotation animation
   *
   * This property holds the axis about which the camera is rotated when
   * 'rotation' is set to 'true'. If 'manipulationEnabled' is set to 'true', the
   * user modify this property with mouse gestures.
   */
  Q_PROPERTY(QVector3D rotationAxis READ rotationAxis WRITE setRotationAxis);

  /**
   * \brief Angular speed for the rotation animation
   *
   * This property holds the angular speed with which the camera is rotated when
   * 'rotation' is set to 'true'. If 'manipulationEnabled' is set to 'true', the
   * user modify this property with mouse gestures.
   */
  Q_PROPERTY(qreal rotationSpeed READ rotationSpeed WRITE setRotationSpeed);

 public:
 /**
  * \brief Standard constructor
  *
  * Constructs a scene widget. The scene is later set using the method
  * setScene(),
  *
  * @param parent Standard argument for the QWidget constructor
  */
  SceneWidget(QWidget *parent = 0);

 /**
  * \brief Set scene to be display
  *
  * Use this method to specify the scene that will be shown. If the property
  * 'manipulationEnabled' is set to true, the user can modify the scene's camera
  * object using mouse gestures. The rotation animation is performed by
  * modifying the camera object at regular intervals. This should be taken into
  * consideration when showing a single scene in more than one SceneWidgets.
  *
  * The SceneWidget does not assume ownership of the scene. It is technically
  * possible to delete the scene while the widget displays it (the widget will
  * become black on the next update). It is, however, questionable if this is a
  * good idea.
  *
  * @param scenePtr Pointer to the scene that shall be displayed.
  */
  void setScene(qsrt::Scene *scenePtr);

  /**
   * \brief Getter method for the property 'manipulationEnabled'
   *
   * @returns 'true' if the user can modify the scene using mouse gestures
   */
  bool manipulationEnabled() const { return _manipulationEnabled; }

  /**
   * \brief Setter method for the property 'manipulationEnabled'
   *
   * @param enabled Set this to 'true' to allow the user to modify the scene
   * using mouse gestures, and to 'false' to disallow this.
   */
  void setManipulationEnabled(bool enabled) {_manipulationEnabled = enabled; }

  /**
   * \brief Getter method for the property 'rotationAxis'
   *
   * @returns The axis about which the camera will rotate when the property
   * 'rotation' is set to 'true'
   */
  QVector3D rotationAxis() const { return _rotationAxis; }

  /**
   * \brief Setter method for the property 'rotationAxis'
   *
   * @param axis A vector specifiying the rotation axis. The vector 'axis'
   * should be of positive length and must not be close to zero, otherwise the
   * method will exist without doing anything.
   */
  void setRotationAxis(QVector3D axis);

  /**
   * \brief Getter method for the property 'rotationSpeed'
   *
   * @returns The angular speed with which the camera will rotate when the
   * property 'rotation' is set to 'true'
   */
  qreal rotationSpeed() const { return _rotationSpeed; }

  /**
   * \brief Getter method for the property 'rotationSpeed'
   */
  void setRotationSpeed(qreal speed) { _rotationSpeed = speed; }

  /**
   * \brief Getter method for the property 'rotation'
   */
  bool rotation() const { return _rotationTimer.isActive(); }

  /**
   * \brief Getter method for the property 'rotation'
   */
  void setRotation(bool rotate);

  /**
   * \brief Write properties into a QVariant object
   *
   * This method writes all properties of the widget into a QVariant object. The
   * data can be loaded back into a camera object using the method load(). This
   * makes it easy to store the properts of your widget with QSettings.
   * 
   * @code
   * QSettings settings;
   * settings.setValue("mainWindow/sceneWidget", sceneWidgetPointer->settings() );
   * @endcode
   *
   * To restore the settings, you can use this.
   * 
   * @code 
   * QSettings settings;
   * sceneWidgetPointer->load(settings.value("mainWindow/sceneWidget"));
   * @endcode
   *
   * @see load()
   */
  QVariant settings() const;
  
  /**
   * \brief Restore properties
   * 
   * This method reads previously saved properties and sets these properties in
   * the present object.
   *
   * @param variant A QVariant, as produced by the settings().
   *
   * @see settings()
   */
  void load(QVariant variant);

 private slots:
  // Rotates about '_rotationAxis', with an angle determined by '_rotationSpeed'
  // and the elapsed time measured by '_rotationTimer'.
  void performRotation();

 private:
  // Re-implemented event handlers
  bool event(QEvent *event);
  void paintEvent (QPaintEvent * event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

  QPointer<qsrt::Scene> scene;

  // Used in mouse manipulation
  bool      _manipulationEnabled;
  int       originalXPos, originalYPos;
  qreal     _manipulationAngle;
  QVector3D _manipulationAxis;
  qreal     _manipulationRotationalSpeed; // in angle/msec
  QTime     stopWatch;

  // Used in the rotation animation
  QVector3D _rotationAxis;
  qreal     _rotationSpeed;
  QTimer    _rotationTimer;
};

} // namespace qsrt

#endif

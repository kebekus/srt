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

#ifndef SRTSCENE
#define SRTSCENE

#include <QImage>
#include <QObject>

#include "srtCamera.h"
#include "srtSurface.h"


namespace qsrt {

/**
 * \brief Compound of camera object, algebraic surface and other data required to render a picture
 *
 * A scene consists of a camera object and an algebraic surface, and thus holds
 * all data required to create an image. Scenes can be rendered into a QImage
 * using the draw() method. They can conveniently be stored in QVariant objects
 * and hence further serialized. If any member of the scene changes in any way,
 * the signal changed() will be emitted.
 *
 * All methods of the class are reentrant and thread-safe.
 */

class Scene : public QObject
{
  Q_OBJECT

 public:
 /**
  * \brief Standard constructor
  *
  * Constructs a scene object
  *
  * @param parent Standard argument for the QObject constructor
  */
  Scene(QObject *parent=0);
 
  /**
   * \brief Render scene
   *
   * This method renders the scene. Since rendering is compute-intense, this
   * method may be slow.
   */
  QImage draw(QSize size);

  /**
   * \brief Write properties into a QVariant object
   *
   * This method writes all properties into a QVariant object. The data can be
   * loaded back into a camera object using the method load(). This allows to
   * conveniently store a surface into an application's settings.
   *
   * @code
   * // Save scene
   * QSettings settings;
   * settings.setValue("mainWindow/scene", scene);
   * ...
   * @endcode
   * @code
   * // Restore camera
   * QSettings settings;
   * if (scene.load(settings.value("mainWindow/scene")))
   *   ..success..
   * else
   *   ..no success..
   * ...
   * @endcode
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

  /**
   * \brief Camera object used the render the image
   *
   * Users of the scen can access this member freely. The signal changed() will
   * be emitted whenever this object changes.
   */
  qsrt::Camera camera;

  /**
   * \brief Surface that will be drawn
   *
   * Users of the scen can access this member freely. The signal changed() will
   * be emitted whenever this object changes.
   */
  qsrt::Surface surface;

 public slots:
  /**
   * \brief Resets all members to their default values
   *
   * This method calles camera.reset() and surface.reset().
   */
  void reset();

 signals:
  /**
   * \brief Emitted whenever any member of this class changes its value
   *
   * The signal is connected to the camera and the surfaces, so changes there
   * will also generate this signal.
   *
   * This signal is emitted whenever any member of this class changes its value
   */
  void changed();

 private:
#warning I do not know what that means
  int use_aabb;
};


} // namespace qsrt


#endif

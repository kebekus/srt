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
 
 public:
  QImage draw(QSize size);

  qsrt::Camera camera;
  qsrt::Surface surface;

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

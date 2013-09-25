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

#include <QFile>
#include "srtQt/Scene.h"

int main(int argc, char **argv)
{
  if (argc == 1)
    return -1;

  // Construct a scene object. By default, the camera is placed in the z-axis
  // and pointed towards the origin.
  srtQt::Scene scene;

  // Read scene
  QString magicID;
  QVariantMap map;
  QFile file(argv[1]);
  file.open(QIODevice::ReadOnly);
  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_4_8);
  in >> magicID;
  in >> map;
  file.close();
  scene.load(map["scene"]);


  // Render the surface into a QImage of size 128500x500
  QImage img = scene.draw( QSize(128,128) );
  // Save the image file.
  img.save("logo128.png");

  return 0;
}

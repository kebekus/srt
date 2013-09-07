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

#include "srtScene.h"

int main(int argc, char **argv)
{
  // Construct a scene object. By default, the camera is placed in the z-axis
  // and pointed towards the origin.
  qsrt::Scene scene;

  // Give the equation of the surface that we would like to draw. In the
  // example, the Clebsch Cubic Surface has been chosen, which is a nice-looking
  // surface that admits a rotational symmetry of order three.
  scene.surface.setEquation("81*(x^3+y^3+z^3) - 189*(x^2*y+x^2*z+y^2*x+y^2*z+z^2*x+z^2*y) + 54*(x*y*z) + 126*(x*y+x*z+y*z) - 9*(x^2+y^2+z^2) - 9*(x+y+z) + 1");

  // Render the surface into a QImage of size 500x500
  QImage img = scene.draw( QSize(500,500) );

  // Save the image file.
  img.save("test.png");

  return 0;
}

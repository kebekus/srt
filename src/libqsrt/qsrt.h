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

/**
   
   \mainpage

   \tableofcontents
   
   \section S1 Introduction
   
   This software package provides a C++/Qt interface to the surface rendering
   library libsrt. It allows to create photo-realistic images of algebraic
   surfaces in three-dimensional space easily and with minimal programming
   effort. Currently, the package contains the following pieces of software.

   - The library 'libqsrt' contains the Qt bindings for the srt library. It
     makes use of the vector extensions available in modern processors. It
     automatically detects and uses multi-core processors and scales well with
     the number of processor cores available. Surface equations are just-in-time
     compiled to optimized machine code, in order to speed up the rendering
     process as much as possible.

   - A Qt Designer plugin allows to easily construct user interfaces containing
     SceneWidgets in Qt Designer.

   \subsection S1a Two minimal surface rendering applications
   
   The API is quite simple and exposes only very few classes to the user. The
   following two examples show how they can be used.

   \subsubsection S1a1 A command line application

   A minimal surface rendering application requires only 4-5 lines of code and
   might look like this. The program computes an image of the Clebsch cubic
   surface and writes it to a graphics file "test.png".

   @code
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
   @endcode

   A more sophisticated program might place the camera at a locaction where is
   has a better view of the surface, set the zoom to highlight certain details,
   etc etc.

   \subsubsection S1a2 An application with a graphical user interface

   Is not much longer or more complicated to write a  minimal surface rendering application with a graphical user interface. The following program  shows an image of the Clebsch
   cubic surface and allows the user to rotate around the surface with the mouse.

   @code
#include <QApplication>
#include "srtSceneWidget.h"

int main(int argc, char **argv)
{
  // Set up the QApplication object, which we use later to start the event loop
  QApplication app(argc, argv);

  // Construct a scene object. By default, the camera is placed in the z-axis
  // and pointed towards the origin.
  qsrt::Scene scene;

  // Give the equation of the surface that we would like to draw. In the
  // example, the Clebsch Cubic Surface has been chosen, which is a nice-looking
  // surface that admits a rotational symmetry of order three.
  scene.surface.setEquation("81*(x^3+y^3+z^3) - 189*(x^2*y+x^2*z+y^2*x+y^2*z+z^2*x+z^2*y) + 54*(x*y*z) + 126*(x*y+x*z+y*z) - 9*(x^2+y^2+z^2) - 9*(x+y+z) + 1");

  // Set up a scene widget and show this widget
  qsrt::SceneWidget widget;
  widget.setScene(&scene);
  widget.show();

  // Start the event loop
  return app.exec();
}
   @endcode

   The resulting program will look like this.

   \image html minimalExampleGUI.png


   \subsection S1b The Qt Designer Plugin

   The software package includes a plugin for the Qt Designer. This allows to use Qt Designer to add SceneWidgets to the graphical user interface of your application, just as you would add any other widget. To make matters easier for the person who designs the GUI, the widgets display an image of the Clebsch Cubic Surface while they are shown in inside the Qt Designer. Once running in your application, the widgets will show whatever surfaces their scenes contain.

   \image html srtSceneWidgetPlugin.png

   @note Qt Designer plugins are rather fragile to build. The plugins will only work if the following conditions are satisfied.
     - If Qt Designer has been build in 'release' mode, the plugin also needs to be build in 'release' mode. Ditto with 'debug' mode. If Qt Designer and the plugin were built with different modes, the plugin will be ignored.
     - The plugin needs to be installed in Qt's plugin directory. If the plugin is installed anywhere else, it will most likely be ignored by Qt Designer. Using 'make install' will install the plugin in the proper location.
     - The libraries 'libsrt' and 'libqsrt' need to be available to the Qt Designer. For debugging purposes, the environment variable 'LD_LIBRARY_PATH' might be useful.


   \section S2 Using libqsrt in your application

   \subsection A Software requirements


   \subsection S2a Compilation with qmake


   \subsection S2b Compilation with cmake


   \subsection S2c Supporting the ssc file format

   The example programs included in this software package read and write scenes in a simple, but effective
   'srtScene' format, with file ending '.ssc'. We feel that this format might be
   useful elsewhere and encourage you to support it in your application. Files
   of this format are written using a QDataStream of version
   QDataStream::Qt_4_8, contain an identifier string, and a single QVariantMap
   object, which maps the key 'scene' to a QVariant where a Scene is stored. The
   QVariantMap can contain an arbitrary number of other entries which your
   application may either use or ignore.

   To write an SSC file, the following code can be used.
   @code
  qsrt::Scene scene;
  // ...fill the scene with lots of interesting data...

  QVariantMap map;
  map["scene"]  = scene;
  // ...add more entries to the map if required...

  QFile file(fileName);
  file.open(QIODevice::WriteOnly);
  QDataStream out(&file);
  out.setVersion(QDataStream::Qt_4_8);
  out << QString("srtScene");
  out << map;
  file.close();
   @endcode

   An SSC file can be read as follows.
   @code
  qsrt::Scene scene;

  QString magicID;
  QVariantMap map;

  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_4_8);
  in >> magicID;
  in >> map;
  file.close();

  // ...check for file errors...
  // ...check that magicID contains the string "srtScene"...
  // ...check that map.contains("scene") is true...
  scene.load(map["scene"]);
  // ...perhaps check the return value of scene.load()...
  // ...check map for more entries which may be useful...
@endcode

 */


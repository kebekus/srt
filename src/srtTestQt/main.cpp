
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QImage>

#include "srtScene.h"




int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  app.setApplicationName("srtTestQt");
  app.setOrganizationName("Albert-Ludwigs-Universität Freiburg");
  app.setOrganizationName("uni-freiburg.de");

  QElapsedTimer timer;
  timer.start();
  srtScene scene;

  // Create a surface and check if the surface could be generated without
  // errors. If an error occurred, write output to the command line.
  scene.surface.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  if (scene.surface.hasError()) {
    qDebug() << scene.surface.errorString() << endl
	     << "Index: " << scene.surface.errorIndex();
    exit(1);
  }
  
  QImage img;
  for(int i=0; i<100; i++)
    img = scene.draw( QSize(900, 900));
  qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";
  
  img.save("test.png");
  
  return 0;
}

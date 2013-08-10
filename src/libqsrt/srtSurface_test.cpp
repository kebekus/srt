#include <QtTest/QtTest>

#include "srtScene.h"
#include "srtSurface_test.h"


void srtSurface_test::benchmark()
{
  QElapsedTimer timer;

  timer.start();
  srtSurface s1;
  qDebug() << "Creation of one empty surface object took" << timer.elapsed() << "milliseconds";

  timer.start();
  srtSurface s("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  qDebug() << "Creation of one non-empty surface object took" << timer.elapsed() << "milliseconds";

  timer.start();
  for(int i=0; i<5; i++) {
    s.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2+0");
    s.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  }
  qDebug() << "JIT compilation of ten equations took" << timer.elapsed() << "milliseconds";
}


void srtSurface_test::specialStates()
{
  srtScene scene;
  QVERIFY( scene.surface.isEmpty() );
  QSize size(50, 50);
  QImage img1 = scene.draw( size );
  // Check that image is empty
  QVERIFY( img1.size() == size );
  for(int x=0; x<size.width(); x++)
    for(int y=0; y<size.height(); y++)
      QVERIFY( img1.pixel(x,y) == qRgb(0,0,0) );
  
  
  scene.surface.setEquation("ASDF");
  QVERIFY( scene.surface.hasError() );
  QImage img2 = scene.draw( size );
  // Check that image is null
  QVERIFY( img2.isNull() );
}


void srtSurface_test::constructor()
{
  srtSurface s1(this);
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );

  srtSurface s2("x^2+y^2-z^2-1", this);
  QVERIFY( !s2.isEmpty() );
  QVERIFY( !s2.hasError() );

  srtSurface s3("x^2+y^2-z^2-goof", this);
  QVERIFY( !s3.isEmpty() );
  QVERIFY( s3.hasError() );
}

void srtSurface_test::equationProperty()
{
  srtSurface s1(this);

  s1.setEquation();
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );
  
  s1.setEquation("x^2+y^2-z^2-1");
  QVERIFY( !s1.isEmpty() );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.errorString().isEmpty() );

  s1.setEquation("x^2+y^2-z^2-goof");
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.hasError() );
  QVERIFY( !s1.errorString().isEmpty() );
  QVERIFY( s1.errorIndex() == 12 );
}

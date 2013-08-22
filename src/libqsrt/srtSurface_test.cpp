#warning copyright info

#include <QtTest/QtTest>

#include "srtScene.h"
#include "srtSurface_test.h"

void srtSurface_test::increaseCounter()
{
  counter++;
}


void srtSurface_test::benchmark_JITinitialization()
{
  QBENCHMARK {
    srtSurface s1(this);
  }
}


void srtSurface_test::benchmark_JITcompilation()
{
  srtSurface s1(this);

  QBENCHMARK {
    s1.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2+0");
    s1.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  }
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

  srtSurface s2("x^2+y^2-z^2-1", 0.0, this);
  QVERIFY( !s2.isEmpty() );
  QVERIFY( !s2.hasError() );

  srtSurface s3("x^2+y^2-z^2-goof", 0.0, this);
  QVERIFY( !s3.isEmpty() );
  QVERIFY( s3.hasError() );
}


void srtSurface_test::equationProperty()
{
  // Newly constructed equations should be empty.
  srtSurface s1(this);
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.getEquation().isEmpty() );  

  // Count number of times that the surface changes
  connect(&s1, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  // Set correct equation
  QString eq("x^2+y^2-z^2-a");
  s1.setEquation(eq);
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.getEquation() == eq );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.errorString().isEmpty() );
  QVERIFY( s1.getEquation() == eq );

  // Setting same equation one more time should not increase the counter
  s1.setEquation(eq);

  // Set empty equation.
  s1.setEquation();
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.getEquation().isEmpty() );  

  // Set wrong equation
  eq = "x^2+y^2-z^2-goof";
  s1.setEquation(eq);
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.getEquation() == eq );
  QVERIFY( s1.hasError() );
  QVERIFY( !s1.errorString().isEmpty() );
  QVERIFY( s1.getEquation() == eq );

  // Setting same equation one more time should not increase the counter
  s1.setEquation("x^2+y^2-z^2-goof");

  // Check counter
  QVERIFY( counter == 3 );
}


void srtSurface_test::aProperty()
{
  // Newly constructed equations should be empty.
  srtSurface s1("x", 1.0, this);
  QVERIFY( s1.getA() == 1.0 );

  // Count number of times that the surface changes
  connect(&s1, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  // Set correct equation
  qreal a = 1.5;
  s1.setA(a);
  QVERIFY( s1.getA() == a );

  // Check counter
  QVERIFY( counter == 1 );
}
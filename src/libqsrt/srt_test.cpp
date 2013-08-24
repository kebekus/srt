#warning copyright info 

#include <QtTest/QtTest>

#include "srtScene.h"
#include "srt_test.h"


void srt_test::parser_errorReporting()
{
  srtSurface s1;

  // Check in examples that errors in the equation are reported at the correct
  // position
  s1.setEquation("x^2+y^2-z^2-goof");
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.hasError() );
  QVERIFY( !s1.errorString().isEmpty() );
  QVERIFY( s1.errorIndex() == 12 );

  s1.setEquation("(a*sqrt(5))^2 * (1 + goof)");
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.hasError() );
  QVERIFY( !s1.errorString().isEmpty() );
  QVERIFY( s1.errorIndex() == 21 );
}


void srt_test::parser_floatParsing()
{
  // At one point, the equation parser was not able to distinguish between the
  // constants 0 and 0.5. Check that this works correctly now.
  srtScene scene;
  scene.surface.setEquation("x^2+z^2-y^2+0");
  QImage img1 = scene.draw( QSize(100,100) );
  scene.surface.setEquation("x^2+z^2-y^2+0.5");
  QImage img2 = scene.draw( QSize(100,100) );
  scene.surface.setEquation("x^2+z^2-y^2+1/2");
  QImage img3 = scene.draw( QSize(100,100) );
  QVERIFY( !(img1 == img2) );
  QVERIFY( !(img1 == img3) );
  QVERIFY( img2 == img3 );
}


void srt_test::parser_constantA()
{
  // At one point, the equation parser was not able to distinguish between the
  // constants 0 and 0.5. Check that this works correctly now.
  srtScene scene;
  scene.surface.setEquation("x^2+z^2-y^2+1");
  QImage img1 = scene.draw( QSize(100,100) );

  scene.surface.setEquation("x^2+z^2-y^2+2");
  QImage img2 = scene.draw( QSize(100,100) );

  scene.surface.setEquation("x^2+z^2-y^2+a");
  scene.surface.setA(1.0);
  QImage img3 = scene.draw( QSize(100,100) );

  QVERIFY( !(img1 == img2) );
  QVERIFY( img1 == img3 );
}


void srt_test::multipleSurfaces()
{
  srtScene scene1;
  scene1.surface.setEquation("x^2+z^2-y^2+1");
  QImage img1 = scene1.draw( QSize(100,100) );

  srtScene scene2;
  scene2.surface.setEquation("x^2+z^2-y^2+2");
  QImage img2 = scene1.draw( QSize(100,100) );
  
  QVERIFY( img1 == img2 );
}

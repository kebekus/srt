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

#include <QtTest/QtTest>

#include "srtQt/Scene.h"
#include "test_Surface.h"


void Surface_test::increaseCounter()
{
  counter++;
}


void Surface_test::benchmark_JITinitialization()
{
  QBENCHMARK {
    srtQt::Surface s1(this);
  }
}


void Surface_test::benchmark_JITcompilation()
{
  srtQt::Surface s1(this);

  QBENCHMARK {
    s1.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2+0");
    s1.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  }
}


void Surface_test::specialStates()
{
  srtQt::Scene scene;
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


void Surface_test::constructor()
{
  srtQt::Surface s1(this);
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );

  srtQt::Surface s2("x^2+y^2-z^2-1", 0.0, this);
  QVERIFY( !s2.isEmpty() );
  QVERIFY( !s2.hasError() );

  srtQt::Surface s3("x^2+y^2-z^2-goof", 0.0, this);
  QVERIFY( !s3.isEmpty() );
  QVERIFY( s3.hasError() );
}


void Surface_test::equationProperty()
{
  // Newly constructed equations should be empty.
  srtQt::Surface s1(this);
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.equation().isEmpty() );  

  // Count number of times that the surface changes
  connect(&s1, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  // Set correct equation
  QString eq("x^2+y^2-z^2-a");
  s1.setEquation(eq);
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.equation() == eq );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.errorString().isEmpty() );
  QVERIFY( s1.equation() == eq );

  // Setting same equation one more time should not increase the counter
  s1.setEquation(eq);

  // Set empty equation.
  s1.setEquation();
  QVERIFY( s1.isEmpty() );
  QVERIFY( !s1.hasError() );
  QVERIFY( s1.equation().isEmpty() );  

  // Set wrong equation
  eq = "x^2+y^2-z^2-goof";
  s1.setEquation(eq);
  QVERIFY( !s1.isEmpty() );
  QVERIFY( s1.equation() == eq );
  QVERIFY( s1.hasError() );
  QVERIFY( !s1.errorString().isEmpty() );
  QVERIFY( s1.equation() == eq );

  // Setting same equation one more time should not increase the counter
  s1.setEquation("x^2+y^2-z^2-goof");

  // Check counter
  QVERIFY( counter == 3 );
}


void Surface_test::aProperty()
{
  srtQt::Surface s1("x", 1.0, this);
  QVERIFY( s1.a() == 1.0 );

  // Count number of times that the surface changes
  connect(&s1, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  // Set correct equation
  qreal a = 1.5;
  s1.setA(a);
  QVERIFY( s1.a() == a );

  // Check counter
  QVERIFY( counter == 1 );
}


void Surface_test::conversionQVariant()
{
  // Create two different surfaces
  srtQt::Surface s1("x", 1.0, this);
  srtQt::Surface s2(this);
  QVERIFY( s1 != s2 );

  // Count number of times that the surface changes
  connect(&s2, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  QVariant var=s1;
  s2.load(var);
  QVERIFY( s1 == s2 );
  QVERIFY( counter == 0 );
}

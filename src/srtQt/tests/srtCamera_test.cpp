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

#include <math.h>
#include <QtTest/QtTest>

#include "srtQt/Camera.h"
#include "srtCamera_test.h"


void Camera_test::increaseCounter()
{
  counter++;
}


void Camera_test::position()
{
  srtQt::Camera c1(this);
  connect(&c1, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  c1.setPosition( QVector3D(1,2,3) );
  c1.translate( QVector3D(1,2,3) );
  c1.translate( QVector3D() );
  QVERIFY( c1.position() == 2*QVector3D(1,2,3) );
  QVERIFY( counter == 2 );
}


void Camera_test::directions()
{
  srtQt::Camera c1(this);
  connect(&c1, SIGNAL(changed()), this, SLOT(increaseCounter()));
  counter = 0;

  c1.setViewDirection( QVector3D(1,2,3) );
  QVERIFY( fabs(QVector3D::crossProduct(c1.upwardDirection(), c1.viewDirection()).length() - 1.0) < 1e-6 );

  c1.setViewDirection( c1.upwardDirection() );
  QVERIFY( fabs(QVector3D::crossProduct(c1.upwardDirection(), c1.viewDirection()).length() - 1.0) < 1e-6 );

  c1.setUpwardDirection( QVector3D(1,2,3) );
  QVERIFY( fabs(QVector3D::crossProduct(c1.upwardDirection(), c1.viewDirection()).length() - 1.0) < 1e-6 );

  c1.setUpwardDirection( c1.viewDirection() );
  QVERIFY( fabs(QVector3D::crossProduct(c1.upwardDirection(), c1.viewDirection()).length() - 1.0) < 1e-6 );

  c1.setView( QVector3D(1,2,3), QVector3D(2,2,3) );
  QVERIFY( fabs(QVector3D::crossProduct(c1.upwardDirection(), c1.viewDirection()).length() - 1.0) < 1e-6 );

  QVERIFY( counter == 5 );
}


void Camera_test::conversionQVariant()
{
  // Create two different surfaces
  srtQt::Camera c1(this);
  srtQt::Camera c2(this);

  // Make c1 and c2 unequal
  c2.setPosition( QVector3D(0,0,0) );
  QVERIFY( c1 != c2 );

  // Write c1 to QVariant, read c2 from QVariant, the two should then be equal
  QVariant var1 = c1;
  QVERIFY(c2.load(var1));
  QVERIFY(c1 == c2);

  // Read c2 from nonsense QVariant, this should not change a thing
  QVariant var2 = "arglebargle";
  QVERIFY(!c2.load(var2));
  QVERIFY( c1 == c2 );
}


void Camera_test::serialization()
{
  // Create two different surfaces
  srtQt::Camera c1(this);
  srtQt::Camera c2(this);

  // Make c1 and c2 unequal
  c2.setPosition( QVector3D(0,0,0) );
  QVERIFY( c1 != c2 );

  // Write c1 to file, read c2 from file, the two should then be equal
  QTemporaryFile file;
  file.open();
  QDataStream out(&file);
  out << c1;
  file.close();

  file.open();
  QDataStream in(&file);
  in >> c2;
  file.close();

  QVERIFY(c1 == c2);

  // Read c2 from nonsense file, this should not change a thing
  file.open();
  out << "arglebargle";
  file.close();

  file.open();
  in >> c2;
  file.close();

  QVERIFY(c1 == c2);
}

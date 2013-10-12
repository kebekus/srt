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

#include <QCoreApplication>
#include <QTest>

#include "test_Camera.h"
#include "test_srt.h"
#include "test_Surface.h"


int main(int argc, char *argv[]){
  QCoreApplication app(argc, argv);

  int failures = 0;

  srt_test test1;
  failures += QTest::qExec(&test1, argc, argv);

  Camera_test test2;
  failures += QTest::qExec(&test2, argc, argv);

  Surface_test test3;
  failures += QTest::qExec(&test3, argc, argv);

  return failures;
}

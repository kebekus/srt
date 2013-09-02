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

#ifndef SRTWIDGET
#define SRTWIDGET

#include <QFrame>
#include <QPointer>

#include "srtScene.h"

class srtWidget : public QFrame
{
    Q_OBJECT

 public:
  srtWidget(QWidget *parent = 0);

  ~srtWidget();

  void setScene(srtScene *_scene);

  // Re-implemented event handlers
  bool event(QEvent *event);
  void paintEvent (QPaintEvent * event );
  void mousePressEvent(QMouseEvent *event );
  void mouseMoveEvent(QMouseEvent *event );

 private:
  QPointer<srtScene> scene;

  // Used in mouse rotation
  int originalXPos;
  int originalYPos;
};

#endif

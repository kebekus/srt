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

#ifndef COORDIANTEWIDGET_H
#define COORDIANTEWIDGET_H

#include <QGLWidget>

namespace srtQt {

class CoordinateWidget : public QGLWidget
{
  Q_OBJECT;

 public:
  CoordinateWidget(QWidget *parent = 0);
  ~CoordinateWidget();
  
  QColor backgroundColor() const {
    return _backgroundColor;
  }

 public slots:
  void setArrowVectors(QVector3D redArrow, QVector3D greenArrow, QVector3D blueArrow);  
  void setBackgroundColor(QColor backgroundColor);
  
 private:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  GLuint makeArrow(QVector3D tip, QColor tipColor);

  // Background color
  QColor _backgroundColor;

  // Vectors for the arrows, and IDs for the arrow objects
  QVector3D _redArrow;
  GLuint    _redArrowID;

  QVector3D _greenArrow;
  GLuint    _greenArrowID;

  QVector3D _blueArrow;
  GLuint    _blueArrowID;
};


} // namespace srtQt

#endif

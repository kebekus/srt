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

#include <QDebug>
#include <QPainter>

#include "srtWidget.h"


srtWidget::srtWidget(QWidget *parent)
  : QFrame(parent)
{
}


srtWidget::~srtWidget()
{
}


void srtWidget::setScene(srtScene *_scene)
{
  scene = _scene;
  if (scene.isNull())
    return;

  connect(scene, SIGNAL(changed()), this, SLOT(update()));
}


void srtWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  
  painter.fillRect( QRectF(frameRect().left()+frameWidth(), frameRect().top()+frameWidth(), frameRect().width()-2*frameWidth(), frameRect().height()-2*frameWidth()),
		    Qt::black);

  if (!scene.isNull())
    if (!scene->surface.isEmpty() && !scene->surface.hasError()) {
      QImage img = scene->draw( QSize(frameRect().width()-2*frameWidth(), frameRect().height()-2*frameWidth()) );
      if (!img.isNull())
	painter.drawImage( QPoint(frameWidth(), frameWidth()), img);
    }

  painter.end();
  QFrame::paintEvent(event);
}

#warning set minimum size (100x100)

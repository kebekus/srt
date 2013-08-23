#warning copyright missing

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

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
  
#warning libsrt can only draw square images. This is a temporary workaround. Fix this.
  painter.fillRect( QRectF(frameRect().left()+frameWidth(),
			   frameRect().top()+frameWidth(), 
			   frameRect().width()-2*frameWidth(),
			   frameRect().height()-2*frameWidth() ), Qt::black);

  if (!scene.isNull())
    if (!scene->surface.isEmpty() && !scene->surface.hasError()) {
      int size = qMin( frameRect().width()-2*frameWidth(), frameRect().height()-2*frameWidth() );
      QImage img = scene->draw( QSize(size, size) );
      painter.drawImage( QPoint(frameWidth() + ((frameRect().width()-2*frameWidth())-size)/2  , frameWidth() + ((frameRect().height()-2*frameWidth())-size)/2), img);
    }

  painter.end();
  QFrame::paintEvent(event);
}
